#ifndef _QDEBUG_H
#define _QDEBUG_H

#include <vector>
#include <string>
#include <sstream>

class Debug
{
private:
    static Debug *mSelf;
    std::vector<std::string> strings;
    int seq;
    
    Debug() : seq(-1) {}
    
public:
    static Debug *instance() 
    {
        if (!mSelf)
            mSelf = new Debug;
        mSelf->seq++;
        mSelf->strings.push_back(std::string());
        return mSelf;
    }
    
    template <class T>
    Debug &operator <<(T v)
    {
        std::stringstream ss;
        ss << v;
        strings[seq] += " " + ss.str();
        return *this;
    }
};

extern Debug &qDebug();

#endif