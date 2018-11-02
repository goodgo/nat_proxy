#ifndef _THREAD_LOCKS_INCLUDE_
#define _THREAD_LOCKS_INCLUDE_
#include <stdexcept>

namespace thread_tool
{
	struct defer_lock_t {};
	struct try_to_lock_t {};
	struct adopt_lock_t {};

	const defer_lock_t defer_lock = {};
	const try_to_lock_t try_to_lock = {};
	const adopt_lock_t adopt_lock = {};

	template <typename Mutex>
	class lock_guard
	{
	private:
		explicit lock_guard(lock_guard&);
		lock_guard& operator=(lock_guard&);

	public:
		explicit lock_guard(Mutex& m)
			: _m(m)
		{
			_m.Lock();
		}

		lock_guard(Mutex& m, adopt_lock_t)
			: _m(m)
		{
		}

		~lock_guard()
		{
			_m.Unlock();
		}

	private:
		Mutex& _m;
	};

	template<typename Mutex>
	class unique_lock
	{
	private:
		explicit unique_lock(unique_lock&);
		unique_lock& operator=(unique_lock&);

	public:
		unique_lock()
			: _m(NULL)
			, _locked(false)
		{
		}

		explicit unique_lock(Mutex& m)
			: _m(&m)
			, _locked(false)
		{
			Lock();
		}

		explicit unique_lock(Mutex& m, defer_lock_t)
			: _m(&m)
			, _locked(false)
		{
		}

		explicit unique_lock(Mutex& m, try_to_lock_t)
			: _m(&m)
			, _locked(false)
		{
			TryLock();
		}

		explicit unique_lock(Mutex& m, adopt_lock_t)
			: _m(&m)
			, _locked(true)
		{
		}

		void swap(unique_lock& other)
		{
			std::swap(_m, other._m);
			std::swap(_locked, other._locked);
		}

		~unique_lock()
		{
			if (_locked)
				_m->Unlock();
		}

		void Lock()
		{
			if (_locked)
				throw std::logic_error("already have locked");
			_m->Lock();
			_locked = true;
		}

		bool TryLock()
		{
			if (_locked)
				throw std::logic_error("already have locked");
			_locked = _m->TryLock();
			return _locked;
		}

		bool TimedLock(unsigned int wait_usec)
		{
			if (_locked)
				throw std::logic_error("already have locked");
			_locked = _m->TimedLock(wait_usec);
			return _locked;
		}

		void Unlock()
		{
			if (!_locked)
				throw std::logic_error("unlock when do not have lock");

			_m->Unlock();
			_locked = false;
		}

		bool IsLocked() const
		{
			return _locked;
		}

		bool operator!() const
		{
			return !IsLocked();
		}

		Mutex* mutex() const
		{
			return _m;
		}

		Mutex* release()
		{
			const Mutex * ret = _m;
			_m = NULL;
			_locked = false;
			return ret;
		}

	private:
		Mutex * _m;
		bool _locked;
	};

	template<typename Mutex>
	class shared_lock
	{
	private:
		explicit shared_lock(shared_lock&);
		shared_lock& operator=(shared_lock&);

	public:
		shared_lock()
			: _m(NULL)
			, _locked(false)
		{
		}

		explicit shared_lock(Mutex& m)
			: _m(&m)
			, _locked(false)
		{
			Lock();
		}

		explicit shared_lock(Mutex& m, defer_lock_t)
			: _m(&m)
			, _locked(false)
		{
		}

		explicit shared_lock(Mutex& m, try_to_lock_t)
			: _m(&m)
			, _locked(false)
		{
			TryLock();
		}

		explicit shared_lock(Mutex& m, adopt_lock_t)
			: _m(&m)
			, _locked(true)
		{
		}

		~shared_lock()
		{
			if (_locked)
				_m->UnlockShared();
		}

		void swap(shared_lock& other)
		{
			std::swap(_m, other._m);
			std::swap(_locked, other._locked);
		}

		void Lock()
		{
			if (_locked)
				throw std::logic_error("already have locked");
			_m->LockShared();
			_locked = true;
		}

		bool TryLock()
		{
			if (_locked)
				throw std::logic_error("already have locked");
			_locked = _m->TryLockShared();
			return _locked;
		}

		bool TimedLock(unsigned int wait_usec)
		{
			if (_locked)
				throw std::logic_error("already have locked");
			_locked = _m->TimedLockShared(wait_usec);
			return _locked;
		}

		void Unlock()
		{
			if (!_locked)
				throw std::logic_error("unlock when do not have lock");

			_m->UnlockShared();
			_locked = false;
		}

		bool IsLocked() const
		{
			return _locked;
		}

		bool operator!() const
		{
			return !IsLocked();
		}

		Mutex* mutex() const
		{
			return _m;
		}

		Mutex* release()
		{
			const Mutex * ret = _m;
			_m = NULL;
			_locked = false;
			return ret;
		}

	private:
		Mutex * _m;
		bool _locked;
	};
}

#endif //_THREAD_LOCKS_INCLUDE_

