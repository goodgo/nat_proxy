#ifndef CMUTEX_H
#define CMUTEX_H

#include <pthread.h>

class CMutex   
{   
public:   
	CMutex (bool be_initial_owner = false)   
	{   
		pthread_mutex_init(&mtx,NULL);
	}   
	~CMutex (void)   
	{  
		pthread_mutex_destroy( &mtx );
	}   
public:   
	int acquire ()   
	{   
		pthread_mutex_lock( &mtx );
		return 1;
	}   
	int release ()   
	{   
		pthread_mutex_unlock( &mtx );
		return 1;
	}   

	void Lock()
	{
		pthread_mutex_lock( &mtx );
	}

	void Unlock()
	{
		pthread_mutex_unlock( &mtx );
	}

protected:
	pthread_mutex_t mtx;
};  

class NullMutex
{
public:   
	int acquire ()   
	{   
		return 0;
	}   

	int release ()   
	{   
		return 0;
	}   
};

template<class T>
class Lock
{
private:
	Lock( const Lock& );
	Lock& operator=( const Lock& );

public:
	Lock( T& _t ):_lockobj(_t)
	{
		_lockobj.acquire();
	}

	~Lock()
	{
		_lockobj.release();
	}

private:
	T& _lockobj;
};

typedef Lock<CMutex>    MutexLock;
typedef Lock<NullMutex> NullLock;

struct MutexPolicy
{
	typedef MutexLock LockWrapper;
	typedef CMutex    LockType;
};

struct NullPolicy
{
	typedef NullLock   LockWrapper;
	typedef NullMutex  LockType;
};


#endif
