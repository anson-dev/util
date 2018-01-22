﻿#ifndef _COMMHASHMAP_H_
#define _COMMHASHMAP_H_

#include "log/taf_logger.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_singleton.h"
#include "TypeDefine.h"
/**
 *
 *
 */

template <unsigned long long iMaxMemSize = 1073741824>
class TCommHashMap : public TC_Singleton<TCommHashMap<iMaxMemSize> >
{
public:

    TCommHashMap()
    {
    }

    template <typename M,typename K, typename V> 
    int addHashMap(M &tJceMap,K &tKey,V &tValue)
    {
        int iRet = -1;

        HUYA_TRY
        {
            float rate =  (tJceMap.getMapHead()._iUsedChunk) * 1.0/tJceMap.allBlockChunkCount();

            if (rate >0.9  && (tJceMap.getMapHead()._iMemSize*2 < iMaxMemSize) )
            {
                tJceMap.expand(tJceMap.getMapHead()._iMemSize * 2);
                LOG->debug() <<__FILE__<<"::"<< __FUNCTION__ <<" hashmap expand to "<<tJceMap.getMapHead()._iMemSize<<endl;
            }

            iRet = tJceMap.set( tKey, tValue );

            if ( iRet !=  TC_HashMap::RT_OK )
            {
                LOG->debug() <<__FILE__<<"::"<< __FUNCTION__<<" set hashmap recourd erro|" << iRet<< endl;
            }
            return iRet;
        }
        HUYA_CATCH
        {
             LOG_EXCEPTION("TCommHashMap");
        }
        return iRet ;
    }

    template <typename M,typename K, typename V> 
    int getHashMap(M &tJceMap,K &tKey,V &tValue)
    {
        int iRet = -1;

        HUYA_TRY
        {
            iRet = tJceMap.get( tKey, tValue );
            if ( iRet !=  TC_HashMap::RT_OK )
            {
               // LOG->debug() <<__FILE__<<"::"<< __FUNCTION__<<" get hashmap recourd erro|" << iRet<< endl;
            } 
            else
            {
                iRet = 0;
            }
            return iRet;
        }
        HUYA_CATCH
        {
            LOG_EXCEPTION("TCommHashMap");
        }

        return iRet ;
    }

    template <typename M>
    void dump2file(M &tJceMap,const string &sFilePre,int iDumpInterv = 3600*24)
    {
        HUYA_TRY
        {
            time_t tTimeNow         = TC_TimeProvider::getInstance()->getNow();
            static time_t g_tLastDumpTime   = TC_TimeProvider::getInstance()->getNow();

            if (tTimeNow - g_tLastDumpTime > iDumpInterv)
            {

                static  TC_ThreadLock g_mutex;
                TC_ThreadLock::Lock  lock( g_mutex );
                if (tTimeNow - g_tLastDumpTime > iDumpInterv)
                {

                    string sFile = sFilePre +"_" +TC_Common::tm2str(tTimeNow,"%Y%m%d%H%M") + ".txt";

                    int iRet = tJceMap.dump2file(sFile);
                    if (iRet != 0)
                    {
                        TC_File::removeFile(sFile,false);
                        LOG->error()<<__FILE__<<"::"<< __FUNCTION__<<"|"<< sFile <<"|ret|"<<iRet<< endl;
                    }

                    LOG->debug()<<__FILE__<<"::"<< __FUNCTION__<<"|" << sFile << " TimeInterv:" << iDumpInterv << " now:" << tTimeNow << " last:" << g_tLastDumpTime << endl;
                    g_tLastDumpTime = tTimeNow;
                }

            }
        }
        HUYA_CATCH
        {
            LOG_EXCEPTION("TCommHashMap");
        }
    }

    template <typename M,typename K>
    int eraseHashMap(M &tJceMap,K &tKey)
    {
        int iRet = -1;

        HUYA_TRY
        {
            iRet = tJceMap.erase(tKey);

            if (iRet == TC_HashMap::RT_OK || iRet == TC_HashMap::RT_ONLY_KEY || TC_HashMap::RT_NO_DATA)
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
        HUYA_CATCH
        {
            LOG_EXCEPTION("TCommHashMap");
        }
        return iRet ;
    }

};

typedef TCommHashMap<> CommHashMap;

/////////////////////////////////////////////////////
#endif
