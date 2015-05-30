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

void printHelp(char* progName);

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

	// skip sequential calculation //
	bool skipSeq = false;
	skipSeq = chCommandLineGetBool("sq", argc, argv);

	// print help function //
	bool needHelp = false;
	needHelp = chCommandLineGetBool("h", argc, argv);
	if (needHelp) {printHelp(argv[0]); return 0;}
	
/*******************************
 * INITIALIZE THREAD ARGUMENTS *
 *******************************/
 	typedef unsigned long int data_t; // here you can choose the datatype for the array
 	auto vArray = init_random_array<data_t>(asize, 0, 20); 	
	vector<data_t> vArray_copy; // for testing correctness of the result
	if (!skipSeq) {
		 vArray_copy = vArray; 
	}
	auto vSum   = vector<data_t>(tcount, 0);
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, tcount);
	vector<ThreadArg<data_t> > vThreadArg;
	for (int i = 0; i < tcount; ++i) {
	    ThreadArg<data_t> ta(i, tcount, &vArray, &vSum, &barrier);
		vThreadArg.push_back(ta);
	}

/*********************************
 * CREATE THREADS AND START JOBS *
 *********************************/
	void* (*p_parallel_scan)(void *);
	p_parallel_scan = &parallel_scan<data_t>;
 	ThreadHandler<ThreadArg<data_t> > thandler(tcount, p_parallel_scan, vThreadArg);
	TimeMeasurer tm; // for time measurements
	tm.start("parallel");
	thandler.doWork();
	tm.stop("parallel");

/**********************************
 * TESTING RESULT FOR CORRECTNESS *
 **********************************/
 	if (!skipSeq) {
		tm.start("sequential");
		partial_scan(&vArray_copy, 0, asize);	
		tm.stop("sequential");
		for (int i = 0; i < asize; ++i) {
			if (vArray_copy[i] != vArray[i]) {
				cout << "WRONG RESULT" << endl;
			}
		}
		for (int i = 1; i < asize; ++i) {
			if (vArray[i] < vArray[i-1]) {
				cout << "WRONG RESULT" << endl;
				cout << "\tvArray[" << i << "] = " << vArray[i] << ", vArray[" 
				     << i-1 << "] = " << vArray[i-1] << endl;
			}
		}
		if (vArray[0] != 0) {
			cout << "WRONG RESULT" << endl;
			cout << "\tvArray[0] = " << vArray[0] << endl;
		}
	}

/******************
 * PROGRAM REPORT *
 ******************/
	cout << endl << "Program Report:"
	     << endl << "\tarray_size " << asize
		 << endl << "\tthread_count " << tcount
		 << endl << "\tprefix_sum_time_parallel   " << tm.get_time("parallel") << " s"
		 << endl << "\tprefix_sum_time_sequential " << tm.get_time("sequential") << " s"
		 << endl << endl;

	// Finalize //		 
	pthread_exit(NULL);
}

void printHelp(char* progName) {
	cout << endl << "Usage:"
	     << endl << progName << " -s <array size> -t <thread count>" << endl
		 << endl << "Main Options:"
		 << endl << "  -h   Print this help message"
		 << endl << "  -sq  Skip sequential calculation of prefix sum (no correctness tests)"
		 << endl << endl;
}
