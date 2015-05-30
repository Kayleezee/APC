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

int partial_scan (vector<unsigned int>& v, size_t start, size_t size);
void partial_add (vector<unsigned int>& v, size_t start, size_t size, unsigned int val);
void* parallel_scan (void* threadargs);

vector<unsigned int> init_random_array (size_t array_size, size_t rand_min, size_t rand_max);


#endif
