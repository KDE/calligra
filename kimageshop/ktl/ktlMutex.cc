#include <ktlMutex.h>

#include <errno.h>

void KTL::Mutex::lock()
{
	::pthread_mutex_lock(mutexHandle);
}

void KTL::Mutex::unlock()
{
	::pthread_mutex_unlock(mutexHandle);
}

bool KTL::Mutex::trylock()
{
	return ::pthread_mutex_trylock(mutexHandle) != EBUSY;
}
