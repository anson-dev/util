#ifndef __LOCKEDSINGLETON_H__
#define __LOCKEDSINGLETON_H__

#include "jce/JceType.h"
#include "log/taf_logger.h"
#include "util/tc_timeprovider.h"
#include "util/tc_singleton.h"

namespace SingletonHelper
{
    template <typename Derived, typename Base>
    struct add_base
    {
        typedef class : public Derived, public Base {} DerivedWithBase;
        typedef typename std::conditional<std::is_base_of<Base, Derived>::value, Derived, DerivedWithBase>::type type;
    };
    
    template <typename Derived, typename Base>
    struct add_base_direct
    {
        typedef class : public Derived, public Base {} type;
    };
    
    template <typename T, typename L>
    class LockedGuard
    {
    public:
        LockedGuard( T *t, L *l )
            : t( t ), l( l )
        {
#ifndef LOCKEDGUARD_DISABLE_LONGLOCK_WARNING
            lLockedStart = taf::TC_TimeProvider::getInstance()->getNowMs();
#endif
#ifdef LOCKEDGUARD_DEBUG
            cout << "lock" << endl;
#endif
            l->lock();
        }
        ~LockedGuard()
        {
            if( l )
            {
#ifdef LOCKEDGUARD_DEBUG
                cout << "unlock" << endl;
#endif
#ifndef LOCKEDGUARD_DISABLE_LONGLOCK_WARNING
                //500ms 以上的锁时间告警
                taf::Int64 timeCost = taf::TC_TimeProvider::getInstance()->getNowMs() - lLockedStart;
                
                if( timeCost > 500 )
                {
                    LOG->error() << "[tafutil] LockedGuard low performance warning cost:" << timeCost << endl;
                }
                
#endif
                l->unlock();
            }
        }
        T *operator->() const
        {
            return t;
        }
        T &operator*() const
        {
            return *t;
        }
        LockedGuard( const LockedGuard &rhs );
        void operator=( const LockedGuard &rhs );
    private:
        T *t;
        L *l;
        taf::Int64 lLockedStart;
    };
    
    template <typename T,
              template <class> class CreatePolicy = taf::CreateUsingNew,
              template <class> class LifetimePolicy = taf::DefaultLifetime>
    class LockedSingleton : public taf::TC_ThreadLock, protected taf::TC_Singleton<T, CreatePolicy, LifetimePolicy>
    {
    public:
        static LockedGuard<T, taf::TC_ThreadLock> getLockedInstance()
        {
            static taf::TC_ThreadLock lock;
            auto instance = taf::TC_Singleton<T, CreatePolicy, LifetimePolicy>::getInstance();
            return LockedGuard<T, taf::TC_ThreadLock>( instance, &lock );
        }
    protected:
        static LockedGuard<T, T> getInstance()
        {
            return getLockedInstance();
        }
    };
    
    template <typename T>
    class LockedValue
    {
    private:
        mutable taf::TC_ThreadLock lock;
        T t;
        
    public:
        template <typename... Args>
        LockedValue( Args... param )
            : t( param... )
        {
        }
        LockedGuard<const T, taf::TC_ThreadLock> operator->() const
        {
            return LockedGuard<const T, taf::TC_ThreadLock>( &t, &lock );
        }
        LockedGuard<T, taf::TC_ThreadLock> operator->()
        {
            return LockedGuard<T, taf::TC_ThreadLock>( &t, &lock );
        }
        LockedGuard<T, taf::TC_ThreadLock> operator*()
        {
            return LockedGuard<T, taf::TC_ThreadLock>( &t, &lock );
        }
        LockedGuard<const T, taf::TC_ThreadLock> getLocked() const
        {
            return LockedGuard<const T, taf::TC_ThreadLock>( &t, &lock );
        }
        LockedGuard<T, taf::TC_ThreadLock> getLocked()
        {
            return LockedGuard<T, taf::TC_ThreadLock>( &t, &lock );
        }
        template<typename Type>
        void setValue( Type &&t )
        {
            LockedGuard<T, taf::TC_ThreadLock> l( &this->t, &lock );
            this->t = std::forward<Type>( t );
        }
        template<typename Type>
        LockedValue &operator=( Type &&t )
        {
            LockedGuard<T, taf::TC_ThreadLock> l( &this->t, &lock );
            this->t = std::forward<Type>( t );
            return *this;
        }
        T getValue()
        {
            LockedGuard<T, taf::TC_ThreadLock> l( &t, &lock );
            return t;
        }
    };
}

using namespace SingletonHelper;

#endif
