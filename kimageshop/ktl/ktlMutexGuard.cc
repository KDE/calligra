#include <FC/MutexGuard.h>

MutexGuard::MutexGuard(Mutex *mutex)
  : mutex(mutex)
{
	mutex->lock();
}

MutexGuard::~MutexGuard()
{
	mutex->unlock();
}
