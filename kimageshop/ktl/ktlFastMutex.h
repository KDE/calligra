#ifndef _FASTMUTEX_H_INCLUDED
#define _FASTMUTEX_H_INCLUDED 1

extern "C"
{
	#include <pthread.h>
}

#include <FC/Mutex.h>

class FastMutex : public Mutex
{
 public:
   FastMutex();
   virtual ~FastMutex();
   
 private:
   pthread_mutexattr_t *mutexAttr;
};
      
#endif // _FASTMUTEX_H_INCLUDED
