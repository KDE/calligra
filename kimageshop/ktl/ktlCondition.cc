#include <ktlCondition.h>
#include <ktlMutex.h>

KTL::Condition::Condition()
{
  ::pthread_cond_init(&condHandle, NULL);
}

KTL::Condition::~Condition()
{
  ::pthread_cond_destroy(&condHandle);
}

void KTL::Condition::wait(KTL::Mutex *mutex)
{
  ::pthread_cond_wait(&condHandle, mutex->_handle());
}

//bool Condition::timedWait(Mutex *mutex)
//{
//	return false;
//	return ETIMEDOUT != 
//		::pthread_cond_timedwait(&condHandle, mutex->_handle(), ... );
//}

void KTL::Condition::signal()
{
	::pthread_cond_signal(&condHandle);
}

void KTL::Condition::broadcast()
{
	::pthread_cond_broadcast(&condHandle);
}
