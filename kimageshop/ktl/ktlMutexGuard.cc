#include <ktlMutexGuard.h>

KTL::MutexGuard::MutexGuard(KTL::Mutex *mutex)
  : mutex(mutex)
{
	mutex->lock();
}

KTL::MutexGuard::~MutexGuard()
{
	mutex->unlock();
}
