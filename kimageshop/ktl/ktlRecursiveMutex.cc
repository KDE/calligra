#include <FC/RecursiveMutex.h>

RecursiveMutex::RecursiveMutex()
{
	mutexAttr    = new pthread_mutexattr_t;
	mutexHandle  = new pthread_mutex_t;

	::pthread_mutexattr_init(mutexAttr);
	::pthread_mutexattr_settype(mutexAttr,
		PTHREAD_MUTEX_RECURSIVE_NP);
	::pthread_mutex_init(mutexHandle, mutexAttr);
}

RecursiveMutex::~RecursiveMutex()
{
	::pthread_mutex_destroy(mutexHandle);
	::pthread_mutexattr_destroy(mutexAttr);

	delete mutexHandle;
	delete mutexAttr;
}
