#include <FC/Condition.h>
#include <FC/Mutex.h>

Condition::Condition()
{
	::pthread_cond_init(&condHandle, NULL);
}

Condition::~Condition()
{
	::pthread_cond_destroy(&condHandle);
}

void Condition::wait(Mutex *mutex)
{
	::pthread_cond_wait(&condHandle, mutex->_handle());
}

//bool Condition::timedWait(Mutex *mutex)
//{
//	return false;
//	return ETIMEDOUT != 
//		::pthread_cond_timedwait(&condHandle, mutex->_handle(), ... );
//}

void Condition::signal()
{
	::pthread_cond_signal(&condHandle);
}

void Condition::broadcast()
{
	::pthread_cond_broadcast(&condHandle);
}
