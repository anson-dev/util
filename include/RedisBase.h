#ifndef __ORDERRECORDREDIS_H_
#define __ORDERRECORDREDIS_H_

#include <stdint.h>
#include "hiredis.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include "util/tc_autoptr.h"
#include "TaskSchedule.h"


namespace HuyaUtil {
    
    struct  RedisCluster
    {
        std::string   ip;
        uint32_t      port ;
        uint32_t      dbindex;
        uint32_t      timeout ;
        std::string   auth ;

        string        slaveIp ;
        uint32_t      slavePort ;

        short         iSlotFrom ;  
        short         iSlotTo ;
    };

    typedef  std::list<RedisCluster>                RedisClusterList ;
    typedef  std::map<std::string, std::string>     StringMap;

    const short  MAX_SLOT = 1024 ;

    class  ClusterProxy ;
    class   RedisBaseClient : public taf::TC_HandleBase
    {
        public:
            RedisBaseClient(const std::string& ip, const uint32_t& port , const uint32_t& dbindex, const uint32_t& timeout, const std::string& auth);
            ~RedisBaseClient();
        public:
            int  init();
            void close();
            void preHandConnect();
            int  checkAlive();
            int  doPing();
            int  selectDb(const uint32_t& dbindex);
            int  doMaster();
            redisContext* getRedisContext();
            bool   setRedisProxy(ClusterProxy*  proxy);
            int    collectLastError(redisReply *reply);

            // trans
            bool   transBegin();
            bool   transEnd(); 
            bool   watchKey(const std::string& key);
            bool   transDiscard();

            // oper
            int     addKeyValueCache(const std::string& key, const std::string& val, const int& exprite,  const bool& btrans = false);
            int     getKeyValueCache(const std::string& key, std::string& val, const bool& btrans = false); 
            int     delKeyValueCache(const std::string& key, const bool& btrans = false);
            int     incrKeyValueCache(const std::string& key, const long& val, const int& exprite, long& result);
            int     existsKey(const string& key);

            int     addKeyValueCache(const std::string& key, const int& val, const int& exprite, const bool& btrans = false);
            int     getKeyValueCache(const std::string& key, int& val, const bool& btrans = false);   

            int     pushValueToQueneCache(const std::string& key, const std::string& val, const bool& btrans = false);
            int     popValueFromQueneCache(const std::string& key, std::string& val, const bool& btrans = false);

            int     addHashMapKey(const std::string& hashname, const std::string& key, const std::string& val, const bool& btrans = false);
            int     getHashMapKey(const std::string& hashname, const std::string& key, std::string& val, const bool& btrans = false);
            int     delHashMapKey(const std::string& hashname, const std::string& key, const bool& btrans = false);
            int     getHashMapValues(const std::string& hashname,  StringMap& values, const bool& btrans = false);

            int     setAdd(const string& key, const string& value, int64_t score, bool isBinary);
            int     setDel(const string& key, const string& member);
            int     setDel(const string& key, int64_t score);
            int     setRevrangebyscore(const string& key, int64_t maxscore, int64_t minscore, int count, list<string>& result);
            int     setRangebyscore(const string& key, int64_t minscore, int64_t maxscore, int count, list<string>& result);
        protected:
            uint32_t  toUint32(std::string input);
            int       checkConnect();
        protected:
            std::string         _sIp;
            uint32_t            _iPort ;
            uint32_t            _iDbIndex ;
            uint32_t            _iTimeout ;
            redisContext*       _pClient;
            bool                _bConnectFlag;
            int64_t             _lCheckTime;
            std::string         _sName;
            std::string         _sAuth;
            ClusterProxy*         _pClusterProxy ;
    };

    typedef  taf::TC_AutoPtr<RedisBaseClient>   RedisBaseClientPtr ;
    typedef  std::vector<RedisBaseClientPtr>    RedisBaseClientPtrArray ;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    class ClusterGroup : public taf::TC_HandleBase
     {
     public:
        ClusterGroup(const RedisCluster& node);
        ~ClusterGroup();

        void init();
        void destroy();

        void doCheck();

        RedisBaseClientPtr& getMaster(); 

        bool   isInGroup(int hashcode); 
    private:
        RedisCluster        _tClusterConfig;
        RedisBaseClientPtrArray  _tClusterArray ; 
        RedisBaseClientPtr  _pMasterCluster ;
        RedisBaseClientPtr  _pSlaveCluster ;
        TC_ThreadLock       _tDataLock ;
        int                 _iMasterIndex ;
        int                 _iMasterCount ;
     }; 


     typedef taf::TC_AutoPtr<ClusterGroup>  ClusterGroupPtr ;
     typedef std::vector<ClusterGroupPtr>      ClusterGroupPtrArray;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    class ClusterProxy :  public TaskSchedule
    {
    public:
        ClusterProxy(const RedisClusterList&  rediscluster);
        ~ClusterProxy();

        virtual void init();
        void close();

    private:
        virtual bool process();

    public:
        int     addKeyValueCache(const std::string& key, const std::string& val, const int& exprite,  const bool& btrans = false);
        int     getKeyValueCache(const std::string& key, std::string& val, const bool& btrans = false); 
        int     delKeyValueCache(const std::string& key, const bool& btrans = false);

        int     addKeyValueCache(const std::string& key, const int& val, const int& exprite, const bool& btrans = false);
        int     getKeyValueCache(const std::string& key, int& val, const bool& btrans = false);   

        int     pushValueToQueneCache(const std::string& key, const std::string& val, const bool& btrans = false);
        int     popValueFromQueneCache(const std::string& key, std::string& val, const bool& btrans = false);

        int     addHashMapKey(const std::string& hashname, const std::string& key, const std::string& val, const bool& btrans = false);
        int     getHashMapKey(const std::string& hashname, const std::string& key, std::string& val, const bool& btrans = false);
        int     delHashMapKey(const std::string& hashname, const std::string& key, const bool& btrans = false);
        int     getHashMapValues(const std::string& hashname,  StringMap& values, const bool& btrans = false);

        int     setAdd(const string& key, const string& value, int64_t score, bool isBinary);
        int     setDel(const string& key, const string& member);
        int     setDel(const string& key, int64_t score);
        int     setRevrangebyscore(const string& key, int64_t maxscore, int64_t minscore, int count, list<string>& result);
        int     setRangebyscore(const string& key, int64_t minscore, int64_t maxscore, int count, list<string>& result);

    protected:
        RedisBaseClientPtr&      getRedisCluster(int hashcode);
        uint32_t                 eLFHash(const char *str) ;

    private:
        RedisClusterList          _vRedisCluster ;
        ClusterGroupPtrArray      _vClusterGroup ;
        TC_ThreadLock             _tDataLock ;
    };

     typedef  taf::TC_AutoPtr<ClusterProxy>   ClusterProxyPtr ;
};
#endif //__ORDERRECORDREDIS_H_
