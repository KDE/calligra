#ifndef _CONDITION_H_INCLUDED
#define _CONDITION_H_INCLUDED 1

extern "C"
{
	#include <pthread.h>
}

class Mutex;

class Condition
{
 public:
   Condition();
   ~Condition();

   void wait(Mutex *mutex);
   // bool timedWait(Mutex *mutex);

   void signal();
   void broadcast();

   pthread_cond_t *_handle() { return &condHandle; }
   
 private:
   pthread_cond_t condHandle;
};

#endif // _CONDITION_H_INCLUDED
