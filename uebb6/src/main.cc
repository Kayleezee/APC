/******************************************************************
 *
 * FILENAME    - main.cc
 *
 * DESCRIPTION - Advanced Parallel Computing, SS15 HD, Sheet 6
 *               Parallel Prefix Sum
 *
 * AUTHOR      - Naumann, Klein
 *
 * LAST CHANGE - 2015-05-30
 *
 ******************************************************************/ 

#include <iostream>
#include <vector>
#include <pthread.h>

#include "time_measurement.h"
#include "chCommandLine.h"
#include "thread_handler.h"
#include "thread_arg.h"
#include "thread_routines.h"

static const int DEFAULT_THREAD_COUNT = 4;
static const int DEFAULT_ARRAY_SIZE = 1000;

using namespace std;

void* job_f (void* arg) {
	cout << "it is me thread number " << *(int *) arg << endl;
}

int main (int argc, char** argv) {
	
/******************************
 * GETTING COMMAND LINE INPUT *	
 ******************************/
 	// thread count //
 	int tcount = -1;
	chCommandLineGet<int>(&tcount, "t", argc, argv);
	tcount = (tcount == -1) ? DEFAULT_THREAD_COUNT : tcount;

	// getting array size //
	int asize = -1;
	chCommandLineGet<int>(&asize, "s", argc, argv);
	asize = (asize == -1) ? DEFAULT_ARRAY_SIZE : asize;

/*******************************
 * INITIALIZE THREAD ARGUMENTS *
 *******************************/
 	auto vArray = init_random_array(asize, 0, 100000); 	
	auto vSum   = vector<unsigned int>(tcount, 0);
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, tcount);
	vector<ThreadArg> vThreadArg;
	for (int i = 0; i < tcount; ++i) {
	    ThreadArg ta(i, tcount, &vArray, &vSum, &barrier);
		vThreadArg.push_back(ta);
	}

	void* (*f)(void *);
	f = &job_f;
	int* pp = new int[4];
	for (int i = 0; i < 4; ++i) {
		pp[i] = i;
	}
//	ThreadHandler<int, int*> th(4, f, pp);
//	th.doWork();
	cout << "Try to change size of thread arg" << endl;
	vector<unsigned int> v2(8);
	vector<unsigned int> v3(4);
	pthread_exit(NULL);
}
