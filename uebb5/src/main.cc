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

#include <thread>
#include <chrono>
#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <cmath>

static const int DEFAULT_THREAD_COUNT = 4;
static const int DEFAULT_NBARRIER = 1000;

pthread_mutex_t mut;


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
	struct pthread_data* ptd = new struct pthread_data[thread_count];
	struct central_data* ctd = new struct central_data[thread_count];
	struct dissemination_data* dtd = new struct dissemination_data[thread_count];
	pthread_t* threads = new pthread_t[thread_count];

	// initialize general thread arguments //
	volatile bool sense = true;	
	int P = thread_count;
	int rc; // for error in thread creation
	double pthread_barrier_per_second;
	double central_barrier_per_second;
	double dissemination_barrier_per_second;
	pthread_mutex_init(&mut, NULL);
	TimeMeasurer tm;	
	
	for (int i = 0; i < thread_count; ++i) {
		ptd[i].thread_id = i;
		ctd[i].thread_id = i;
		dtd[i].thread_id = i;
		ptd[i].barrier_count = 0;
		ctd[i].barrier_count = 0;
		dtd[i].barrier_count = 0;
		ptd[i].wait = 0;
		ctd[i].wait = 0;
		dtd[i].wait = 0;
		ptd[i].n_barrier = n_barrier;
		ctd[i].n_barrier = n_barrier;
		dtd[i].n_barrier = n_barrier;

		dtd[i].thread_count = thread_count;
		dtd[i].par = 0;

		ctd[i].thread_count = thread_count;
		ctd[i].shared_sense = &sense; // shared sense variable
		ctd[i].shared_counter = &P; // shared count variable
		ctd[i].local_sense = true;

		ptd[i].shared_barrier = &barrier;
	}
	std::cout << "initialize_dissemination" << std::endl;
	initialize_dissemination(dtd, thread_count);
	ctd[1].wait = 1;	
	ptd[1].wait = 1;
	dtd[1].wait = 5;
	for (int i = 0; i < thread_count; ++i) {
		std::cout << "Thread " << i << " has the following partners:" << std::endl;
		for (int k = 0; k < log2(thread_count); ++k) {
			std::cout << "\t" << dtd[i].fl[k][0].partner_id << std::endl;
		}
	}


/*******************
 * PTHREAD BARRIER *
 *******************/
	std::cout << "I am going to test the pthread barrier performance ..." << std::endl;
	tm.start("pthread");
	for (int i = 1; i < thread_count; ++i) {
		rc = pthread_create(&threads[i], NULL, pthread_barrier, (void *) &ptd[i]);
		if (rc) {
			std::cout << "***ERROR: could not create thread for pthread barrier" << std::endl;
			exit(-1);
		}
	}
	pthread_barrier((void *) &ptd[0]);
	tm.stop("pthread");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	test_barrier_count(ptd, thread_count);
	reset_barrier_count(ptd, thread_count);
	pthread_barrier_per_second = ((double) n_barrier) / tm.get_time("pthread");

/*******************
 * CENTRAL BARRIER *
 *******************/
	std::cout << "I am going to test the central barrier performance ..." << std::endl;
	tm.start("central");
	for (int i = 1; i < thread_count; ++i) {
		rc = pthread_create(&threads[i], NULL, central_barrier, (void *) &ctd[i]);
		if (rc) {
			std::cout << "***ERROR: could not create thread for central barrier" << std::endl;
			exit(-1);
		}
	}
	central_barrier((void *) &ctd[0]);
	tm.stop("central");

	std::this_thread::sleep_for(std::chrono::seconds(1));
	test_barrier_count(ctd, thread_count);
	reset_barrier_count(ctd, thread_count);
	central_barrier_per_second = ((double) n_barrier) / tm.get_time("central");

/*************************
 * DISSEMINATION BARRIER *
 *************************/
	std::cout << "I am going to test the dissemination barrier performance ..." << std::endl;
	tm.start("dissemination");
	for (int i = 1; i < thread_count; ++i) {
		rc = pthread_create(&threads[i], NULL, dissemination_barrier, (void *) &dtd[i]);
		if (rc) {
			std::cout << "***ERROR: could not create thread for dissemination barrier" << std::endl;
			exit(-1);
		}
	}
	dissemination_barrier((void *) &dtd[0]);
	tm.stop("dissemination");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	test_barrier_count(dtd, thread_count);
	reset_barrier_count(dtd, thread_count);
	dissemination_barrier_per_second = ((double) n_barrier) / tm.get_time("dissemination");

/************
 * FINALIZE *
 ************/
	std::cout << "\nProgram Report:" << std::endl
		      << "\tn_barrier " << n_barrier << std::endl
		      << "\tthread_count " << thread_count << std::endl
		      << "\tpthread_barrier_per_second " << pthread_barrier_per_second << std::endl
              << "\tcentral_barrier_per_second " << central_barrier_per_second << std::endl
              << "\tdissemination_barrier_per_second " << dissemination_barrier_per_second << std::endl;

	delete[] ptd;
	delete[] ctd;
	delete[] threads;
	
	pthread_exit(NULL);
}
