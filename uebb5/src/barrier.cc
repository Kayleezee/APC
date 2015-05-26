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
* Last Change : 25. Mai 2015
*
*************************************************************************************************/

#include "barrier.h"
#include <iostream>
#include <pthread.h>
#include <cmath>
#include <queue>

extern pthread_mutex_t mut;

void* pthread_barrier(void* threadargs) {
	struct pthread_data* td;
	td = (struct pthread_data *) threadargs;
	
	for (int i = 0; i < td->n_barrier; ++i) {
		pthread_barrier_wait(td->shared_barrier);
		++(td->barrier_count);
	}

	if (td->thread_id != 0) {
		pthread_exit(NULL);
	}
}

void* central_barrier(void* threadargs) {
	struct central_data* td;
	td = (struct central_data *) threadargs;

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

void initialize_dissemination(struct dissemination_data* td, int thread_count) {
	for (int i = 0; i < thread_count; ++i) {
		td[i].fl = new struct flag*[(int) ceil(log2(thread_count))];
		for (int r = 0; r < 2; ++r) {
			for (int k = 0; k < log2(thread_count); ++k) {
				td[i].fl[k] = new struct flag[2];
			}
		}
	}
	for (int i = 0; i < thread_count; ++i) {
		td[i].sense = true;
		for (int r = 0; r < 2; ++r) {
			for (int k = 0; k < log2(thread_count); ++k) {
				td[i].fl[k][r].me = false;
				int j = (int) (1 + i + pow(k,2)) % thread_count;
				td[i].fl[k][r].partner = &td[j].fl[k][r].me;
				td[i].fl[k][r].partner_id = j;
			}
		}
	}
}

void initialize_tree(struct tree_data* td, int thread_count) {

	struct node_t* p_node = new struct node_t[thread_count/2 + thread_count % 2];
	struct node_t* p0;
	struct node_t* p1;
	std::queue<struct node_t*> nq;
	int counter = 0;

	for (int i = 0; i < thread_count/2 + thread_count % 2; ++i) {
		p_node[i].id = counter;
		p_node[i].k = 0;
		p_node[i].locksense = false;
		++counter;
		nq.push(&p_node[i]);
	}

	for (int i = 0; i < thread_count; ++i) {
		td[i].node = &p_node[i/2];
		++p_node[i/2].k; 
		p_node[i/2].count = p_node[i/2].k;
	}

	while (nq.size() > 1) {
		p0 = nq.front();
		nq.pop();
		p1 = nq.front();
		nq.pop();
		p_node = new struct node_t;
		p_node->k = 2;
		p_node->count = 2;
		p_node->locksense = false;
		p_node->id=counter;
		++counter;
		p0->parent = p_node;
		p1->parent = p_node;
		nq.push(p_node);
	}
	p_node = nq.front();
	p_node->parent = NULL;
}

void* tree_barrier(void* threadargs) {
	struct tree_data* td = (struct tree_data *) threadargs;

	for (int j = 0; j < td->n_barrier; ++j) {
		combining(td->node, td->sense, td->thread_id);
		td->sense = !td->sense;
		++(td->barrier_count);
	}
}

void combining(struct node_t* pn, bool sense, int id) {
	if (__sync_fetch_and_sub(&pn->count, 1) == 1) {
		if (pn->parent != NULL) {
			combining(pn->parent, sense, id);
		}
		pn->count = pn->k;
		pn->locksense = !pn->locksense;
	}
	while (pn->locksense != sense);
}

void* dissemination_barrier(void* threadargs) {
	struct dissemination_data* td = (struct dissemination_data *) threadargs;

	for (int j = 0; j < td->n_barrier; ++j) {
		for (int i = 0; i < log2(td->thread_count); ++i) {
			struct flag* const f = &(td->fl[i][td->par]);
			*f->partner = td->sense;
			while(f->me != td->sense);
		}
		++(td->barrier_count);
		if (td->par == 1) {
			td->sense = !td->sense;
		}
		td->par ^= 0x1;
	}

	if (td->thread_id != 0) {
		pthread_exit(NULL);
	}
}

template <typename TD>
void test_barrier_count(TD* td, int thread_count) {

	bool check = true;
	for (int i = 0; i < thread_count; ++i) {
		// here one value difference in the passed barriers is valid, because
		// the barrier counter is incremented after the barrier.
		// Therefore the main process might increment faster, although
		// all processes synchronized at the barrier.
		check = check && (abs(td[i].barrier_count - td[0].barrier_count) < 2);
	}
	if (!check) {
		std::cout << "***ERROR: barrier count of threads is not equal." << std::endl;
		for (int i = 0; i < thread_count; ++i) {
			std::cout << "Thread " << i << " barrier_count " << td[i].barrier_count << std::endl;
		}
	}
}

template <typename TD>
void reset_barrier_count(TD* td, int thread_count) {
	for (int i = 0; i < thread_count; ++i) {
		td[i].barrier_count = 0;
	}
}

// here you force the compiler to generate code for this types //
template void test_barrier_count<struct pthread_data>(struct pthread_data*, int);
template void test_barrier_count<struct central_data>(struct central_data*, int);
template void test_barrier_count<struct dissemination_data>(struct dissemination_data*, int);
template void test_barrier_count<struct tree_data>(struct tree_data*, int);
template void reset_barrier_count<struct central_data>(struct central_data*, int);
template void reset_barrier_count<struct pthread_data>(struct pthread_data*, int);
template void reset_barrier_count<struct dissemination_data>(struct dissemination_data*, int);
template void reset_barrier_count<struct tree_data>(struct tree_data*, int);

