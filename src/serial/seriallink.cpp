#include "seriallink.h"
#include "serialframe.h"

#define NO_EEPROM

SerialLink::SerialLink(Device *dev) :
    m_device(dev),
    mUartLinkVersion(UARTLINK_PROTOCOL_VERSION),
    mProductId(0x23420000),
    mProductName("SerialLink"),
    mProductVersion(0x0100),
    mCompilationDate(__DATE__),
    mCompilationTime(__TIME__),
    mEepromInitialized(false)
{
//    if (m_device->isSequential())
//        m_device = new SerialFrame(m_device);
    m_device->onReadyRead = EVENT(&SerialLink::onDataReceived);
}

void SerialLink::setProductInfo(unsigned long productId, string productName, unsigned short productVersion)
{
    mProductId = productId;
    mProductName = productName;
    mProductVersion = productVersion;
}

void SerialLink::registerParam(string name, void *ptr, size_t size, ParamFlags flags)
{
    SUartParam p = {
        .size = (uint8_t)size,
        .flags = flags,
        .virtAddr = 0
    };
    if (mParamVector.empty())
        p.virtAddr = 0;
    else
        p.virtAddr = mParamVector.back().virtAddr + ((mParamVector.back().size+1)>>1);
    int slen = name.length();
    for (int i=0; i<8; i++)
        p.name[i] = (i < slen)? name[i]: '\0';
    mParamVector.push_back(p);
    mParamPtrVector.push_back(reinterpret_cast<unsigned long>(ptr));
}

void SerialLink::registerFunc(string name, DataEvent func)
{
    SUartFunc f;
    int slen = name.length();
    for (int i=0; i<8; i++)
        f.name[i] = (i < slen)? name[i]: '\0';
    mFuncVector.push_back(f);
    mEventVector.push_back(func);
}
//---------------------------------------------------------------------------

void SerialLink::onDataReceived()
{
    ByteArray ba = m_device->readAll();

    Header hdr = *reinterpret_cast<const Header*>(ba.data());
    const char *data = ba.data() + sizeof(Header);
    unsigned char idx = data[0];

    ByteArray out(ba.data(), sizeof(Header));
    out[1] = 0;
    Header *outHdr = reinterpret_cast<Header*>(out.data());
    outHdr->response = 1;

    switch (hdr.cmd)
    {
      case cmdEcho:
        break;

      case cmdInfo:
        out.append(idx);
        switch (idx)
        {
          case icmdProductId:
            out.append(&mProductId, sizeof(unsigned long));
            break;
          case icmdProductName:
            out.append(mProductName.c_str(), mProductName.size()+1);
            break;
          case icmdProductVersion:
            out.append(&mProductVersion, sizeof(unsigned short));
            break;
          case icmdCompilationDate:
            out.append(mCompilationDate.c_str(), mCompilationDate.size()+1);
            break;
          case icmdCompilationTime:
            out.append(mCompilationTime.c_str(), mCompilationTime.size()+1);
            break;
          case icmdResources:
            out.append((char)mParamVector.size());
            out.append((char)mFuncVector.size());
            break;
        }
        break;

      case cmdUartLinkVersion:
        out.append(&mUartLinkVersion, sizeof(unsigned short));
        break;

      case cmdGetParamInfo:
        out.append(idx);
        if (idx < mParamVector.size())
            out.append(&mParamVector[idx], sizeof(SUartParam));
        else
            outHdr->error = true;
        break;

      case cmdGetParam:
        out.append(idx);
        if (idx < mParamVector.size())
        {
            const SUartParam &p = mParamVector[idx];
            if (p.flags & pfRead)
                out.append(reinterpret_cast<const char *>(mParamPtrVector[idx]), p.size);
            else
                outHdr->error = true;
        }
        else
            outHdr->error = true;
        break;

      case cmdSetParam:
        out.append(idx);
        if (idx < mParamVector.size())
        {
            const SUartParam &p = mParamVector[idx];
            if ((p.flags & pfWrite) && (p.size == (ba.size() - 1 - sizeof(Header))))
            {
                unsigned char *ptr = reinterpret_cast<unsigned char*>(mParamPtrVector[idx]);
                for (int i=0; i<p.size; i++)
                    ptr[i] = data[i+1];
            }
        }
        else
            outHdr->error = true;
        break;

      case cmdGetFuncInfo:
        out.append(idx);
        if (idx < mFuncVector.size())
            out.append(&mFuncVector[idx], sizeof(SUartFunc));
        else
            outHdr->error = true;
        break;

      case cmdCallFunc:
        out.append(idx);
        if (idx < mFuncVector.size())
        {
            ByteArray exchangeBa(data+1, ba.size()-1-sizeof(Header));
            mEventVector[idx](exchangeBa);
                out.append(exchangeBa);
        }
        else
            outHdr->error = true;
        break;
    }

    m_device->write(out);
}
//---------------------------------------------------------------------------

void SerialLink::sendParam(string name)
{
    int len = mParamVector.size();
    for (int i=0; i<len; i++)
    {
        const SUartParam &p = mParamVector[i];
        if (p.name == name)
        {
            ByteArray out;
            out.append('\0');
            out.append('\0');
            Header *outHdr = reinterpret_cast<Header*>(out.data());
            outHdr->cmd = cmdGetParam;
            outHdr->response = 1;
            out.append((unsigned char)i);
            if (p.flags & pfRead)
            {
                out.append(reinterpret_cast<const char *>(mParamPtrVector[i]), p.size);
                m_device->write(out);
            }
            break;
        }
    }
}
//---------------------------------------------------------------------------

void SerialLink::restoreParams()
{
#if !defined(NO_EEPROM)
    if (!mEepromInitialized)
    {
        uint16_t virtAddrCnt = mParamVector.back().virtAddr + ((mParamVector.back().size+1)>>1);
        EE_Init(virtAddrCnt);
        mEepromInitialized = true;
    }
    else
    {
        FLASH_Unlock();
    }

    int cnt = mParamVector.size();
    for (int i=0; i<cnt; i++)
    {
        SUartParam &p = mParamVector[i];
        if (!(p.flags & pfPermanent))
            continue;
        uint16_t va = p.virtAddr;
        int sz = (p.size + 1) >> 1;
        for (int j=0; j<sz; j++)
        {
            EE_ReadVariable(va + j, &reinterpret_cast<uint16_t*>(mParamPtrVector[i])[j]);
        }
    }
    FLASH_Lock();
#endif
}

void SerialLink::storeParams()
{
#if !defined(NO_EEPROM)
    if (!mEepromInitialized)
    {
        FLASH_Unlock();
        uint16_t virtAddrCnt = mParamVector.back().virtAddr + ((mParamVector.back().size+1)>>1);
        EE_Init(virtAddrCnt);
        mEepromInitialized = true;
    }
    else
    {
        FLASH_Unlock();
    }

    int cnt = mParamVector.size();
    for (int i=0; i<cnt; i++)
    {
        SUartParam &p = mParamVector[i];
        if (!(p.flags & pfPermanent))
            continue;
        uint16_t va = p.virtAddr;
        int sz = (p.size + 1) >> 1;
        for (int j=0; j<sz; j++)
        {
            EE_WriteVariable(va + j, reinterpret_cast<uint16_t*>(mParamPtrVector[i])[j]);
        }
    }

    FLASH_Lock();
#endif
}
//---------------------------------------------------------------------------
