#include <ktlThread.h>
#include <ktlThreadManager.h>
#include <ktlExceptions.h>

#include <iostream>

extern "C"
{
#include <pthread.h>
#include <sched.h>
#include <unistd.h>	
}

void __cleanup_handler(void * arg)
{
  ((KTL::Thread *)arg)->__signalTerminated();
}

void *__entry_threadproc(void * arg)
{
  KTL::Thread *thread = (KTL::Thread *)arg;
  
  thread->__signalStarted();
  
  pthread_cleanup_push(__cleanup_handler, thread);
  
  thread->__threadEntry();
  
  pthread_cleanup_pop(1);
  
  return NULL;
}

KTL::Thread::Thread(KTL::ThreadProc *threadProc)
{
  if (threadProc == NULL)
    throw KTL::NullPointerException("ThreadProc is NULL");
  
  this->threadProc = threadProc;
}

KTL::Thread::~Thread()
{
}

void KTL::Thread::create()
{
  ::pthread_create(&threadHandle, NULL, 
		   __entry_threadproc, this);
}

void KTL::Thread::detach()
{
  if ( ::pthread_detach(threadHandle) != 0 )
    throw KTL::ThreadException("KTL::Thread::detach error");
  
}

void KTL::Thread::join()
{
  if ( ::pthread_join(threadHandle, NULL) != 0 )
    throw KTL::ThreadException("Thread::join error");
}

void KTL::Thread::cancel()
{
  if ( ::pthread_cancel(threadHandle) != 0 )
    throw KTL::ThreadException("Thread::cancel");
}

void KTL::Thread::testCancel()
{
  ::pthread_testcancel();
}

void KTL::Thread::sleep(unsigned int seconds)
{
  ::sleep(seconds);
}

void KTL::Thread::usleep(unsigned long microseconds)
{
  ::usleep(microseconds);
}

void KTL::Thread::yield()
{
  ::sched_yield();
}

bool KTL::Thread::operator == ( const KTL::Thread &thread )
{
  return ::pthread_equal(threadHandle, thread.threadHandle);
}

void KTL::Thread::exit()
{
  ::pthread_exit(NULL);
}

void KTL::Thread::__threadEntry()
{
  threadProc->threadProc();
}

void KTL::Thread::__signalStarted()
{
  KTL::ThreadManager *manager =
    KTL::ThreadManager::getThreadManager();
  manager->registerThread(threadHandle, this);
  
  emit threadStarted();
}

void KTL::Thread::__signalTerminated()
{
  emit threadEnded();
  
  KTL::ThreadManager *manager =
    KTL::ThreadManager::getThreadManager();
  manager->deregisterThread(threadHandle);       
}

#include "ktlThread.moc"
