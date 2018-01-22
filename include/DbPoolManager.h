#ifndef  DBPOOLMANAGER_H_
#define  DBPOOLMANAGER_H_

#include "util/tc_mysql.h"
#include "util/tc_autoptr.h"
#include "util/tc_thread.h"
#include <string>
#include <list>

namespace HuyaUtil
{
	#define DBINT( value ) make_pair( TC_Mysql::DB_INT, TC_Common::tostr( value ) )
	#define DBSTR( value ) make_pair( TC_Mysql::DB_STR, ( value ) )

	typedef 	std::list<taf::TC_Mysql*>	MySqlConnectionList ;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// db Manger
	class DbPoolManager : public taf::TC_HandleBase, public taf::TC_ThreadLock
	{
	public:
		DbPoolManager(const taf::TC_DBConf& config, const int& size);
		~DbPoolManager();

	public:
		void init();
		void destroy();

		taf::TC_Mysql*  getMysqlConnection();
		void            pushMysqlConnection(taf::TC_Mysql* connect);

		void begin(taf::TC_Mysql* connect);
		void rollback(taf::TC_Mysql* connect);
		void commit(taf::TC_Mysql* connect);

	private:
		taf::TC_DBConf          _tDbConfig ;
		int       				_iPoolSize ;
		MySqlConnectionList		_vConnectionList ;
	};

	typedef taf::TC_AutoPtr<DbPoolManager>   DbPoolManagerPtr ;
};

#endif
