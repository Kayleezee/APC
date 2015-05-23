/*****************************************************************
 * 
 * FILE         time_measurement.h
 * 
 * DESCRIPTION  This header provides a class, which can be used to
 *              perform elegant formulated wall clock program
 *              execution time measurements. It is possible to
 *              measure different times, which are accessible with
 *              keywords.
 *              
 * AUTHOR       Christoph Klein
 * 
 * LAST CHANGE  2015-05-23
 * 
 ****************************************************************/

#include <sys/time.h>
#include <string>
#include <map>

class TimeMeasurer {
	public:
		void start(std::string key) {
			struct timeval tim;
			gettimeofday(&tim, NULL);
			startTime[key] = tim.tv_sec + (tim.tv_usec / 1000000.0);
		}

		double stop(std::string key) {
			struct timeval tim;
			gettimeofday(&tim, NULL);
			elapsedTime[key] = (tim.tv_sec + (tim.tv_usec/1000000.0)) - startTime[key];
			return elapsedTime[key];
		}

	private:
		std::map<std::string, double> startTime;
		std::map<std::string, double> elapsedTime;
};
