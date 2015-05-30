/******************************************************************
 *
 * FILENAME    - thread_routines.cc
 *
 * DESCRIPTION - This file provides the thread routines for the
 *               parallel prefix sum.
 *
 * AUTHOR      - Naumann, Klein
 *
 * LAST CHANGE - 2015-05-30
 *
 ******************************************************************/ 

#include <iostream>
#include <vector>
#include <random>
#include <exception>

#include "thread_arg.h"
#include "thread_routines.h"

int partial_scan (vector<unsigned int>* v, size_t start, size_t size) {
	if (size < 1 || start < 0 || start >= v->size()) {
		throw index_error("int partial_scan");
	}
	int sum = v->at(start + size - 1);
	for (int i = start+1; i < start + size; ++i) {
		v->at(i) += v->at(i-1);
	}
	v->at(start) = 0;
	return sum + v->at(start + size - 1);
}

void partial_add (vector<unsigned int>* v, size_t start, size_t size, unsigned int val) {
	if (size < 1 || start < 0 || start >= v->size()) {
		throw index_error("void partial_add");
	}
	for (int i = start; i < start + size; ++i) {
		v->at(i) += val;
	}
}

void* parallel_scan (void* threadarg) {
	ThreadArg* pTArg = (ThreadArg *) threadarg;

	// start the partial prescan on threads own range //
	pTArg->pSum()->at(pTArg->tid()) = partial_scan(pTArg->pArray(), pTArg->offset(), pTArg->jobSize());

	// wait for every thread to finish its own prescan on its partial range on the array //
	pthread_barrier_wait(pTArg->pBar());
	
	// the intermediate prescan. We do not use a tree structure here,
	// as the speedup due to the tree structure is negligible for a maximum of
	// 48 threads for large arrays.
	if (pTArg->tid() == 0) {
		partial_scan(pTArg->pSum(), 0, pTArg->pSum()->size());
	}
	
	// wait for main process to calculate the prescan values from the sums of the working areas.
	// See Belloch figure 1.6
	pthread_barrier_wait(pTArg->pBar());
	
	// add the prescan values to the own working area //
	partial_add(pTArg->pArray(), pTArg->offset(), pTArg->jobSize(), pTArg->pSum()->at(pTArg->tid()));
	
	// ensure that the array is finished when main thread leaves this function //
	pthread_barrier_wait(pTArg->pBar());
	
	// only main thead should not exit //
	if (pTArg->tid() != 0) {
		pthread_exit(NULL);
	}
}

vector<unsigned int> init_random_array (size_t s, size_t min, size_t max = 10000) {
	vector<unsigned int> v(s);
	default_random_engine gen(0); // random engine with seed 0 //
	uniform_int_distribution<unsigned int> dist(min, max);
	for (auto& e : v) {
		e = dist(gen); 
	}
	return v;
}
