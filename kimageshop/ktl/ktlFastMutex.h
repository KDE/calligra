#ifndef _KTLFASTMUTEX_H_INCLUDED
#define _KTLFASTMUTEX_H_INCLUDED 1

extern "C"
{
#include <pthread.h>
}

#include <ktlMutex.h>

namespace KTL
{

  class FastMutex : public Mutex {
  public:
    FastMutex();
    virtual ~FastMutex();
    
  private:
    pthread_mutexattr_t *mutexAttr;
  };
    
};
  
#endif // _KTLFASTMUTEX_H_INCLUDED
