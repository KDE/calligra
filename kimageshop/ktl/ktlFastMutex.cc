#include <FC/FastMutex.h>

FastMutex::FastMutex()
{
	mutexAttr    = new pthread_mutexattr_t;
	mutexHandle  = new pthread_mutex_t;

	::pthread_mutexattr_init(mutexAttr);
	::pthread_mutexattr_settype(mutexAttr,
		PTHREAD_MUTEX_FAST_NP);
	::pthread_mutex_init(mutexHandle, mutexAttr);
}

FastMutex::~FastMutex()
{
	::pthread_mutex_destroy(mutexHandle);
	::pthread_mutexattr_destroy(mutexAttr);

	delete mutexHandle;
	delete mutexAttr;
}
