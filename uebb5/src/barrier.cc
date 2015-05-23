/*************************************************************************************************
*
* Heidelberg University - APC Exercise 04
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        barrier.cc
*
* Last Change : 23. Mai 2015
*
*************************************************************************************************/

#include "barrier.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <pthread.h>

void* pthread_barrier(void* threadargs) {
	struct thread_data* td;
	td = (struct thread_data *) threadargs;
	
	std::this_thread::sleep_for(std::chrono::seconds(td->wait));

	for (int i = 0; i < td->n_barrier; ++i) {
		pthread_barrier_wait(td->shared_barrier);
		++(td->barrier_count);
	}

	if (td->thread_id != 0) {
		pthread_exit(NULL);
	}
}

void* central_barrier(void* threadargs) {
	struct thread_data* td;
	td = (struct thread_data *) threadargs;

	std::this_thread::sleep_for(std::chrono::seconds(td->wait));

	for (int i = 0; i < td->n_barrier; ++i) {

		td->local_sense = !(td->local_sense);
		if (__sync_fetch_and_sub(td->shared_counter, 1) == 1) {
			*(td->shared_counter) = td->thread_count;
			*(td->shared_sense) = td->local_sense;
		}
		else {
			while (*(td->shared_sense) != td->local_sense);
		}
	}
	if (td->thread_id != 0) {
			pthread_exit(NULL);
		}
}

void test_barrier_count (struct thread_data* td, int thread_count) {

	bool check = true;
	for (int i = 0; i < thread_count; ++i) {
		check = check && (td[i].barrier_count == td[0].barrier_count);
	}
	if (!check) {
		std::cout << "***ERROR: barrier count of threads is not equal." << std::endl;
		for (int i = 0; i < thread_count; ++i) {
			std::cout << "Thread " << i << " barrier_count " << td[i].barrier_count << std::endl;
		}
	}
}

void reset_barrier_count (struct thread_data* td, int thread_count) {
	for (int i = 0; i < thread_count; ++i) {
		td[i].barrier_count = 0;
	}
}

