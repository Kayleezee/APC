/******************************************************************
 *
 * FILENAME    - thread_arg.h
 *
 * DESCRIPTION - provides thread argument data structure
 *
 * AUTHOR      - Naumann, Klein
 *
 * LAST CHANGE - 2015-05-30
 *
 ******************************************************************/ 

#ifndef THREAD_ARG_H
#define THREAD_ARG_H

#include <vector>
#include <exception>
#include <sstream> // int to string conversion
#include <string>
#include <iostream>

using namespace std;

/*
 *  DESCRIPTION - This error will be thrown if the thread count and array size
 *                do not satisfy the algorithm's condition
 */
class environment_error : public exception {
	public:
		environment_error(size_t lenArray, size_t tcount) : lenArray_(lenArray), tcount_(tcount) {
			stringstream ss;
			ss << "Object of Class ThreadArg must get an data vector 'v' and a thread count 't' with"
			   << endl << "\t\t v.size() % t == 0"
			   << endl << "But instead it got"
			   << endl << "\t\tv.size() = " << lenArray_ << " and t = " << tcount_ << endl;
			message = ss.str();	
		}

		const char* what() const throw() {
			return message.c_str();
		}
		
		~environment_error() throw() {}

	private:
		size_t lenArray_;
		size_t tcount_; 
		string message;
};

/*
 *  DESCRIPTION - Objects of this class are given to each pthread on their
 *                creation.
 *  PARAMETER   - data_t: this is the numeric data type of the array, which
 *                is used for parallel prefix summation.
 */
template <typename data_t>
class ThreadArg {
	public:
		/*
		 *  DESCRIPTION - Constructor
		 *  PARAMETER   - thread_id
		 *              - thread_count: total amount of working threads
		 *              - pArray: pointer to the array, which is used to
		 *                calculate the parallel prefix sum.
		 *              - pSum: pointer to the array, where every thread
		 *                saves its the total sum of the array part it is
		 *                working on.
		 *              - pBar: pthread barrier for synchronization
		 */
		ThreadArg(size_t thread_id, size_t thread_count,
		          vector<data_t>* pArray, vector<data_t>* pSum, pthread_barrier_t* pBar) :
		          tid_(thread_id),
				  tcount_(thread_count),
				  pArray_(pArray),
				  pSum_(pSum),
				  pBar_(pBar) {

			int size = pArray->size();	
			if (size % thread_count != 0 || size == 0 || thread_count == 0) {
				throw environment_error(size, thread_count);
			}
			jobSize_ = size / thread_count;
			offset_  = jobSize_ * thread_id;
		}
		
		// GET FUNCTIONS //
		size_t tid() const {return tid_;}	
		size_t offset() const {return offset_;}
		size_t jobSize() const {return jobSize_;}
		pthread_barrier_t* pBar() const {return pBar_;}
		vector<data_t>* pArray() const {return pArray_;}
		vector<data_t>* pSum() const {return pSum_;} 

	private:
		size_t tid_;
		size_t tcount_;
		size_t offset_;
		size_t jobSize_;
		vector<data_t>* pArray_; 
		vector<data_t>* pSum_;	
		pthread_barrier_t* pBar_;
};
#endif
