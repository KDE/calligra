#ifndef _KTLTHREAD_H_INCLUDED
#define _KTLTHREAD_H_INCLUDED

extern "C"
{
#include <pthread.h>
}

#include <qobject.h>
#include <ktlExceptions.h>

extern void *__entry_threadproc(void *);
extern void  __cleanup_handler(void *);

namespace KTL
{

  DECLARE_EXCEPTION(ThreadException)
  
  class ThreadProc {
  public:
    virtual void threadProc() = 0;
  };
  
  class Thread : public QObject {

    Q_OBJECT

  public:
    typedef enum {
      Enable  = PTHREAD_CANCEL_ENABLE,
      Disable = PTHREAD_CANCEL_DISABLE
    } CancelState; 
    
    typedef enum {
      Asynchronous = PTHREAD_CANCEL_ASYNCHRONOUS,
      Deferred     = PTHREAD_CANCEL_DEFERRED
    } CancelType;
    
  public:
    Thread( ThreadProc *threadProc );
    virtual ~Thread();
    
    void create();
    
    void cancel();
    void detach();
    void join();
    
    static void sleep(unsigned int seconds);
    static void usleep(unsigned long microseconds);
    static void yield();		
    
    void testCancel();
    
    virtual bool operator == (const Thread &thread);
    
  signals:
    void threadStarted();
    void threadEnded();
    
  private:
    void exit();
    
    void __threadEntry();
    void __signalStarted();
    void __signalTerminated();
    
  private:
    ThreadProc *threadProc;
    pthread_t   threadHandle;
    
    friend void * __entry_threadproc(void *);
    friend void __cleanup_handler(void *);
  };
  
};

#endif // _KTLTHREAD_H_INCLUDED
