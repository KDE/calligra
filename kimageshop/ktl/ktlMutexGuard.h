#ifndef _MUTEXGUARD_H_INCLUDED
#define _MUTEXGUARD_H_INCLUDED 1


#include <FC/Mutex.h>

class MutexGuard
{
 public:
   MutexGuard(Mutex *mutex);
   ~MutexGuard();
   
 private:
   Mutex *mutex;
};

#endif // _MUTEXGUARD_H_INCLUDED
