#ifndef _QDEBUG_H
#define _QDEBUG_H

#include <vector>
#include <string>
#include <sstream>

class Debug
{    
public:
    typedef void (*OutFn_t)(const char *);
  
    static Debug *instance() 
    {
        if (!mSelf)
            mSelf = new Debug;
//        mSelf->seq++;
//        mSelf->strings.push_back(std::string());
        return mSelf;
    }
    
    template <class T>
    Debug &operator <<(T v)
    {
        std::stringstream ss;
        ss << v;
        mOutFn(ss.str().c_str());
//        strings[seq] += " " + ss.str();
        return *this;
    }
    
    static void setOutputFunc(OutFn_t f);
      
private:
    static Debug *mSelf;
    static OutFn_t mOutFn;
//    std::vector<std::string> strings;
//    int seq;
    
//    Debug() : seq(-1) {}     
};

extern Debug &qDebug();

#endif