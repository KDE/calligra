#ifndef _KTLMUTEXGUARD_H_INCLUDED
#define _KTLMUTEXGUARD_H_INCLUDED 1

#include <ktlMutex.h>

namespace KTL
{
  
  class MutexGuard {
  public:
    MutexGuard(Mutex *mutex);
    ~MutexGuard();
    
  private:
    KTL::Mutex *mutex;
  };
  
};

#endif // _MUTEXGUARD_H_INCLUDED
