#include <ktlThreadManager.h>
#include <ktlThread.h>

KTL::ThreadManager *KTL::ThreadManager::singleton = NULL;

KTL::ThreadManager::ThreadManager()
{
}

KTL::ThreadManager::~ThreadManager()
{
}

void KTL::ThreadManager::setCancelStateSelf(KTL::Thread::CancelState state)
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

void KTL::ThreadManager::setCancelTypeSelf(KTL::Thread::CancelType cancelType)
{
        switch ( cancelType )
        {
          case KTL::Thread::Asynchronous:
                ::pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
                break;
          case KTL::Thread::Deferred:
                ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );
        };            
}

KTL::Thread *KTL::ThreadManager::getSelfThread()
{
	pthread_t selfThread = pthread_self();

	return threadList[selfThread]; 
}

KTL::ThreadManager *KTL::ThreadManager::getThreadManager()
{
	if (singleton == NULL)
		singleton = new KTL::ThreadManager;

	return singleton;
}

KTL::Thread *KTL::ThreadManager::getThread(pthread_t threadHandle)
{
	map<pthread_t, KTL::Thread *>::const_iterator it;

	it = threadList.find(threadHandle);

	if ( it == threadList.end() )
		throw KTL::UnknownThreadException("Thread is not known");

	return it->second; 
}

void KTL::ThreadManager::registerThread(pthread_t threadHandle,
                                   KTL::Thread *thread)
{
	map<pthread_t, KTL::Thread *>::const_iterator it;

	it = threadList.find(threadHandle);
	if ( it != threadList.end() )
		throw KTL::ThreadRegistrationException("Thread already registered");

	threadList[threadHandle] = thread;
}

void KTL::ThreadManager::deregisterThread(pthread_t threadHandle)
{
	map<pthread_t, KTL::Thread *>::iterator it;

	it = threadList.find(threadHandle);
	if ( it == threadList.end() )
		throw KTL::ThreadRegistrationException("Thread not registered");

	threadList.erase(it);
}

