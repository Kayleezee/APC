/******************************************************************
 *
 * FILENAME    - thread_routines.h
 *
 * DESCRIPTION - This file provides the thread routines for the
 *               parallel prefix sum.
 *
 * AUTHOR      - Naumann, Klein
 *
 * LAST CHANGE - 2015-05-30
 *
 ******************************************************************/ 

#ifndef THREAD_ROUTINES_H
#define THREAD_ROUTINES_H

#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <exception>

#include "thread_arg.h"

using namespace std;

class index_error : public exception {
	public:
		index_error (const char* name) {
			name_ += "Index Error in Function: ";
			name_ += name;
		} 

		const char* what() const throw() {
			return name_.c_str();
		}
	private:
		string name_;
};

/*
 *  DESCRIPTION - This function executes a prefix scan on a certain
 *                part of an std::vector.
 *  PARAMETER   - v: the pointer to the std::vector
 *              - start: start indize
 *              - size: number of fields to work on. This means if 
 *                start = 10 and size = 12 the function will change
 *                vector entries v[i], with i = 10..21.
 *  RETURN      - data_t: The total sum over all changed indizes
 */
template <typename data_t>
data_t partial_scan (vector<data_t>* v, size_t start, size_t size);

/*
 *  DESCRIPTION - This function takes an std::vector and a certain range
 *                like partial_scan. The function adds one value to
 *                every element in the range.
 *  PARAMETER   - v: pointer to the std::vector
 *              - start: analog to partial_scan
 *              - size: analog to partial_scan
 *              - val: the numerical value, which will be added to every element in the range
 */
template <typename data_t>
void partial_add (vector<data_t>* v, size_t start, size_t size, data_t val);

/*
 *  DESCRIPTION - This function is the pthread parallel function which executes
 *                the parallel prefix sum.
 *  PARAMETER   - void*: is a pthread convention. For more information look at class
 *                ThreadArg in file thread_arg.h.
 *  RETURN      - void*: pthread convention
 */
template <typename data_t>
void* parallel_scan (void* threadargs);

/*
 *  DESCRIPTION - takes a numeric data_t and a certain array size,
 *                fills a std::vector with uniform random numbers
 *                in the wanted range and gives the vector as
 *                a return value back.
 *  PARAMETER   - array_size: the size of the returned std::vector
 *              - rand_min: minimal generated random value
 *              - rand_max: maximal generated random value
 *  RETURN      - std::vector<data_t>: with uniform random numbers filled
 */
template <typename data_t>
vector<data_t> init_random_array (size_t array_size, data_t rand_min, data_t rand_max);



template <typename data_t>
data_t partial_scan (vector<data_t>* v, size_t start, size_t size) {
	if (size < 1 || start < 0 || start >= v->size()) {
		throw index_error("int partial_scan");
	}
	data_t lst = v->at(start);
	data_t llst;
	data_t sum = v->at(start + size - 1);
	v->at(start) = 0;
	for (int i = start+1; i < start + size; ++i) {
		llst = v->at(i);
		v->at(i) = v->at(i-1) + lst;
		lst = llst;
	}
	return sum + v->at(start + size - 1);
}

template <typename data_t>
void partial_add (vector<data_t>* v, size_t start, size_t size, data_t val) {
	if (size < 1 || start < 0 || start >= v->size()) {
		throw index_error("void partial_add");
	}
	for (int i = start; i < start + size; ++i) {
		v->at(i) += val;
	}
}

template <typename data_t>
void* parallel_scan (void* threadarg) {
	ThreadArg<data_t>* pTArg = (ThreadArg<data_t> *) threadarg;

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

template <typename data_t>
vector<data_t> init_random_array (size_t s, data_t min, data_t max = 100) {
	vector<data_t> v(s);
	default_random_engine gen(0); // random engine with seed 0 //
	uniform_int_distribution<data_t> dist(min, max);
	for (auto& e : v) {
		e = dist(gen); 
	}
	return v;
}

#endif
