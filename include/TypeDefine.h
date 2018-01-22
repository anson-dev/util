#ifndef GAMELIVE_TYPEDEFINE_H_
#define GAMELIVE_TYPEDEFINE_H_

namespace HuyaUtil {
	
	// ¼ÇÂ¼ÐÐÎªÊä³ö
	#ifndef  LOG_FUNCTION_INTER_WITH_UID
	#define  LOG_FUNCTION_INTER_WITH_UID( classname ,uid )  \
			 LOG->debug() << classname <<"::"<< __FUNCTION__  << " |enter | uid :=" << uid  <<  endl 
	#endif
	
	#ifndef  LOG_FUNCTION_INTER
	#define  LOG_FUNCTION_INTER( classname )  \
			  LOG->debug() << classname <<"::"<< __FUNCTION__  << " |enter" << endl 
	#endif
	

	#ifndef LOG_FUNCTION_LEAVE
	#define LOG_FUNCTION_LEAVE( classname )  \
			LOG->debug() << classname << "::"<< __FUNCTION__  << " |leave" << endl 
	#endif
	
	#ifndef LOG_FUNCTION_LEAVE_WITH_RET
	#define LOG_FUNCTION_LEAVE_WITH_RET(classname ,ret)  \
	LOG->debug() << classname << "::"<< __FUNCTION__  << " |leave return " << ret << endl 
	#endif
	

	#ifndef LOG_EXCEPTION
	#define LOG_EXCEPTION( classname ) \
			LOG->error()  << classname <<"::" << __FUNCTION__<< " err:" << e.what() << endl; 
	#endif
	
	#ifndef LOG_EXCEPTION_RET
	#define LOG_EXCEPTION_RET( classname, ret ) \
	LOG->error()  << ret << " " << classname <<"::" << __FUNCTION__<< " err:" << e.what() << endl; \
	return ret  
	#endif

	#ifndef LOG_EXCEPTION_ALL
	#define LOG_EXCEPTION_ALL( classname, ret ) \
			LOG->error()  << ret << " " << classname <<"::" << __FUNCTION__ << " catch .. error" << endl; \
	return ret 
	#endif 

	#ifndef LOG_EXCEPTION_ALL_NO_RETURN
	#define LOG_EXCEPTION_ALL_NO_RETURN( classname) \
			LOG->error()  << " " << classname <<"::" << __FUNCTION__ << " catch .. error" << endl; 
	#endif 
	
	#ifndef HUYA_TRY
	#define HUYA_TRY try 
	#endif
	
	#ifndef HUYA_CATCH
	#define HUYA_CATCH catch ( std::exception &e)  
	#endif

	#ifndef HUYA_CATCH_ALL
	#define HUYA_CATCH_ALL   catch (...) 
	#endif
	
	// ¼ÇÂ¼Ê±¼äÊä³ö
	#ifndef LOG_TIME_BEGIN
	#define LOG_TIME_BEGIN int64_t time = taf::TC_TimeProvider::getInstance()->getNowMs();
	#endif
	
	#ifndef LOG_TIME_END
	#define LOG_TIME_END    LOG->debug() << __FUNCTION__ << "| time :="<<taf::TC_TimeProvider::getInstance()->getNowMs() - time << endl ; 
	#endif

	#define ELOG_MAX_BUFFER_SIZE 30000

	#define SPRINTF_ELOG_MSG(buffer, fmt, args...) \
    char buffer[ELOG_MAX_BUFFER_SIZE]; \
    snprintf( buffer, ELOG_MAX_BUFFER_SIZE, fmt, ##args );

	#define ELOG_TRACE2( fmt, args...) \
	{ \
		SPRINTF_ELOG_MSG( __tmp, fmt, ##args ); \
		LOG->debug() << "|" << std::string(__FILE__) << "|" << std::string(__FUNCTION__) <<"|" <<  __LINE__ <<"|" << __tmp << std::endl; \
	}

	#define ELOG_DEBUG2(  fmt, args...) \
	{ \
		SPRINTF_ELOG_MSG( __tmp, fmt, ##args ); \
		LOG->debug() << "|" << std::string(__FILE__) << "|" << std::string(__FUNCTION__) <<"|" <<  __LINE__ <<"|" << __tmp << std::endl; \
	} 

	#define ELOG_INFO2( fmt, args...) \
	{ \
		SPRINTF_ELOG_MSG( __tmp, fmt, ##args ); \
		LOG->info() << "|" << std::string(__FILE__) << "|" << std::string(__FUNCTION__) <<"|"  <<  __LINE__ <<"|" << __tmp << std::endl; \
	} 

	#define ELOG_WARN2( fmt, args...) \
	{ \
		SPRINTF_ELOG_MSG( __tmp, fmt, ##args ); \
		LOG->warn() << "|" << std::string(__FILE__) << "|" << std::string(__FUNCTION__)  <<"|" <<	__LINE__ <<"|" << __tmp << std::endl; \
	}        

	#define ELOG_ERROR2( fmt, args...) \
	{ \
		SPRINTF_ELOG_MSG( __tmp, fmt, ##args ); \
		LOG->error() << "|" << std::string(__FILE__) << "|" << std::string(__FUNCTION__)  <<"|" <<	__LINE__ <<"|" << __tmp << std::endl; \
	} 

	#define ELOG_FATAL2( fmt, args...) \
	{ \
		SPRINTF_ELOG_MSG( __tmp, fmt, ##args ); \
		LOG->error() << "|" << std::string(__FILE__) << "|" << std::string(__FUNCTION__)  <<"|" <<	__LINE__ <<"|" << __tmp << std::endl; \
	} 


	#define ELOG_TRACE(fmt, args...) \
    ELOG_TRACE2(   fmt, ##args );

	#define ELOG_DEBUG(fmt, args...) \
    ELOG_DEBUG2(  fmt, ##args );

	#define ELOG_INFO(fmt, args...) \
    ELOG_INFO2(  fmt, ##args );

	#define ELOG_WARN(fmt, args...) \
    ELOG_WARN2(  fmt, ##args );

	#define ELOG_ERROR(fmt, args...) \
    ELOG_ERROR2( fmt, ##args );

	#define ELOG_FATAL(fmt, args...) \
    ELOG_FATAL2(  fmt, ##args );


    #define PROC_BEGIN do{
	#define PROC_END   }while(0);


#define __TRY__ try\
    {

#define __CATCH__ }\
    catch (std::exception const& e)\
    {\
        LOG->error() <<__FILE__<<"::"<< __FUNCTION__ << string(" catch std exception: ") + e.what() << endl;\
    }\
    catch (...)\
    {\
        LOG->error() <<__FILE__<<"::"<< __FUNCTION__<< " catch unknown exception" << endl;\
    }

#define FUN_BEGIN do{\
         try\
         {

#define FUN_END  }\
    catch (std::exception const& e)\
    {\
        LOG->error() <<__FILE__<<"::"<< __FUNCTION__ << string(" catch std exception: ") + e.what() << endl;\
    }\
    catch (...)\
    {\
        LOG->error() <<__FILE__<<"::"<< __FUNCTION__<< " catch unknown exception" << endl;\
    }\
    }while(0);


	// 控制过频繁日志， 
	#define LOG_FREQUENCE_CONTROL(a) \
	static long lTimeMs = TC_TimeProvider::getInstance()->getNowMs(); \
	long lTimeNow = TC_TimeProvider::getInstance()->getNowMs();\
	bool gPrintLog = false; \
    if (lTimeNow - lTimeMs > a) \
	{\
		lTimeMs = lTimeNow; \
		gPrintLog = true; \
	}\

	#define  L_MSG  (string(__FILE__)+"/"+string(__FUNCTION__)+":"+string(TC_Common::tostr(__LINE__))+"|")


	// 监控日志
	#ifndef LOG_RECORD_NOTIFY_EXCEPTION
	#define LOG_RECORD_NOTIFY_EXCEPTION FDLOG("record_upload_exception")
	#endif

	#define __NOW__ taf::TC_TimeProvider::getInstance()->getNow()

	#ifndef NOWTIME
	#define NOWTIME taf::TC_TimeProvider::getInstance()->getNow()
	#endif

	#ifndef NOWTIMEM
	#define NOWTIMEM taf::TC_TimeProvider::getInstance()->getNowMs()
	#endif

	const int CALL_TIMEOUT = 1000 ;
	const int MAX_TRYS     = 3 ;
	const int WAIT_SLEEP_TIME = 50000 ;
	const float EPS = 0.0000001 ;
	const long MAX_USER_TABLE_INDEX = 100 ;

	static const long WEEK_SECODNS = 604800;      //60*60*24*7
	static const long ADDTIONAL_SECONDS = 288000;    //timestamp 0 is 1970-1-1 8:00 in beijing is thursday, 60*60*24*3+60*60*8 

};

#endif
