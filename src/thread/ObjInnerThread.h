#ifndef OBJ_INNER_THREAD_H
#define OBJ_INNER_THREAD_H

#include "ThreadHelper.h"
#include "CDefine.h"
#include "CInclude.h"

template<class T>
class ObjInnerThread
{
public:
	ObjInnerThread()
		: m_bTerminate(false)
		, hpid(0)
		, hThreadHandle(INVALID_HANDLE_VALUE)
	{
		flag = 1;
		pthread_mutex_init(&hmutex,NULL); 
		pthread_cond_init(&hcond,NULL);
	}

	virtual ~ObjInnerThread()
	{
		StopInnerThread();

		pthread_mutex_destroy(&hmutex);
		pthread_cond_destroy(&hcond);
	}


public:

	static void Run( LPVOID lpParam)
	{
		T* lpT = (T *)lpParam;
		lpT->hpid = GETPID;
		if(lpT)
		{
			while( !lpT->IsNeedStop() )
			{
				//lpT->ThreadNeedSuspend();
				lpT->InnerThreadProc();
			}
		}
	}

	void StartInnerThread(T* threadOwner )
	{
		if ( !threadOwner)
			return;

		ThreadHelper::spawn( (LPTHREAD_START_ROUTINE)ObjInnerThread<T>::Run, threadOwner,hThreadHandle);
	}

	void SuspendInnerThread()
	{
		if((long)hThreadHandle !=INVALID_HANDLE_VALUE)
		{
			pthread_mutex_lock(&hmutex);  
			flag--;  
			pthread_mutex_unlock(&hmutex);  
		}
	}

	void ResumeInnerThread()
	{
		if((long)hThreadHandle !=INVALID_HANDLE_VALUE)
		{
			pthread_mutex_lock(&hmutex);  
			flag++;  
			pthread_cond_signal(&hcond);  
			pthread_mutex_unlock(&hmutex);  
		}
	}

	void StopInnerThread()
	{
		if ( (long)hThreadHandle != INVALID_HANDLE_VALUE  )
		{
			 m_bTerminate = true;
			 hThreadHandle = INVALID_HANDLE_VALUE;
		}
	}

	bool IsInnerThreadRunning()
	{
		return (long)hThreadHandle!= INVALID_HANDLE_VALUE;
	}

	bool IsNeedStop()
	{
		return m_bTerminate;
	}

	void ThreadNeedSuspend()
	{
		pthread_mutex_lock(&hmutex);  
		while(flag <= 0)  
		{  
			pthread_cond_wait(&hcond, &hmutex);  
		}  
		pthread_mutex_unlock(&hmutex);
	}

	pid_t Getpid()
	{
		return hpid;
	}

protected:
	volatile  bool m_bTerminate;
	pid_t 	  hpid;
	pthread_t hThreadHandle;//
	pthread_mutex_t hmutex;//
	pthread_cond_t  hcond;//
	int flag;
};


#endif
