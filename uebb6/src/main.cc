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

#include "time_measurement.h"
#include "chCommandLine.h"
#include "thread_handler.h"

using namespace std;

static const int DEFAULT_THREAD_COUNT = 4;
static const int DEFAULT_ARRAY_SIZE = 1000;

void* job_f (void* arg) {

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
	
	void* (*f)(void *);
	f = &job_f;
	int* pp = new int[10];
	ThreadHandler<int, int*>(10, f, pp);

	return 0;
}
