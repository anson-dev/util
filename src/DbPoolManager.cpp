#include "DbPoolManager.h"
#include "TypeDefine.h"
#include "log/taf_logger.h"
#include  <util/tc_timeprovider.h>

using namespace taf ;

namespace HuyaUtil
{
	DbPoolManager::DbPoolManager(const taf::TC_DBConf& config, const int& size)
	:_tDbConfig(config),_iPoolSize(size)
	{

	}

	DbPoolManager::~DbPoolManager()
	{
		destroy() ;
	}

	void DbPoolManager::init()
	{
		HUYA_TRY
		{
			// create pool
			TC_ThreadLock::Lock sync(*this);
			taf::TC_Mysql*  pMysql = NULL ;
			for (int i = 0; i < _iPoolSize; ++i)
			{
				pMysql = new taf::TC_Mysql();
            	pMysql->init( _tDbConfig._host, _tDbConfig._user, _tDbConfig._password, _tDbConfig._database, _tDbConfig._charset, _tDbConfig._port, 0 );
            	pMysql->connect();

            	_vConnectionList.push_back(pMysql);
			}

		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("DbPoolManager");
		}
	}

	void DbPoolManager::destroy()
	{
		HUYA_TRY
		{
			MySqlConnectionList::iterator iter =  _vConnectionList.begin();
			for (; iter != _vConnectionList.end(); ++iter)
			{
				/* code */
				(*iter)->disconnect();
				delete (*iter) ;
			}
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("DbPoolManager");
		}
	}

	taf::TC_Mysql*  DbPoolManager::getMysqlConnection()
	{
		HUYA_TRY
		{
			TC_ThreadLock::Lock sync(*this);

			taf::TC_Mysql*  pMysql = NULL ;
			if (_vConnectionList.empty())
			{
				pMysql = new taf::TC_Mysql();
				pMysql->init( _tDbConfig._host, _tDbConfig._user, _tDbConfig._password, _tDbConfig._database, _tDbConfig._charset, _tDbConfig._port, 0 );
            	pMysql->connect();

            	LOG->debug() << __FUNCTION__ << "create a new mysql connection !" << endl ;
				return pMysql ;
			}


			pMysql = _vConnectionList.front();
			_vConnectionList.pop_front();

			LOG->debug() << __FUNCTION__ << "_vConnectionList size:" << _vConnectionList.size() << endl ;
			return pMysql ;
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("DbPoolManager");
		}

		return NULL ;
	}

	void    DbPoolManager::pushMysqlConnection(taf::TC_Mysql* connect)
	{
		HUYA_TRY
		{
			TC_ThreadLock::Lock sync(*this);
			_vConnectionList.push_back(connect);

			LOG->debug() << __FUNCTION__ << "_vConnectionList size:" << _vConnectionList.size() << endl ;
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("DbPoolManager");
		}
	}

	void DbPoolManager::rollback(taf::TC_Mysql* connect)
	{
		HUYA_TRY
		{
			connect->execute("rollback");
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("DbPoolManager");
		}
	}

	void DbPoolManager::begin(taf::TC_Mysql* connect)
	{
		HUYA_TRY
		{
			//设置事务超时时长
			connect->execute("set session innodb_lock_wait_timeout=1");
			connect->execute("begin");
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("DbPoolManager");
		}
	}

	void DbPoolManager::commit(taf::TC_Mysql* connect)
	{
		HUYA_TRY
		{
			connect->execute("commit");
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("DbPoolManager");
		}
		
	}
};
