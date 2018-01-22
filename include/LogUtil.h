#ifndef _LOG_UTIL_H_
#define _LOG_UTIL_H_
#include <log/taf_logger.h>
#include <util/tc_timeprovider.h>


#define __FILE__LINE__ (string(__FILE__) + string("(") + TC_Common::tostr<int>(__LINE__) + string(")"))

//输出请求相关信息
#define LOG_REQ LOG->debug()<<current->getRequestId()<<"|"<<string(__FUNCTION__)<<"|"<<__FILE__LINE__<<"|"
#define FDLOG_REQ(ret, uid) FDLOG()<<(ret)<<"|"<<string(__FUNCTION__)<<"|"<<(uid)<<"|"<<current->getRequestId()<<"|"
//输出文件，行号，函数名
#define LOG_F LOG->debug()<<__FILE__LINE__<<"|"<<__FUNCTION__<<"|"
#define LOG_E LOG->error()<< __FILE__LINE__<<"|" <<__FUNCTION__<< "|"


//记录抛出异常的sql命令
#define LOG_MYSQL_EXCEPTION(e, mysql) (LOG->error()<<"mysql exception|"<<__FILE__LINE__<<"|["<<(mysql)->getLastSQL())<<"]|"<<(e).what()
//记录sql命令执行情况
#define LOG_MYSQL(mysql) (LOG->debug()<<__FILE__LINE__<<"|"<<__FUNCTION__<<"|sql|["<<(mysql)->getLastSQL())<<"]"


#define LOG_ERR LOG->debug()<<__FILE__LINE__<<"|"<<__FUNCTION__<<"|error|"

#define TIME_WARNING_BEGIN(ms) time_t tv_begin; \
                            int threshold_ms = (ms); \
                            tv_begin = TC_TimeProvider::getInstance()->getNowMs();


#define TIME_WARNING_END  time_t tv_end = TC_TimeProvider::getInstance()->getNowMs(); \
    int escaped_ms = tv_end - tv_begin; \
    if(escaped_ms >= threshold_ms) \
    { \
        LOG_F << "escaped ms " << escaped_ms << endl; \
    }


template <typename T> string JCETOSTR(T &t)
{   
    ostringstream os;
    t.displaySimple(os);
    return os.str();
}

template<typename T>  string JCETOSTR(const vector<T> &t)
{ 
    ostringstream os;
    for(unsigned i=0;i<t.size();i++) 
    {
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}

template<typename T>  string JCETOSTR(vector<T> &t)
{ 
    ostringstream os;
    for(unsigned i=0;i<t.size();i++) 
    {
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}

template <typename T> string SIMPLEJCETOSTR(T &t)
{   
    ostringstream os;
    t.displaySimple(os);
    return os.str();
}

template<typename T>  string SIMPLEJCETOSTR(const vector<T> &t)
{ 
    ostringstream os;
    for(unsigned i=0;i<t.size();i++) 
    {
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}

template<typename T>  string SIMPLEJCETOSTR(vector<T> &t)
{ 
    ostringstream os;
    for(unsigned i=0;i<t.size();i++) 
    {
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}


template <typename T> string DETAILJCETOSTR(T &t)
{   
    ostringstream os;
    t.display(os);
    return os.str();
}

template<typename T>  string DETAILJCETOSTR(const vector<T> &t)
{ 
    ostringstream os;
    for(unsigned i=0;i<t.size();i++) 
    {
        os<<DETAILJCETOSTR(t[i]);
        os<<endl;
    }
    return os.str();
}

template<typename T>  string DETAILJCETOSTR(vector<T> &t)
{ 
    ostringstream os;
    for(unsigned i=0;i<t.size();i++) 
    {
        os<<DETAILJCETOSTR(t[i]);
        os<<endl;
    }
    return os.str();
}

inline string logId(taf::JceCurrentPtr current)
{
    map<string, string> &mContext = current->getContext();

    if(mContext.find("logid") != mContext.end())
    {
        return mContext["logid"];
    }

    /**
     * logid组成(位数): 机房id(2) + 机器id(16) + 线程号id(16) + 时间戳(16) + 递增数(14位)
     * 机房: 佛山-0, 无锡-1
     * 时间戳16位，只能记录不到一天的时间，过后就会重复
     * 递增数14位，意味着理论上一个线程一秒请求数不能超过1.6w
     * 其实如果做在底层的话，机器的数量和线程数可以大大减少，因为在接入层就会分配logid
     */
    static int idc = (ServerConfig::Application == "MDWWX") ? 1 : 0;

    static uint32_t ip = (uint32_t)inet_addr(ServerConfig::LocalIp.c_str());

    int thread  = syscall(SYS_gettid);

    unsigned int now = TC_TimeProvider::getInstance()->getNow();

    static __thread unsigned short seq = 0;
    ++seq;

    unsigned long logid = ((unsigned long)idc << 62)
                        + ((unsigned long)(ip & 0x0000FFFF) << 46)        //ip低2段
                        + ((unsigned long)(thread & 0x0000FFFF) << 30)    //线程号
                        + ((now % 0xFFFF) << 14)                          //时间戳
                        + (seq % 0x3FFF);

    mContext["logid"] = TC_Common::tostr(logid);

    return mContext["logid"] + "S";     //发起点加个S
}

class TimeUse
{
public:
    TimeUse(const string& function ) : _sFun ( function )
    {
        _iTime = taf::TC_TimeProvider::getInstance()->getNowMs();
    }
    ~TimeUse()
    {
        int64_t iEnd = taf::TC_TimeProvider::getInstance()->getNowMs();
        LOG->debug() << _sFun << "|costtime=" << iEnd - _iTime << endl;
    }
private:
    string  _sFun;
    int64_t _iTime;
};

#endif
