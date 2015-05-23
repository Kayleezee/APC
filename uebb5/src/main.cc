/*************************************************************************************************
*
* Heidelberg University - APC Exercise 04
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        main.cc
*
* Last Change : 23. Mai 2015
*
*************************************************************************************************/

#include "time_measurement.h"
#include "chCommandLine.h"
#include "barrier.h"

#include <pthread.h>
#include <iostream>
#include <iomanip>


static const int DEFAULT_THREAD_COUNT = 4;
static const int DEFAULT_NBARRIER = 1000;



int main (int argc, char* argv[]) {

	// get thread count //
	int thread_count = -1;
	chCommandLineGet<int>(&thread_count, "t", argc, argv);
	thread_count = (thread_count == -1) ? DEFAULT_THREAD_COUNT : thread_count;

	// get wanted barrier count //
	int n_barrier = 0;
	chCommandLineGet<int>(&n_barrier, "n", argc, argv);
	n_barrier = (n_barrier == 0) ? DEFAULT_NBARRIER : n_barrier;
	
	std::cout << "I got these command line arguments:" << std::endl
		      << "\tn_barrier    = " << n_barrier << std::endl
		      << "\tthread_count = " << thread_count << std::endl;

	// initialize pthread barrier //	
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, thread_count); 

	// initialize threads and their function arguments //
	struct thread_data* td = new struct thread_data[thread_count];
	pthread_t* threads = new pthread_t[thread_count];

	// initialize general thread arguments //
	volatile bool sense = true;	
	int P = thread_count;
	int rc; // for error in thread creation
	double pthread_barrier_per_second;
	double central_barrier_per_second;
	TimeMeasurer tm;	

	for (int i = 0; i < thread_count; ++i) {
		td[i].thread_id = i;
		td[i].barrier_count = 0;
		td[i].wait = 0;
		td[i].shared_sense = &sense; // shared sense variable
		td[i].shared_counter = &P; // shared count variable
		td[i].local_sense = true;
		td[i].thread_count = thread_count;
		td[i].shared_barrier = &barrier;
		td[i].n_barrier = n_barrier;
	}
	td[1].wait = 1;	

/*******************
 * PTHREAD BARRIER *
 *******************/
	std::cout << "I am going to test the pthread barrier performance ..." << std::endl;
	tm.start("pthread");
	for (int i = 1; i < thread_count; ++i) {
		rc = pthread_create(&threads[i], NULL, pthread_barrier, (void *) &td[i]);
		if (rc) {
			std::cout << "***ERROR: could not create thread for pthread barrier" << std::endl;
			exit(-1);
		}
	}
	library_barrier((void *) &td[0]);
	tm.stop("pthread");
	test_barrier_count(td, thread_count);
	reset_barrier_count(td, thread_count);
	pthread_barrier_per_second = ((double) n_barrier) / tm.get_time("pthread");

/*******************
 * CENTRAL BARRIER *
 *******************/
	std::cout << "I am going to test the central barrier performance ..." << std::endl;
	tm.start("central");
	for (int i = 1; i < thread_count; ++i) {
		rc = pthread_create(&threads[i], NULL, central_barrier, (void *) &td[i]);
		if (rc) {
			std::cout << "***ERROR: could not create thread for central barrier" << std::endl;
			exit(-1);
		}
	}
	central_barrier((void *) &td[0]);
	tm.stop("central");
	test_barrier_count(td, thread_count);

	reset_barrier_count(td, thread_count);
	central_barrier_per_second = ((double) n_barrier) / tm.get_time("central");

/************
 * FINALIZE *
 ************/
	std::cout << "\nProgram Report:" << std::endl
		      << "\tn_barrier " << n_barrier << std::endl
		      << "\tthread_count " << thread_count << std::endl
		      << "\tpthread_barrier_per_second " << pthread_barrier_per_second << std::endl
              << "\tcentral_barrier_per_second " << central_barrier_per_second << std::endl;

	delete[] td;
	delete[] threads;

	pthread_exit(NULL);
}
