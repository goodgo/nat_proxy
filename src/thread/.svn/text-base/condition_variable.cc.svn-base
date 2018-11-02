#include "condition_variable.h"

namespace thread_tool
{
	condition_variable::condition_variable()
	{
		if (pthread_mutex_init(&_internal_mutex, NULL))
			throw std::runtime_error("condition variable init failed");
		if (pthread_cond_init(&_cond, NULL))
		{
			pthread_mutex_destroy(&_internal_mutex);
			throw std::runtime_error("condition variable init failed");
		}
	}

	condition_variable::~condition_variable()
	{
		pthread_mutex_destroy(&_internal_mutex);
		pthread_cond_destroy(&_cond);
	}

	void condition_variable::NotifyOne()
	{
		internal_lock_guard internal_guard(_internal_mutex);
		pthread_cond_signal(&_cond);
	}

	void condition_variable::NotifyAll()
	{
		internal_lock_guard internal_guard(_internal_mutex);
		pthread_cond_broadcast(&_cond);
	}

}

