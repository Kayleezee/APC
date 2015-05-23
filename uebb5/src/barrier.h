/*************************************************************************************************
*
* Heidelberg University - APC Exercise 04
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        barrier.h
*
* Last Change : 23. Mai 2015
*
*************************************************************************************************/

#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

struct thread_data {
	int thread_id;
	int barrier_count;
	int wait;
	volatile bool* shared_sense;
	int* shared_counter;
	bool local_sense;
	int thread_count;
	pthread_barrier_t* shared_barrier;
	int n_barrier;
};

void* pthread_barrier(void* threadargs); 
void* central_barrier(void* threadargs);
void reset_barrier_count(struct thread_data* td, int thread_count);
void test_barrier_count(struct thread_data* td, int thread_count);

#endif
