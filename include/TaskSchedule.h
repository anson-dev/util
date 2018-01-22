#ifndef  TASK_SCHEDULE_H_
#define  TASK_SCHEDULE_H_

#include "util/tc_autoptr.h"
#include "util/tc_thread.h"
#include "util/tc_thread_rwlock.h"


namespace HuyaUtil
{
	using namespace taf ;
	class TaskSchedule : public TC_Thread, public TC_ThreadLock, public taf::TC_HandleBase
	{
	public:
		TaskSchedule(const int& interval);
		~TaskSchedule();

		virtual void init();
		virtual void terminate() ;
		virtual bool process() = 0 ;
	protected:
		virtual void run() ;
	private:
		int32_t		    	_iInterval ;		
		bool 				_bTerminate ;
		
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	class TaskScheduleEx: protected taf::TC_Thread
	{
	public:
		TaskScheduleEx() ;
    	virtual ~TaskScheduleEx() ;
    	void terminate() ;
    	void start( int updateFreq ) ;
    	void notify() ;

	protected:
    	virtual void init() {}
    	virtual bool onScheduled() = 0;
 		void run() ;

	protected:
    	bool bTerminate;
    	int updateFreq;
    	taf::TC_ThreadLock lock;
	};
};

#endif  //#endif TASK_SCHEDULE_H_
