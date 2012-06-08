///
/// The Thread abstract class.
/// This class purpose is to provided threaded capability to whatever child class that inherits from it.
/// It is based on pthreads and it's foreseen to be used while the official support from the new C++11 is not fully
/// implemented on GNU gcc.
/// This class can not be directly instantiated.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include <iostream>

class Thread
{
public:

    /** Default constructor.
     * By calling this constructor, a pthread mutex shall be initialised.
     */
	Thread();

	/** Default destructor.
	 * When called, it will wait ("join") for the current thread to terminate and destroy the available mutex.
	 * Child classes must be sure to create their own mechanisms for the threaded method  ("job"), to quit,
	 * otherwise it may be stuck while deleting itself.
	 */
	virtual ~Thread();

protected:

    /** This method creates the thread itself. */
	void openThread();

	/** At this point this method does nothing...
	 * \note Maybe a call for killing the thread.
	 */
	void closeThread();

    /** This method shall use the pthread mutext variable member for locking
     * \sa Thread()
     * \sa mutex_
     */
	void lock();

	/** This method shall use the pthread mutext variable member for unlocking
	 * \sa Thread()
	 * \sa mutex_
	 */
	void unlock();

	/** This method shall try to lock the pthread mutex.
	 * \return A boolean value. "true" if lock was successful and "false" if otherwise.
	 * \sa Thread()
	 * \sa mutex_
	 */
	bool tryLock();

    /** This method is the one who actually takes care of whatever work is to be processed inside the opened thread.
     * This method is an abstract one, so it has to be implemented by whatever class inherits from Thread.
     */
	virtual void job() = 0;

private:

    /** This private static method is called as a parameter of the pthread create_thread function to open the thread which will
     * process whatever is implemented inside execute. For this class to support thread creation without the mandatory usage of
     * all static member variables, it will call the purely virtual protected job method. For this to be accomplished the "this"
     * variable is passed as an argument, so that the call can be made.
     * \param arg A pointer to void. The "this" object (thus the Thread), is passed as argument to allow calling the job method that
     * shall be implemented by whatever class that inherits from Thread.
     * \sa job
     */
	static void* execute(void* arg);

	pthread_t pthread_; /*!< pthread_t member variable to hold the thread that shall be used. */
	pthread_mutex_t mutex_; /*!< pthread_mutex_t memeber variable to provide mutex support to the opened thread. */


};

#endif // THREAD_H
