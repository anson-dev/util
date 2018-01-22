#include "TaskSchedule.h"
#include "TypeDefine.h"
#include "log/taf_logger.h"

namespace HuyaUtil
{
	TaskSchedule::TaskSchedule(const int& interval)
	:_iInterval(interval),_bTerminate(true)
	{

	}

	TaskSchedule::~TaskSchedule()
	{

	}

	void TaskSchedule::init()
	{
		HUYA_TRY
		{
			_bTerminate = false;
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("TaskSchedule");
		}
	}

	void TaskSchedule::terminate() 
	{
		HUYA_TRY
		{
			_bTerminate = true;
    	
      		TC_ThreadLock::Lock sync(*this);
      		notifyAll();
    	
		}
		HUYA_CATCH
		{
			LOG_EXCEPTION("TaskSchedule");
		}
	}

	void TaskSchedule::run() 
	{
		while (!_bTerminate)
    	{
      		process();

      		{
        		TC_ThreadLock::Lock sync(*this);
        		timedWait(_iInterval);
      		}
    	}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	TaskScheduleEx::TaskScheduleEx()
    : bTerminate( false ), updateFreq( 60000 )
    {
    }
	
	TaskScheduleEx::~TaskScheduleEx()
    {
        terminate();
    }

    void TaskScheduleEx::terminate()
    {
        bTerminate = true;
        
        TC_ThreadLock::Lock tempLock( lock );
        lock.notifyAll();
    }

    void TaskScheduleEx::start( int updateFreq )
    {
    	init();

    	LOG->debug() << __FUNCTION__ << "|[TaskScheduleEx]Start." << endl;
        this->updateFreq = updateFreq;
        
        try
        {
            bTerminate = !onScheduled();
        }
        catch( std::exception &ex )
        {
            LOG->error() << __FUNCTION__ << "|[TaskScheduleEx]Ex:" << ex.what() << endl;
        }
        
        taf::TC_Thread::start();
    }

    void TaskScheduleEx::notify()
    {
        taf::TC_ThreadLock::Lock tempLock( lock );
        lock.notifyAll();
    }

    void TaskScheduleEx::run()
    {
       	if (!bTerminate )
        {
            taf::TC_ThreadLock::Lock tempLock( lock );
            lock.timedWait( updateFreq );
        }
        
        while( !bTerminate )
        {
            try
            {
                bTerminate = !onScheduled();
            }
            catch( std::exception &ex )
            {
                LOG->error() << __FUNCTION__ << "|[TaskSchedule]Ex:" << ex.what() << endl;
            }
            
            taf::TC_ThreadLock::Lock tempLock( lock );
            lock.timedWait( updateFreq );
        }
        
        LOG->debug() << __FUNCTION__ << "|[TaskSchedule]Exit." << endl;
    }

};
