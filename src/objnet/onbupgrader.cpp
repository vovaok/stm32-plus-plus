#include "onbupgrader.h"

using namespace Objnet;

OnbUpgrader::OnbUpgrader(ObjnetMaster *master) :
    mMaster(master),
    mState(sIdle),
    mSize(0), mPageSize(0),
    mAppinfoIdx(0),
    mCount(0), mCurBytes(0),
    mPageDone(false), mPageTransferred(false), mPageRepeat(false)
{
    logEvent = EVENT(&OnbUpgrader::_internal_log);

    mTimer = new Timer();
#ifdef QT_CORE_LIB
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
#else
    mTimer->setTimeoutEvent(EVENT(&OnbUpgrader::onTimer));
#endif

    mMaster->onServiceMessage = EVENT(&OnbUpgrader::onMessage);
}

OnbUpgrader::~OnbUpgrader()
{
    stop();
    delete mTimer;
    mMaster->onServiceMessage = Closure<void(CommonMessage&)>();
}

void OnbUpgrader::load(const void *firmware, int size)
{
    if (mState != sIdle)
        return;
    
    mData = reinterpret_cast<const unsigned char*>(firmware);
    mSize = size;
    
    mCount = 0;
    mPageDone = false;
    mPageTransferred = false;
    mPageRepeat = false;

    logEvent("firmware size = " + _number(mSize) + " bytes");

    unsigned char s[] = "__APPINFO__";
    for (int i=0; i<=mSize-11; i++)
    {
        int j;
        for (j=0; j<11 && (mData[i+j]==s[j]); j++);
        if (j==11)
        {
            mAppinfoIdx = i;
            break;
        }
    }
    
    if (mAppinfoIdx < 0)
    {
        logEvent("no app info supplied. abort");
        return;
    }

    mInfo = *reinterpret_cast<const __appinfo_t__*>(mData + mAppinfoIdx);
    logEvent(getFirmwareInfo());
    mClass = mInfo.cid;
    mPageSize = mInfo.pageSize;
    if (mPageSize < 8)
        mPageSize = 2048; // HACK
    mInfo.length = (mSize+3) & ~3L;
    mInfo.checksum = 0;
    uint32_t cs = 0;
    const uint32_t *data = reinterpret_cast<const uint32_t*>(mData);
    for (int i=0; i<mSize/4; i++)
        cs -= data[i];
    unsigned long lastdata = data[mSize/4] & ((1<<((mSize & 3)*8))-1);
    cs -= lastdata;
    cs -= mInfo.length;
    cs += 0xDeadFace + 0xBaadFeed;
    mInfo.checksum = cs;
    logEvent("checksum = " + _number(mInfo.checksum, true) + "\n");
}

void OnbUpgrader::scan(unsigned char netaddr)
{
    mState = sIdle;
    mDevices.clear();
  
    ByteArray ba;
    ba.append(reinterpret_cast<const char*>(&mClass), 4);
    if (netaddr)
        mMaster->sendServiceRequest(netaddr, svcUpgradeRequest, ba);
    else
        mMaster->sendGlobalRequest(aidUpgradeStart, true, ba);
    logEvent("start upgrade devices with class = " + _number(mClass, true) + "\n");
    mTimer->start(4000);
}

void OnbUpgrader::start()
{
    uint32_t size_aligned = (mSize + 3) & ~3L;
    ByteArray ba;
    ba.append(reinterpret_cast<const char*>(&size_aligned), 4);
    sendMessageToDevices(svcUpgradeConfirm, ba);
    //mMaster->sendGlobalRequest(aidUpgradeConfirm, true, ba);
    mState = sStarted;
    mTimer->start(100);
}

void OnbUpgrader::stop()
{
    mState = sIdle;
    mDevices.clear();  
    mTimer->stop();
    mMaster->sendGlobalRequest(aidUpgradeEnd, true);
    logEvent("upgrade finished\n");
#ifdef QT_CORE_LIB
    emit finished();
#endif
}
//---------------------------------------------------------------------------

void OnbUpgrader::onTimer()
{
    mTimer->setInterval(16);

    if (mState == sIdle)
    {
        if (!mDevices.size())
        {
            mTimer->stop();
            logEvent("no devices to upgrade :c\n");
            return;
        }

        logEvent("found " + _number(mDevices.size()) + " device(s)\n\n");
        mTimer->stop();
        return;
    }
    else if (mState == sStarted)
    {
        if (isAllReady())
        {
            clearReady();
            logEvent("all of devices are ready\n");
            mCount = 0;
            mPageDone = mPageTransferred = mPageRepeat = false;
            setPage(0);
        }
    }
    else if (mState == sSetPage)
    {
        if (isAllReady())
        {
            mState = sWork;
        }
        else
        {
            int page = mCount / mPageSize;
            ByteArray ba;
            ba.append(reinterpret_cast<const char*>(&page), 4);
            sendMessageToUnreadyDevices(svcUpgradeSetPage, ba);
        }
    }
    else if (mState == sWork)
    {
        if (mPageSize < 8) // backwards compatibility
            mPageSize = 2048;

        if (mPageTransferred)
        {
            if (!mPageDone && !mPageRepeat)
            {
                sendMessageToDevices(svcUpgradeProbe);
                //mMaster->sendGlobalRequest(aidUpgradeProbe, true);
                logEvent("no response, probe request...\n");
                mTimer->setInterval(200);
                return;
            }
            else
            {
                mPageTransferred = false;
                if (mPageRepeat && mCount)
                    mCount = ((mCount - 1) / mPageSize) * mPageSize;
                int page = mCount / mPageSize;

                if (mCount < mSize)
                    setPage(page);
                else
                    mState = sFinish;
                return;
            }
        }

        mPageDone = false;
        mPageRepeat = false;
        mCurBytes = 0;

        mState = sTransferPage;
        mTimer->setInterval(16);
        
#ifdef QT_CORE_LIB
        emit progressChanged(progress());
#endif
    }
    else if (mState == sTransferPage)
    {
        mTimer->setInterval(16);

        int seqcnt = mPageSize >> 3;
        int maxseqs = 128 >> 3;
        logEvent(".");
        for (int i=0; i<maxseqs; i++)
        {
            ByteArray ba(8, '\0');
            for (int i=0; i<8; i++)
            {
                int ci = mCount + i;
                int ai = ci - mAppinfoIdx;
                if (ai >= 0 && ai < sizeof(__appinfo_t__))
                    ba[i] = reinterpret_cast<unsigned char*>(&mInfo)[ai];
                else if (ci < mSize)
                    ba[i] = mData[ci];
            }
            int basz = ba.size();
            int seq = (mCount >> 3) & (seqcnt - 1);
            mMaster->sendUpgrageData(seq, ba);
            mCount += basz;
            mCurBytes += basz;
        }

        if (mCurBytes >= mPageSize || mCount >= mSize) // page transferred
            mState = sEndPage;
    }
    else if (mState == sEndPage)
    {
        mTimer->setInterval(200); // wait ACK
        mPageTransferred = true;
        sendMessageToDevices(svcUpgradeProbe);
        //master->sendGlobalRequest(aidUpgradeProbe, true);
        mState = sWork;
    }
    else if (mState == sFinish)
    {
        stop();
    }
    else if (mState == sError)
    {
        logEvent("ERROR!!!\n");
        mTimer->stop();
    }
}

void OnbUpgrader::onMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {
        return;
    }
    
    unsigned char addr = msg.localId().sender;
    unsigned char oid = msg.localId().oid;

    if (!mDevices.count(addr) && oid != svcUpgradeAccepted)
        return;
    
    switch (oid)
    {
      case svcUpgradeAccepted:
        mDevices[addr] = false;
        logEvent("+1 ");
        break;
        
      case svcUpgradeReady:
        mDevices[addr] = true;
        logEvent("+1 ");
        break;

      case svcUpgradeSetPage:
        mDevices[addr] = true;
        if (isAllReady())
            mTimer->start(16);
        break;
        
      case svcUpgradeRepeat:
        mPageRepeat = true;
      case svcUpgradePageDone:
        mDevices[addr] = true;
        logEvent("+1 ");
        if (isAllReady())
        {
            if (mPageRepeat)
            {
                logEvent("FAIL! repeat page\n");
            }
            else
            {
                mPageDone = true;
                logEvent("success\n");
            }
            mTimer->start(16);
        }
        break;
        
      default:
        logEvent("unknown response " + _number(oid) + "\n");
    }
}

void OnbUpgrader::sendMessageToDevices(SvcOID oid, const ByteArray &ba)
{
    for (DevIterator it=mDevices.begin(); it!=mDevices.end(); it++)
        mMaster->sendServiceRequest(it->first, oid, ba);
}

void OnbUpgrader::sendMessageToUnreadyDevices(SvcOID oid, const ByteArray &ba)
{
    for (DevIterator it=mDevices.begin(); it!=mDevices.end(); it++)
        if (!it->second)
            mMaster->sendServiceRequest(it->first, oid, ba);
}

void OnbUpgrader::setPage(int page)
{
    clearReady();
    ByteArray ba;
    ba.append(reinterpret_cast<const char*>(&page), 4);
    logEvent("page " + _number(page) + " of " + _number(pageCount()) + " ...");
    sendMessageToDevices(svcUpgradeSetPage, ba);
    mState = sSetPage;
    mTimer->start(200);
    //mMaster->sendGlobalRequest(aidUpgradeSetPage, true, ba);
}

void OnbUpgrader::clearReady()
{
    for (DevIterator it=mDevices.begin(); it!=mDevices.end(); it++)
        it->second = false;
}

bool OnbUpgrader::isAllReady()
{
    for (DevIterator it=mDevices.begin(); it!=mDevices.end(); it++)
        if (!it->second)
            return false;
    return true;
}
//---------------------------------------------------------------------------

bool OnbUpgrader::checkClass(const ByteArray &firmware, uint32_t cid)
{
    int idx = firmware.indexOf("__APPINFO__");
    if (idx < 0)
        return false;
    const __appinfo_t__ *info = reinterpret_cast<const __appinfo_t__*>(firmware.data()+idx);
    return (info->cid == cid);
}
//---------------------------------------------------------------------------

string OnbUpgrader::_number(int num, bool hex)
{
    char buf[16];
    if (hex)
        sprintf(buf, "0x%08X", (uint32_t)num);
    else
        sprintf(buf, "%d", num);
    return string(buf);
}

string OnbUpgrader::getFirmwareInfo() const
{
    string text;
    char buf[64];
    sprintf(buf, "class id = 0x%08X\n", (unsigned int)mInfo.cid);
    text += buf;
    sprintf(buf, "version %d.%d, %s\n", mInfo.ver >> 8, mInfo.ver & 0xFF, mInfo.timestamp);
    text += buf;
    sprintf(buf, "page size = %d\n", mInfo.pageSize);
    text += buf;
    return text;
}
//---------------------------------------------------------------------------
