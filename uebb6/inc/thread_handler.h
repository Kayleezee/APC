/******************************************************************
 *
 * FILENAME    - thread_handler.h
 *
 * DESCRIPTION - provides thread a generic thread handling class
 *
 * AUTHOR      - Naumann, Klein
 *
 * LAST CHANGE - 2015-05-30
 *
 ******************************************************************/ 

#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

#include <pthread.h>
#include <vector>
#include <exception>

using namespace std;

class thread_creation_error : public exception {
	public:
		const char* what() const throw() {
			return "Object of class ThreadHandler could not create thread.";
		}
};


// Thread Argument Type and Thread Argument Container Type //
template<typename TArg_t, typename TArg_Cont_t = vector<TArg_t> >
class ThreadHandler {
	public:
		/*
		 *  DESCRIPTION - Constructor
		 *  PARAMETER   - tcount: total worker/thread count
		 *              - job_function: pointer to the worker function, which
		 *                should be executed by the workers.
		 *              - tArgs: this argument must be of a type, which provides
		 *                the [] access operator. The [] access operator must
		 *                return an object of type TArg_t in the template
		 *                specialization. The default is an STL vector.
		 *                tArgs[0] is reserved for the main process if it is
		 *                included as worker with the following parameter.
		 *              - main_works: if true the main process executes the
		 *                job_function too. if true main must be contained
		 *                in the tcount parameter.
		 */
		ThreadHandler(int tcount, void* (*job_function)(void *),
		              TArg_Cont_t& tArgs, bool main_works = true) :
		              tcount_(tcount),
					  job_func_(job_function),
					  p_tArg_(tArgs),
					  main_works_(main_works) {}

		/*
		 *  DESCRIPTION - This function creates the threads and
		 *                executes the job function for each thread.
		 *                If main_works_ == true the main function
		 *                also executes the job_function.
		 */
		pthread_t* doWork() const {
			pthread_t* p_t;
			// threads to create count //
			int mworks = (main_works_ ? 1 : 0);
			p_t = new pthread_t[tcount_];
			int rc;
			for (int i = mworks; i < tcount_; ++i) {
				rc = pthread_create(&p_t[i], NULL, job_func_, (void *) &p_tArg_[i]);
				if (rc) {
					throw thread_creation_error(); 
				}
			}
			if (main_works_) {
				job_func_((void *) &p_tArg_[0]);
			}
			return p_t;
		}

	private:
		// Pointer to a function, which takes a void pointer as an argument
		// and gives back a void pointer.
		// This is the pthreads convention.
		void* (*job_func_)(void *);
		int tcount_;
		bool main_works_; // true: main process is a worker and included in t_count_ //
		bool sync_; // true: threads are synchronized after job execution
		TArg_Cont_t p_tArg_;
};



#endif
