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

struct pthread_data {
	int thread_id;
	int barrier_count;
	int wait;
	pthread_barrier_t* shared_barrier;
	int n_barrier;
};

struct central_data {
	int thread_id;
	int barrier_count;
	int wait;
	volatile bool* shared_sense;
	int* shared_counter;
	bool local_sense;
	int thread_count;
	int n_barrier;
};

struct flag {
	volatile bool me;
	volatile bool* partner;
	int partner_id;
};

struct dissemination_data {
	int thread_count;
	int thread_id;
	int barrier_count;
	int wait;
	int n_barrier;
	struct flag** fl;
	bool sense;
	int par;
};

struct node_t {
	node_t* parent;
};

struct tree_data {
	int thread_count;
	int thread_id;
	int barrier_count;
	int wait;
	int n_barrier;
	node_t* node;
};

void* pthread_barrier(void* threadargs); 
void* central_barrier(void* threadargs);
void* dissemination_barrier(void* threadargs);

void initialize_dissemination(struct dissemination_data* td, int thread_count);
void initialize_tree(struct tree_data* td, int thread_count);

template <typename TD>
void reset_barrier_count(TD* td, int thread_count);
template <typename TD>
void test_barrier_count(TD* td, int thread_count);

#endif
