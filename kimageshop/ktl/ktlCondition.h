#ifndef _KTLCONDITION_H_INCLUDED
#define _KTLCONDITION_H_INCLUDED 1

extern "C"
{
#include <pthread.h>
}

namespace KTL
{

  class Mutex;

  class Condition {
  public:
    Condition();
    ~Condition();
    
    void wait(KTL::Mutex *mutex);
    // bool timedWait(Mutex *mutex);
    
    void signal();
    void broadcast();
    
    pthread_cond_t *_handle() { return &condHandle; }
    
  private:
    pthread_cond_t condHandle;
  };

};
  
#endif // _KTLCONDITION_H_INCLUDED
