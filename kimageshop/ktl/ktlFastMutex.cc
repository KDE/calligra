#include <ktlFastMutex.h>

KTL::FastMutex::FastMutex()
{
  mutexAttr    = new pthread_mutexattr_t;
  mutexHandle  = new pthread_mutex_t;
  
#ifdef __USE_UNIX98
  ::pthread_mutexattr_init(mutexAttr);
  ::pthread_mutexattr_settype(mutexAttr,
			      PTHREAD_MUTEX_FAST_NP);
  ::pthread_mutex_init(mutexHandle, mutexAttr);
#else
  ::pthread_mutex_init(mutexHandle, NULL);
#endif
}

KTL::FastMutex::~FastMutex()
{
	::pthread_mutex_destroy(mutexHandle);
	::pthread_mutexattr_destroy(mutexAttr);

	delete mutexHandle;
	delete mutexAttr;
}
