#include "RedisBase.h"
#include "util/tc_common.h"
#include "TypeDefine.h"
#include "log/taf_logger.h"


namespace HuyaUtil {

    const std::string TIMEOUT_HASH_KEY ="timeout_order" ;

       RedisBaseClient::RedisBaseClient(const std::string& ip, const uint32_t& port, const uint32_t& dbindex, const uint32_t& timeout, const std::string& auth)
        :_sIp(ip),_iPort(port),_iDbIndex(dbindex),_iTimeout(timeout),_sAuth(auth),_pClient(NULL),_bConnectFlag(false)
        {
            _lCheckTime     = __NOW__;
            _pClusterProxy  = NULL ;
        }
        
        RedisBaseClient::~RedisBaseClient()
        {
            close();
        }
        
        int  RedisBaseClient::init()
        {
            LOG->debug() << "RedisBaseClient::" << __FUNCTION__ << "| ip :=" << _sIp << "| port :=" << _iPort << "| timeout :=" << _iTimeout << "| name :=" << _sName 
             << "|_sAuth:" << _sAuth << endl ;

            int iRet = -1 ;

            HUYA_TRY
            {
                  struct timeval timeout = { _iTimeout/1000, (_iTimeout%1000)*1000 };
                  _pClient = redisConnectWithTimeout( (char*)_sIp.c_str(), _iPort, timeout);
                  if( _pClient->err )
                  {
                    LOG->error() << iRet << __FUNCTION__ <<  "|ret: " << _pClient->errstr << endl ;
                  }
                  else
                  {
                    iRet = 0 ;
                    _bConnectFlag = true;

                    if (!_sAuth.empty())
                    {
                        redisReply *reply = (redisReply *)redisCommand( _pClient, "auth %s", _sAuth.c_str());
                        iRet = collectLastError( reply );

                        if (iRet != 0)
                        {
                            LOG->error() << iRet << __FUNCTION__ << "auth error :_sAuth" << _sAuth << endl ;
                        }

                        if ( reply != NULL )
                        {
                            freeReplyObject(reply);
                        }
                    }

                    if (_iDbIndex >=0 )
                    {
                        redisReply *reply = (redisReply *)redisCommand( _pClient, "select %d", _iDbIndex );
                        iRet = collectLastError( reply );

                        if (iRet != 0)
                        {
                            LOG->error() << iRet << __FUNCTION__ << "select error :index" << _iDbIndex << endl ;
                        }

                        if ( reply != NULL )
                        {
                            freeReplyObject(reply);
                        }
                    }

                  }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }
        
        void RedisBaseClient::close()
        {
            LOG_FUNCTION_INTER("RedisBaseClient");
            
            HUYA_TRY
            {
                if( _pClient != NULL )
                {
                    redisFree( _pClient);
                    _pClient = NULL ;
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            LOG_FUNCTION_LEAVE("RedisBaseClient");
        }
        
        void RedisBaseClient::preHandConnect()
        {       
            if( !_bConnectFlag )
            {
                  close();
                  init();
            }
        }
        
        int RedisBaseClient::collectLastError(redisReply *reply)
        {
            int iRet = 0 ;
            HUYA_TRY
            {
                if (_pClient != NULL )
                {
                    if (_pClient->err != 0)
                    {
                        LOG->error() << iRet << " " <<  __FUNCTION__ << "| ERR: redis errcode " << _pClient->err <<"[" << _pClient->errstr <<"]" << endl ;
                        _bConnectFlag = false;
                        iRet = -1;
                    }
                    else
                    {
                        if ( reply != NULL && reply->type == REDIS_REPLY_ERROR)
                        {
                            LOG->error() << iRet << " " <<  __FUNCTION__ << "| ERR: redis reply err_str: " << reply->str << endl ;
                            iRet = -1;
                        }
                    }
                }
                else
                {
                    LOG->error() << iRet << " " <<  __FUNCTION__ <<  "ERR: redis not connect" << endl ;
                    _bConnectFlag = false;
                    iRet = -1;
                }
                return iRet;
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }
        
        int RedisBaseClient::checkAlive()
        {
            int ret = 0 ;
            HUYA_TRY
            {
                int64_t now = __NOW__;
                if  ( now - _lCheckTime > 50 && _pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand( _pClient, "PING" );
                
                    ret = collectLastError( reply );
                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }

                    _lCheckTime = now;
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }
            HUYA_CATCH_ALL
            {
                LOG_EXCEPTION_ALL_NO_RETURN("RedisBaseClient");   
            }

            return ret;
        }

        int  RedisBaseClient::doPing()
        {
            int ret = 0 ;
            HUYA_TRY
            {
                if  (_pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand( _pClient, "PING" );
                
                    ret = collectLastError( reply );
                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }
            HUYA_CATCH_ALL
            {
                LOG_EXCEPTION_ALL_NO_RETURN("RedisBaseClient");   
            }

            return ret;
        }

        int  RedisBaseClient::doMaster()
        {
            int iRet = 0 ;
            HUYA_TRY
            {
                if (_pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand( _pClient, "SLAVEOF NO ONE" );
                
                    iRet = collectLastError( reply );
                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        
        int  RedisBaseClient::selectDb(const uint32_t& dbindex)
        {
            int iRet = 0;
            HUYA_TRY
            {
                if (dbindex >=0 && _pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand( _pClient, "select %d", dbindex );
                    iRet = collectLastError( reply );

                    if (iRet != 0)
                    {
                        LOG->error() << iRet << __FUNCTION__ << "select error :index" << dbindex << endl ;
                    }

                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }
                }

            }
            HUYA_CATCH
            {
                iRet = -1 ;
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }
        
        uint32_t  RedisBaseClient::toUint32(std::string input)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                iRet  = TC_Common::strto<uint32_t>(input);
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        redisContext* RedisBaseClient::getRedisContext()
        {
            return _pClient ;
        }

        bool   RedisBaseClient::setRedisProxy(ClusterProxy*  proxy)
        {
            bool bRet = false ;
            HUYA_TRY
            {
                _pClusterProxy = proxy ;
                bRet = true ;
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }
        }

        bool   RedisBaseClient::transBegin()
        {
            bool bRet = true ;
            HUYA_TRY
            {
                if (_pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand(_pClient, "MULTI");
                    int iRet = collectLastError( reply );

                    if (iRet != 0)
                    {
                        bRet = false ;
                        LOG->error() << iRet << __FUNCTION__ << "MULTI error " << endl ;
                    }

                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return bRet ;
        }

        bool   RedisBaseClient::transEnd()
        {
            bool bRet = true ;
            HUYA_TRY
            {
                if (_pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand(_pClient, "EXEC");
                    int iRet = collectLastError( reply );

                    if (iRet != 0)
                    {
                        bRet = false ;
                        LOG->error() << iRet << __FUNCTION__ << "EXEC error " << endl ;
                    }

                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }
                }
            }
            HUYA_CATCH
            {
                bRet = false ;
                LOG_EXCEPTION("RedisBaseClient");
            }

            return bRet ;
        }

        bool   RedisBaseClient::transDiscard()
        {
            bool bRet = true ;
            HUYA_TRY
            {
                if (_pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand(_pClient, " DISCARD");
                    int iRet = collectLastError( reply );

                    if (iRet != 0)
                    {
                        bRet = false ;
                        LOG->error() << iRet << __FUNCTION__ << "DISCARD error " << endl ;
                    }

                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return bRet ;
        }

        bool   RedisBaseClient::watchKey(const std::string& key)
        {
            bool bRet = true ;
            HUYA_TRY
            {
                if (_pClient != NULL)
                {
                    redisReply *reply = (redisReply *)redisCommand(_pClient, "WATCH %s", key.c_str());
                    int iRet = collectLastError( reply );

                    if (iRet != 0)
                    {
                        bRet = false ;
                        LOG->error() << iRet << __FUNCTION__ << "watchKey error " << endl ;
                    }

                    if ( reply != NULL )
                    {
                        freeReplyObject(reply);
                    }
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return bRet ;
        }

        int       RedisBaseClient::checkConnect()
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                checkAlive();
                preHandConnect();
                selectDb(0);

                iRet = 0 ;
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }
        }

        int  RedisBaseClient::addKeyValueCache(const std::string& key, const std::string& val, const int& exprite, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                if (btrans)
                {
                    watchKey(key);
                    transBegin();
                }  

                selectDb(0);
                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "setex %s %d %s", key.c_str(), exprite, val.c_str());

                if (btrans) transEnd();

                iRet = collectLastError( pReply );
                if (iRet != 0)
                {
                    LOG->error() << iRet << "|" << __FUNCTION__ << "|key:" << key << "|val:" << val << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int  RedisBaseClient::getKeyValueCache(const std::string& key, std::string& val, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                if (btrans) 
                { 
                    watchKey(key);
                    transBegin();
                }

                selectDb(0);
                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "GET %s ", key.c_str());

                if (btrans) transEnd();

                iRet = collectLastError( pReply );

                if (0 == iRet && pReply->len > 0 && REDIS_REPLY_STRING == pReply->type) 
                {   
                    val.assign(pReply->str, pReply->len);
                    iRet = 0 ;
                }
                else
                {
                    LOG->error() << iRet << "|" <<__FUNCTION__ << "|key:" << key << "|val:" << val << endl ;
                }   

                if (pReply != NULL)
                {   
                    freeReplyObject(pReply);
                } 
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::delKeyValueCache(const std::string& key, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();
                if (btrans)
                {
                    watchKey(key);
                    transBegin();
                }

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "del %s ", key.c_str());

                if (btrans) transEnd();

                iRet = collectLastError( pReply );
                if ( iRet != 0) 
                {
                    LOG->error() << __FUNCTION__ << "|key:" << key <<"|error" << pReply->str << endl ;
                }   

                if (pReply != NULL)
                {   
                    freeReplyObject(pReply);
                } 
                
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::addKeyValueCache(const std::string& key, const int& val, const int& exprite, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                if (btrans)  
                {
                    watchKey(key);
                    transBegin();
                }

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "setex %s %d %d", key.c_str(), exprite, val);

                if (btrans) transEnd();

                iRet = collectLastError( pReply );
                if (iRet != 0)
                {
                    LOG->error() << iRet << "|" << __FUNCTION__ << "|key:" << key << "|val:" << val << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }
    
            }
            HUYA_CATCH
            {                
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::getKeyValueCache(const std::string& key, int& val, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                if (btrans)  
                {
                    watchKey(key);
                    transBegin();
                }

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "GET %s ", key.c_str());

                if (btrans) transEnd();

                iRet = collectLastError( pReply );

                if ( iRet == 0 && pReply->len > 0 && REDIS_REPLY_INTEGER == pReply->type) 
                {   
                    val = pReply->integer ;
                    iRet = 0 ;
                }
                else
                {
                    LOG->debug() << iRet << "|" << __FUNCTION__ << "|key:" << key << "|val:" << val << endl ;
                }   

                if (pReply != NULL)
                {   
                    freeReplyObject(pReply);
                } 
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::incrKeyValueCache(const std::string& key, const long& val, const int& exprite, long& result)
        {
            int  iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "INCRBY %s %ld", key.c_str(), val);

                iRet = collectLastError(pReply);
                if( iRet == 0 && pReply->type == REDIS_REPLY_INTEGER )
                {
                    result = pReply->integer;
                }
                else
                {
                     LOG->error() << iRet << "|" << __FUNCTION__ << "|INCRBY error" << "|key:" << key << "|val:" << val << endl ;   
                }

                if (exprite >0)
                {
                    pReply = (redisReply *)redisCommand(getRedisContext(), "EXPIRE %s %u", key.c_str(), exprite);
                    iRet = collectLastError( pReply );

                    if (iRet != 0)
                    {
                        LOG->error() << iRet << "|" << __FUNCTION__ << "|EXPIRE error" << "|key:" << key << "|exprite:" << exprite << endl ;  
                    }
                }

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::existsKey(const string& key)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "EXISTS %s", key.c_str());

                iRet = collectLastError(pReply);
                
                if (0 == iRet && pReply->type == REDIS_REPLY_INTEGER)
                {
                    iRet = pReply->integer;
                }
                else
                {
                    LOG->error() << iRet << "|" << __FUNCTION__ << "|EXISTS error" << "|key:" << key << endl ;  
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::pushValueToQueneCache(const std::string& key, const std::string& val, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "rpush %s %s", key.c_str(), val.c_str() );

                iRet = collectLastError( pReply );
                if (iRet != 0)
                {
                    LOG->error() << iRet << "|" << __FUNCTION__ << "|push list error" << "|key:" << key << "val:" << val << endl ;
                }

                if (pReply != NULL)
                {   
                    freeReplyObject(pReply);
                } 

                LOG->debug() << iRet << "|" << __FUNCTION__ << "|key:" << key << "val:" << val << endl ;
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::popValueFromQueneCache(const std::string& key, std::string& val, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = (redisReply *)redisCommand(getRedisContext(), "lpop %s", key.c_str());
                iRet  = collectLastError( pReply );
                if (iRet != 0)
                {
                    LOG->error() << __FUNCTION__ << " |rpop ERR redisCommand ret :=" <<  iRet << endl ;
                }
                else
                {
                    if ( pReply != NULL )
                    {
                        if (pReply->type == REDIS_REPLY_STRING && pReply->len > 0)
                        {
                            val.assign(pReply->str, pReply->len);
                        }
                    }
                }

                if (pReply != NULL)
                {   
                    freeReplyObject(pReply);
                } 

                LOG->debug() << iRet << __FUNCTION__ << "|key:" << key << "|val:"<< val << endl ;

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::addHashMapKey(const std::string& hashname, const std::string& key, const std::string& val, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "hset %s %s %s",hashname.c_str(), key.c_str(), val.c_str());
                iRet = collectLastError( pReply );
                
                if (iRet != 0)
                {
                    LOG->error() << __FUNCTION__ << "|error" << pReply->str << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::getHashMapKey(const std::string& hashname, const std::string& key, std::string& val, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "hget %s %s ", hashname.c_str(), key.c_str());
                iRet   = collectLastError( pReply );

                if (0 == iRet && 
                    pReply != NULL && 
                    pReply->type == REDIS_REPLY_STRING && pReply->len > 0)
                {
                    val.assign(pReply->str, pReply->len);
                }
                else
                {
                    if (iRet != 0)
                    {
                        LOG->error() << __FUNCTION__ << "|error" << pReply->str << endl ;   
                    }
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::delHashMapKey(const std::string& hashname, const std::string& key, const bool& btrans)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "hdel %s %s ", hashname.c_str(), key.c_str());
                iRet = collectLastError( pReply );

                if (iRet != 0)
                {
                    LOG->error() << __FUNCTION__ << "|hashname:" << hashname << "|key:" << key << "|error" << pReply->str << endl ;   
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::getHashMapValues(const std::string& hashname,  StringMap& values, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                pReply = (redisReply *)redisCommand(getRedisContext(), "hgetall %s ", hashname.c_str());
                iRet = collectLastError( pReply );
                if (0 == iRet && pReply->type == REDIS_REPLY_ARRAY || pReply->elements > 0)
                {
                    redisReply* rrField = NULL;
                    redisReply* rrValue = NULL;
                    for ( size_t i = 0; i+1 < pReply->elements ; i = i+2)
                    {
                        rrField = (pReply->element)[i];
                        rrValue = (pReply->element)[i+1];
                        
                        if (rrField->type == REDIS_REPLY_STRING && rrValue->type == REDIS_REPLY_STRING)
                        {
                            string value;
                            value.assign(rrValue->str, (size_t)rrValue->len);
                            values[rrField->str] = value;
                        }
                        else
                        {
                            if (pReply != NULL)
                            {
                                freeReplyObject(pReply);
                            }

                            LOG->error() << __FUNCTION__ << "| failed: rrField->type == REDIS_REPLY_STRING  && rrValue->type == REDIS_REPLY_STRING" << endl ;
                            return -2;
                        }
                    }
                }
                else
                {
                    if (pReply != NULL)
                    {
                        freeReplyObject(pReply);
                    }

                    LOG->error() << __FUNCTION__ << "| failed: " << "|iRet :" <<  iRet << "|type:" << pReply->type << endl ;
                    return -1 ;
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::setAdd(const string& key, const string& value, int64_t score, bool isBinary)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                std::string cmd = "zadd " + key + " " + TC_Common::tostr(score) + " ";
                if (isBinary)
                {
                    cmd = cmd + "%b";
                   pReply = (redisReply*)redisCommand(getRedisContext(), cmd.c_str(), value.c_str(), (size_t)value.size());
                }
                else
                {
                    cmd = cmd + value;
                    pReply = (redisReply*)redisCommand(getRedisContext(), cmd.c_str());
                }

                iRet = collectLastError( pReply );
                if (iRet != 0)
                {
                    LOG->error() << __FUNCTION__ << "| failed: " << "|iRet :" <<  iRet << "|key:" << key << "|score:" << score << "|type:" << pReply->type << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }
        
        int     RedisBaseClient::setDel(const string& key, const string& member)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                checkConnect();
                
                redisReply *pReply = NULL;
                std::string cmd = "srem " + key + " " + member;
                pReply = (redisReply*)redisCommand(getRedisContext(), cmd.c_str());

                iRet = collectLastError( pReply );
                if (iRet != 0)
                {
                    LOG->error() << __FUNCTION__ << "| failed: " << "|iRet :" <<  iRet << "|key:" << key << "|type:" << pReply->type << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }
        
        int     RedisBaseClient::setDel(const string& key, int64_t score)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                checkConnect();
                
                redisReply *pReply = NULL;
                std::string cmd = "zremrangebyscore  " + key + " " + TC_Common::tostr(score) + " " + TC_Common::tostr(score);
                pReply = (redisReply*)redisCommand(getRedisContext(), cmd.c_str());

                iRet = collectLastError( pReply );
                if (iRet != 0)
                {
                    LOG->error() << __FUNCTION__ << "| failed: " << "|iRet :" <<  iRet << "|key:" << key << "|score:" << score << "|type:" << pReply->type << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;
        }

        int     RedisBaseClient::setRevrangebyscore(const string& key, int64_t maxscore, int64_t minscore, int count, list<string>& result)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                checkConnect();

                redisReply *pReply = NULL;
                std::string cmd = "zrevrangebyscore " + key + " " + TC_Common::tostr(maxscore) + " " + TC_Common::tostr(minscore) + " limit 0 " + TC_Common::tostr(abs(count));
                pReply = (redisReply*)redisCommand(getRedisContext(), cmd.c_str());

                iRet = collectLastError( pReply );
                if (0 == iRet)
                {
                    for ( size_t i = 0; i < pReply->elements; i++)
                    {
                        redisReply* tempr = (pReply->element)[i];
                        if (tempr->type == REDIS_REPLY_STRING )
                        {
                            string member;
                            member.assign(tempr->str, (size_t)tempr->len);
                            result.push_back(member);
                        }
                    }
                }
                else
                {
                    LOG->error() << __FUNCTION__ << "| failed: " << "|iRet :" <<  iRet << "|key:" << key << "|maxscore:" << maxscore << "|minscore:" << minscore << "|type:" << pReply->type << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;

        }

        int     RedisBaseClient::setRangebyscore(const string& key, int64_t minscore, int64_t maxscore, int count, list<string>& result)
        {
            int iRet = -1 ;
            HUYA_TRY
            {
                checkConnect();
                
                redisReply *pReply = NULL;
                std::string cmd = "zrangebyscore " + key + " " + TC_Common::tostr(minscore) + " " + TC_Common::tostr(maxscore) + " limit 0 " + TC_Common::tostr(abs(count));
                pReply = (redisReply*)redisCommand(getRedisContext(), cmd.c_str());
                iRet = collectLastError(pReply);
                if (0 == iRet)
                {
                    for ( size_t i = 0; i < pReply->elements; i++)
                    {
                        redisReply* tempr = (pReply->element)[i];                        
                        if (tempr->type == REDIS_REPLY_STRING )
                        {
                            string member;
                            member.assign(tempr->str, (size_t)tempr->len);
                            result.push_back(member);
                        }
                    }
                }
                else
                {
                    LOG->error() << __FUNCTION__ << "| failed: " << "|iRet :" <<  iRet << "|key:" << key << "|maxscore:" << maxscore << "|minscore:" << minscore << "|type:" << pReply->type << endl ;
                }

                if (pReply != NULL)
                {
                    freeReplyObject(pReply);
                }

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("RedisBaseClient");
            }

            return iRet ;

        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        ClusterGroup::ClusterGroup(const RedisCluster& node)
        :_tClusterConfig(node),_iMasterIndex(-1),_iMasterCount(0)
        {

        }

        ClusterGroup::~ClusterGroup()
        {

        }

        void ClusterGroup::init()
        {
            HUYA_TRY
            {
                _pMasterCluster = new RedisBaseClient(_tClusterConfig.ip, _tClusterConfig.port, _tClusterConfig.dbindex, 
                            _tClusterConfig.timeout, _tClusterConfig.auth) ;

                _pMasterCluster->init();
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterGroup");
            }

            HUYA_TRY
            {
                _pSlaveCluster = new RedisBaseClient(_tClusterConfig.slaveIp, _tClusterConfig.slavePort, _tClusterConfig.dbindex, 
                                _tClusterConfig.timeout, _tClusterConfig.auth) ;
                _pSlaveCluster->init();
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterGroup");
            }

            HUYA_TRY
            {
                _iMasterIndex = 0 ;
                _tClusterArray.push_back(new RedisBaseClient(_tClusterConfig.ip, _tClusterConfig.port, _tClusterConfig.dbindex, 
                            _tClusterConfig.timeout, _tClusterConfig.auth)) ;

                if (_tClusterArray.size() > 0)
                    _tClusterArray[0]->init();
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterGroup");
            }

            HUYA_TRY
            {
                _tClusterArray.push_back(new RedisBaseClient(_tClusterConfig.slaveIp, _tClusterConfig.slavePort, _tClusterConfig.dbindex, 
                                _tClusterConfig.timeout, _tClusterConfig.auth));
                if (_tClusterArray.size() > 1)
                    _tClusterArray[1]->init();
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterGroup");
            }
        }

        void ClusterGroup::destroy()
        {

        }

        void ClusterGroup::doCheck()
        {
            int iRet = 0 ;
            HUYA_TRY
            {
                if (_pMasterCluster.get() != NULL && 0 == _iMasterIndex)
                {
                    iRet = _pMasterCluster->doPing();
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterGroup");
            }

            if (iRet != 0)
            {
                _iMasterCount++ ;
            }
            else
            {
                _iMasterCount = 0 ;
            }

            HUYA_TRY
            {
                if (_tClusterArray.size() > 1 && _iMasterCount > 5)
                {
                    iRet = _pSlaveCluster ->doPing();

                    if (0 == iRet)
                    {
                        _pSlaveCluster->doMaster();
                        _iMasterIndex = 1 ;
                        _iMasterCount = 0 ;
                    }
                }

            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterGroup");
            }

            LOG->debug() << "|iRet:" << iRet << "|" << __FUNCTION__  << "|_iMasterIndex:" << _iMasterIndex <<  "|_iMasterCount:" << _iMasterCount << endl ;
        }

        RedisBaseClientPtr& ClusterGroup::getMaster()
        {
            if (_tClusterArray.size()  > _iMasterIndex)
            {
                return _tClusterArray[_iMasterIndex];
            }
        }

        bool   ClusterGroup::isInGroup(int hashcode)
        {
            return  _tClusterConfig.iSlotFrom <= hashcode && hashcode <= _tClusterConfig.iSlotTo ;
        }

        

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        ClusterProxy::ClusterProxy(const RedisClusterList&  rediscluster)
        :TaskSchedule(3000),_vRedisCluster(rediscluster)
        {

        }

        ClusterProxy::~ClusterProxy()
        {

        }

        void  ClusterProxy::init()
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                for (RedisClusterList::const_iterator iter = _vRedisCluster.begin(); 
                     iter !=  _vRedisCluster.end();  ++iter)
                {
                    _vClusterGroup.push_back(new ClusterGroup((*iter)));
                }

                for (ClusterGroupPtrArray::const_iterator iter = _vClusterGroup.begin(); 
                    iter != _vClusterGroup.end();
                     ++iter)
                {
                    (*iter)->init();
                }

                TaskSchedule::init();
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }
        }

        void ClusterProxy::close()
        {

        }

        RedisBaseClientPtr&      ClusterProxy::getRedisCluster(int hashcode)
        {
            for (ClusterGroupPtrArray::const_iterator iter = _vClusterGroup.begin(); iter != _vClusterGroup.end(); ++iter)
            {
                if ((*iter)->isInGroup(hashcode % MAX_SLOT))
                {
                    return (*iter)->getMaster();
                }
            }

            if (hashcode < 0 && _vClusterGroup.size() > 0)
            {
                return _vClusterGroup[0]->getMaster();
            }

            return _vClusterGroup[hashcode % _vClusterGroup.size()]->getMaster();
        }

        uint32_t ClusterProxy::eLFHash(const char *str)
        {
            unsigned int hash = 0;
            unsigned int x    = 0;
 
            while (*str)
            {
                hash = (hash << 4) + (*str++);
                if ((x = hash & 0xF0000000L) != 0)
                {
                    hash ^= (x >> 24);
                    hash &= ~x;
                }
            }
 
            return (hash & 0x7FFFFFFF);
        }

        int  ClusterProxy::addKeyValueCache(const std::string& key, const std::string& val, const int& exprite, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->addKeyValueCache(key, val, exprite, btrans);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int  ClusterProxy::getKeyValueCache(const std::string& key, std::string& val, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->getKeyValueCache(key, val, btrans);                    
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::delKeyValueCache(const std::string& key, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->delKeyValueCache(key, btrans);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::addKeyValueCache(const std::string& key, const int& val, const int& exprite, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->addKeyValueCache(key, val, exprite, btrans);
                }
            }
            HUYA_CATCH
            {                
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::getKeyValueCache(const std::string& key, int& val, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->getKeyValueCache(key, val, btrans);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::pushValueToQueneCache(const std::string& key, const std::string& val, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));

                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->pushValueToQueneCache(key, val, btrans);
                }

                LOG->debug() << iRet << "|" << __FUNCTION__ << "|key:" << key << "val:" << val << endl ;
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::popValueFromQueneCache(const std::string& key, std::string& val, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->popValueFromQueneCache(key, val, btrans);
                }

                LOG->debug() << iRet << __FUNCTION__ << "|key:" << key << "|val:"<< val << endl ;
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::addHashMapKey(const std::string& hashname, const std::string& key, const std::string& val, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(hashname.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->addHashMapKey(hashname, key, val, btrans);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::getHashMapKey(const std::string& hashname, const std::string& key, std::string& val, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(hashname.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->getHashMapKey(hashname, key, val, btrans);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::delHashMapKey(const std::string& hashname, const std::string& key, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(hashname.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->delHashMapKey(hashname, key, btrans);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::getHashMapValues(const std::string& hashname,  StringMap& values, const bool& btrans)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(hashname.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->getHashMapValues(hashname, values, btrans);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::setAdd(const string& key, const string& value, int64_t score, bool isBinary)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->setAdd(key, value, score, isBinary);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }
        
        int     ClusterProxy::setDel(const string& key, const string& member)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                   iRet = pRedisClient->setDel(key, member);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }
        
        int     ClusterProxy::setDel(const string& key, int64_t score)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->setDel(key, score);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;
        }

        int     ClusterProxy::setRevrangebyscore(const string& key, int64_t maxscore, int64_t minscore, int count, list<string>& result)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->setRevrangebyscore(key, maxscore, minscore, count, result);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;

        }

        int     ClusterProxy::setRangebyscore(const string& key, int64_t minscore, int64_t maxscore, int count, list<string>& result)
        {
            int iRet = -1 ;

            HUYA_TRY
            {
                RedisBaseClientPtr&  pRedisClient =  getRedisCluster(eLFHash(key.c_str()));
                if (pRedisClient->getRedisContext() != NULL)
                {
                    iRet = pRedisClient->setRangebyscore(key, minscore, maxscore, count, result);
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return iRet ;

        }

         bool ClusterProxy::process()
         {
            bool bRet = true ;

            HUYA_TRY
            {
                TC_ThreadLock::Lock sync(_tDataLock);

                ClusterGroupPtrArray::const_iterator iter = _vClusterGroup.begin();
                for (; iter != _vClusterGroup.end(); ++iter)
                {
                    (*iter)->doCheck();
                }
            }
            HUYA_CATCH
            {
                LOG_EXCEPTION("ClusterProxy");
            }

            return bRet ;
         }
};
