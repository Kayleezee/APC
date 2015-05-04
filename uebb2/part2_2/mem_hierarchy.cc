//#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>

using namespace std;

#define ARRAY_MIN (1024) /* 1/4 smallest cache */
#define ARRAY_MAX (4096*4096) /* 1/4 largest cache */

int x[ARRAY_MAX]; /* array going to stride through */

long get_seconds() { /* routine to read time in seconds */
return time ( NULL ); // time in seconds
}

int main(int argc, char* argv[])
{
	fstream fs; 
	fs.open("data.csv", fstream::out | fstream::trunc);
	fs << "csize" << "\t\t" << "stride" << "\t\t" << "loadtime[ns]" << endl;

	int nextstep, i, index, stride;
	int csize;
	size_t steps, tsteps;
	double loadtime, lastsec, sec0, sec1, sec; /* timing variables */
		
	/* Main loop for each configuration */
	for (csize=ARRAY_MIN; csize <= ARRAY_MAX; csize *= 2) {
		cout << "Cache Size = " << csize << endl;

		for (stride=1; stride <= csize/2 ; stride *= 2) {
		
			// FILLING THE ARRAY
			cout << "\tstride = " << stride << endl;	
			for (index=0; index < csize; index=index+stride)
				x[index] = index + stride;
			x[index-stride] = 0;
		
			steps = 0;
			nextstep = 0;

			lastsec = get_seconds();
			do sec0 = get_seconds(); while (sec0 == lastsec);
			
			// ITERATE OVER ARRAY	
			sec0 = get_seconds(); /* start timer */
			do { 
				for (i = stride; i != 0; --i) {
					nextstep = 0;
					do nextstep = x[nextstep];
					while (nextstep != 0);
				}
				++steps;
				sec1 = get_seconds();
			} while ((sec1 - sec0) < 20); /* collect 20 seconds */
			sec = sec1 - sec0;

			// DETERMINE LOOP OVERHEAD
			tsteps = 0;
			sec0 = get_seconds(); 
			do {
				for (i = stride; i != 0; --i) {
					index = 0;
					do index = index + stride;
					while (index < csize);
				}
				++tsteps;
				sec1 = get_seconds();
			} while (tsteps < steps);
			
			// subtract loop overhead
			sec = sec - (sec1 - sec0);

			// latency in nano seconds for accessing one array element
			loadtime = (sec*1e9)/(steps*csize);
			cout << "\tloadtime = " << loadtime << " ns" << endl;
			fs << csize << "\t\t" << stride << "\t\t" << loadtime << endl;

		}; /* end of inner for loop */

		}; /* end of outer for loop */

	fs.close();
	return 0;
}

