#ifndef _RECURSIVEMUTEX_H_INCLUDED
#define _RECURSIVEMUTEX_H_INCLUDED 1

extern "C"
{
	#include <pthread.h>
}

#include <FC/Mutex.h>

class RecursiveMutex : public Mutex
{
 public:
   RecursiveMutex();
   virtual ~RecursiveMutex();
   
 private:
   pthread_mutexattr_t *mutexAttr;
};

#endif // _RECURSIVEMUTEX_H_INCLUDED
      
