#ifndef _ONBUPGRADER_H
#define _ONBUPGRADER_H

#include "objnetmaster.h"

namespace Objnet
{

#ifdef QT_CORE_LIB
class OnbUpgrader : public QObject
{
    Q_OBJECT

    #pragma pack(push,1)
    typedef struct
    {
        char const pre[12];
        unsigned long cid;
        unsigned short ver;
        unsigned short pageSize;
        unsigned long length;
        unsigned long checksum;
        char timestamp[25];
    } __appinfo_t__;
    #pragma pack(pop)

#else
class OnbUpgrader
{
#endif
private:
    ObjnetMaster *mMaster;
    Timer *mTimer;
    std::map<unsigned char, bool> mDevices;
    typedef std::map<unsigned char, bool>::iterator DevIterator;
    
    ByteArray mBin;
    __appinfo_t__ *mInfo;
    enum {sIdle, sStarted, sSetPage, sWork, sTransferPage, sEndPage, sFinish, sError} mState;
    int mSize, mPageSize;
    int mCount, mCurBytes;
    bool mPageDone, mPageTransferred, mPageRepeat;
    unsigned long mClass;
    
    static string _number(int num, bool hex=false);
    void _internal_log(string) {}
    
    void sendMessageToDevices(SvcOID oid, const ByteArray &ba = ByteArray());
    void sendMessageToUnreadyDevices(SvcOID oid, const ByteArray &ba = ByteArray());
    void setPage(int page);
    void clearReady();
    bool isAllReady();

#ifdef QT_CORE_LIB
private slots:
#endif
    void onTimer();
    void onMessage(CommonMessage &msg);
  
public:
    OnbUpgrader(ObjnetMaster *master);
    ~OnbUpgrader();
    
    static bool checkClass(const ByteArray &firmware, unsigned long cid);
    
    void load(const ByteArray &firmware);
    void scan(unsigned char netaddr=0);
    void start();
    void stop();
    
    int progress() const {return mSize? mCount * 100 / mSize: 0;}
    int pageCount() const {return mSize / mPageSize;}
    const __appinfo_t__ *firmwareInfo() const {return mInfo;}
    string getFirmwareInfo() const;
    
    Closure<void(string)> logEvent;

#ifdef QT_CORE_LIB
signals:
    void progressChanged(int percent);
    void finished();
#endif
};

}

#endif
