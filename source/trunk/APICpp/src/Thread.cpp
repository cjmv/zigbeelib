///
/// Definition of Thread class.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "Thread.h"

using namespace std;

// Default constructor
Thread::Thread()
{
	pthread_mutex_init(&mutex_, 0);
}

// Default Destructor
Thread::~Thread()
{
	pthread_join(pthread_, 0);
	pthread_mutex_destroy(&mutex_);
}

// Execute method
void* Thread::execute(void* arg)
{
	Thread *thread = static_cast<Thread*>(arg);

	thread->job();
	return 0;
}

// openThread method
void Thread::openThread()
{
	if (pthread_create(&pthread_, 0, &execute, this) != 0)
	{
		cerr << "Thread creating failed..." << endl;
		//run_ = false;
	}
}

// closeThread method. It would be nice, I'm just not quite sure if can implemente it.
void Thread::closeThread()
{
	//pthread_exit(0);
}

// lock method
void Thread::lock()
{
   pthread_mutex_lock( &mutex_ );
}

// unlock method
void Thread::unlock()
{
   pthread_mutex_unlock( &mutex_ );
}

// tryLock method
bool Thread::tryLock()
{
	if(pthread_mutex_trylock(&mutex_) == 0)
		return true;
	else
		return false;
}
