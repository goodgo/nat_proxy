#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H


#include "Mutex.h"

template<typename T>
class static_constructable
{
private:
	struct helper
    {
        helper()
        {
		    T::static_constructor();
        }
    };

protected:
    static_constructable()
    {
        static helper placeholder;
    }
};

template<typename T,template<class> class TWrapper>
class static_constructableT1
{
private:
   struct helper
    {
      helper()
        {
        	TWrapper<T>::static_constructor();
        }
    };

protected:
    static_constructableT1()
    {
        static helper placeholder;
    }
};


template<typename T>
class TTSWrapper:public static_constructableT1<T,TTSWrapper>
{
public:
	static void static_constructor()
	{
		pthread_key_create( &thread_key, NULL );
	}

	static pthread_key_t thread_key;

public:
	TTSWrapper()
	{}

	~TTSWrapper()
	{}

	static T* GetRawPtr()
	{
		T* ts_obj = (T *)pthread_getspecific( thread_key );
		if ( ts_obj == NULL )
		{
			ts_obj = new T;
			pthread_setspecific( thread_key, ts_obj );
		}
		return ts_obj;
	}

	static void DestoryRawPtr()
	{
		T * pT = (T*)pthread_getspecific(thread_key);
		pthread_key_delete(thread_key);
		if ( pT ){
			delete pT;
		}
	}
};

#define DECLARE_TTS_OBJECT(T) template<> pthread_key_t TTSWrapper<T>::thread_key = 0;

typedef void *(*pt_start_rtn)(void *);
typedef pt_start_rtn LPTHREAD_START_ROUTINE;
typedef void * LPVOID;

class ThreadHelper
{
public:
	static bool spawn( LPTHREAD_START_ROUTINE pthreadFunc, LPVOID lpParam, pthread_t& threadHandle)
	{
		if ( !pthreadFunc )
			return false;
		
		int ret = pthread_create(&threadHandle,NULL,pthreadFunc,lpParam);
		return ret;
	}

	static bool spawn_n( int threadNum, LPTHREAD_START_ROUTINE pthreadFunc, LPVOID lpParam, 
		pthread_t threadHandleArr[]  )
	{
		if ( !pthreadFunc )
			return false;

		for ( int i = 0 ; i < threadNum; i++ )
		{
			spawn( pthreadFunc, lpParam,  threadHandleArr[i]);
		}
		return true;
	}

	static void join( pthread_t threadhandle )
	{
		pthread_join(threadhandle,NULL);
		
	}
	static void join_n( pthread_t threadhandle[], int handleNum )
	{
		for( int i = 0 ; i<handleNum; i++)
		{
			join(threadhandle[i]);
		}
	}

};

#endif
