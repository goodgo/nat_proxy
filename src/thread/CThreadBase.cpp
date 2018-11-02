#include "CThreadBase.h"

#define PTHREAD_STACK_MIN 16384
bool	CThreadBase::onStartThread(void *param,void *(*pRun)(void *))
{
	pthread_attr_t pa;
	pthread_t thread;
	if (!pRun)
	{
		DEBUGINFO("param is null");
		return false;
	}

	if (0 != pthread_attr_init(&pa))
	{
		DEBUGINFO("pthread_attr_init failed!");
		return false;
	}
	if (0 != pthread_attr_setstacksize(&pa,PTHREAD_STACK_MIN*8))
	{
		DEBUGINFO("pthread_attr_setstacksize failed!");
		return false;
	}

	if (0 != pthread_attr_setdetachstate(&pa,PTHREAD_CREATE_DETACHED))
	{
		DEBUGINFO("pthread_attr_setdetachstate failed!");
		return false;
	}
	if(0 != pthread_create(&thread, &pa, pRun, param))
	{
		pthread_attr_destroy(&pa);
		DEBUGINFO("create thread failed");
		return false;
	}
	pthread_attr_destroy(&pa);
	return true;
}
