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
#include <cmath>

extern pthread_mutex_t mut;

void* pthread_barrier(void* threadargs) {
	struct pthread_data* td;
	td = (struct pthread_data *) threadargs;
	
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
	struct central_data* td;
	td = (struct central_data *) threadargs;

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
	int start = 0;
	int end = thread_count/2;
	int odd = thread_count % 2;
	int count = end - start;
	struct node_t* p_node = new node_t[thread_count];

	for (int i = 0; i < thread_count; ++i) {
		td[i].node = &p_node[i/2];
	}

	while (count > 1) {
		std::cout << "count = " << count << std::endl; 
		for(int i = start; i < end; ++i) {
			p_node[i].parent = &p_node[end + odd + (i - start)/2];
			std::cout << "\tNode " << i << " gets parent " << end + odd + (i - start)/2 << std::endl; 
		}
		start = end;
		end = start + count / 2;  
		odd = (count/2 + odd) % 2;
		count = end - start;
	}

	if (odd == 0) {
		p_node[start].parent = &p_node[end+1];
		p_node[end].parent = &p_node[end+1];
	}

}

void* dissemination_barrier(void* threadargs) {
	struct dissemination_data* td = (struct dissemination_data *) threadargs;

	std::this_thread::sleep_for(std::chrono::seconds(td->wait));

/*	pthread_mutex_lock(&mut);
	std::cout << "thread_id " << td->thread_id << std::endl
		      << "\tn_barrier    " << td->n_barrier << std::endl
		      << "\tthread_count " << td->thread_count << std::endl
		      << "\twait         " << td->wait << std::endl
		      << "\tsense        " << td->sense << std::endl
		      << "\tpar          " << td->par << std::endl;

	pthread_mutex_unlock(&mut);*/

	for (int j = 0; j < td->n_barrier; ++j) {
		for (int i = 0; i < log2(td->thread_count); ++i) {
			struct flag* const f = &(td->fl[i][td->par]);
			*f->partner = td->sense;
//			pthread_mutex_lock(&mut);
//			std::cout << "Thread " << td->thread_id << " sended message to thread "
//				      << f->partner_id << std::endl;
//			pthread_mutex_unlock(&mut);
			while(f->me != td->sense);
//			pthread_mutex_lock(&mut);
//			std::cout << "Thread " << td->thread_id << " got message from thread " << std::endl;
//			pthread_mutex_unlock(&mut);
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
		check = check && (td[i].barrier_count == td[0].barrier_count);
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
template void reset_barrier_count<struct central_data>(struct central_data*, int);
template void reset_barrier_count<struct pthread_data>(struct pthread_data*, int);
template void reset_barrier_count<struct dissemination_data>(struct dissemination_data*, int);

