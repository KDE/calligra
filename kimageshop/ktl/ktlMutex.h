#ifndef _KTLMUTEX_H_INCLUDED
#define _KTLMUTEX_H_INCLUDED 1

extern "C"
{
	#include <pthread.h>
}

namespace KTL
{

  class Mutex
    {
    public:
      virtual ~Mutex() {};
      
      virtual void lock();
      virtual void unlock();
      virtual bool trylock();
      
      pthread_mutex_t *_handle() { return mutexHandle; }
      
    protected:
      pthread_mutex_t *mutexHandle;
      
    protected:
      Mutex() {};
    };

};

#endif // _KTLMUTEX_H_INCLUDED
