#ifndef _KTLTHREADMANAGER_H_INCLUDED
#define _KTLTHREADMANAGER_H_INCLUDED 1

#include <ktlExceptions.h>
#include <ktlThread.h>

#include <map>


// This is the first class to look at when you encounter
// porting problems. On Linux with LinuxThreads, pthread_t
// is a unsigned long, so copying isn't a problem. On other
// systems, pthread_t may be a pointer to a struct, ... or
// even worse, a(n) (expanded) struct.


namespace KTL
{

  DECLARE_EXCEPTION(UnknownThreadException)
  DECLARE_EXCEPTION(ThreadRegistrationException)

  class ThreadManager {
  public:
    ~ThreadManager();
    
    void setCancelStateSelf(KTL::Thread::CancelState state);
    void setCancelTypeSelf(KTL::Thread::CancelType cancelType);
    KTL::Thread *getSelfThread();
    
    static ThreadManager *getThreadManager();
    
    KTL::Thread *getThread(pthread_t threadHandle);
    
  protected:
    void registerThread(pthread_t threadHandle, KTL::Thread *thread);
    void deregisterThread(pthread_t threadHandle);
    
    map<pthread_t, Thread *> threadList;
    
    ThreadManager();
    static ThreadManager *singleton;
    
    friend class KTL::Thread;
  };
  
};
  
#endif // _THREADMANAGER_H_INCLUDED
  
  
