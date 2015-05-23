/*************************************************************************************************
*
* Heidelberg University - APC Exercise 04
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        main.cc
*
* Last Change : 23. Mai 2015
*
*************************************************************************************************/

#include "time_measurement.h"

#include <iostream>
#include <iomanip>

int main () {
	TimeMeasurer tm;	
	tm.start("balance");
	for (int i = 0; i < 10000000; ++i) {}
	double elapsed = tm.stop("balance");
	std::cout << "elapsed time = " << std::setprecision(10) << elapsed << " s" << std::endl;
	return 0;
}
