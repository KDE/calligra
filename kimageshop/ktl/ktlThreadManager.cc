#include <FC/ThreadManager.h>
#include <FC/Thread.h>

ThreadManager *ThreadManager::singleton = NULL;

ThreadManager::ThreadManager()
{
}

ThreadManager::~ThreadManager()
{
}

void ThreadManager::setCancelStateSelf(Thread::CancelState state)
{
	switch ( state )
	{
	  case Thread::Enable:
		::pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
		break;
          case Thread::Disable:
		::pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
	};
}

void ThreadManager::setCancelTypeSelf(Thread::CancelType cancelType)
{
        switch ( cancelType )
        {
          case Thread::Asynchronous:
                ::pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
                break;
          case Thread::Deferred:
                ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );
        };            
}

Thread *ThreadManager::getSelfThread()
{
	pthread_t selfThread = pthread_self();

	return threadList[selfThread]; 
}

ThreadManager *ThreadManager::getThreadManager()
{
	if (singleton == NULL)
		singleton = new ThreadManager;

	return singleton;
}

Thread *ThreadManager::getThread(pthread_t threadHandle)
{
	map<pthread_t, Thread *>::const_iterator it;

	it = threadList.find(threadHandle);

	if ( it == threadList.end() )
		throw UnknownThreadException("Thread is not known");

	return it->second; 
}

void ThreadManager::registerThread(pthread_t threadHandle,
                                   Thread *thread)
{
	map<pthread_t, Thread *>::const_iterator it;

	it = threadList.find(threadHandle);
	if ( it != threadList.end() )
		throw ThreadRegistrationException("Thread already registered");

	threadList[threadHandle] = thread;
}

void ThreadManager::deregisterThread(pthread_t threadHandle)
{
	map<pthread_t, Thread *>::iterator it;

	it = threadList.find(threadHandle);
	if ( it == threadList.end() )
		throw ThreadRegistrationException("Thread not registered");

	threadList.erase(it);
}

