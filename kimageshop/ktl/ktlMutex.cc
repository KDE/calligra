#include <FC/Mutex.h>

#include <errno.h>

void Mutex::lock()
{
	::pthread_mutex_lock(mutexHandle);
}

void Mutex::unlock()
{
	::pthread_mutex_unlock(mutexHandle);
}

bool Mutex::trylock()
{
	return ::pthread_mutex_trylock(mutexHandle) != EBUSY;
}
