/*************************************************************************************************
*
* Heidelberg University - APC Exercise 04
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        shared_counter_pthreads.cpp
*
* Purpose :     SHARED COUNTER (Parallel POSIX Threads Version)
*
* Last Change : 17. Mai 2015
*
*************************************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#define DEBUG false

typedef struct mcs_node sMcsNode;

typedef struct mcs_node {
	sMcsNode *volatile next;
	volatile int iIsLocked;
} *sMcsLock;

static sMcsNode node;
static sMcsLock lock;

//: define global variables
int iMode;
int iNumThreads;
int iC;
int myLock = 0;
std::string modes[4] = {"MUTEX", "ATOMIC", "LOCK_RMW", "MCS"};
volatile int iCounter = 0;

//: define shared barrier
pthread_barrier_t barrier;

//: define shared mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*************************************************************************************************
* HANDLING HEADER
*************************************************************************************************/
void vPrintUsage() {
    printf("\nUsage:\n\n./mtlb\n"
            "\n\t<C: number of increments>"
            "\n\t<T: number of threads>"
            "\n\t<M: mode [0: MUTEX | 1: ATOMIC | 2: LOCK_RMW | 3: MCS]\n");
}

/*************************************************************************************************
* TIME MEASUREMENT HEADER
*************************************************************************************************/
//: Start time-measurement with gettimeofday()
double dStartMeasurement(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec+(t.tv_usec/1000000.0);
}

//: Stop time-measurement with gettimeofday()
double dStopMeasurement(double dStartTime)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (t.tv_sec+(t.tv_usec/1000000.0)) - dStartTime;
}

/*************************************************************************************************
* MCS_LOCK HEADER
*************************************************************************************************/
//: MCS Lock
void vMcsLock(sMcsLock *lock, sMcsNode *node) {
	sMcsNode *pred;
	node->next = NULL;

	pred = __sync_lock_test_and_set(lock, node);

	if (pred == NULL) {
            return;
	}
    node->iIsLocked = 1;
    pred->next = node;
    while (node->iIsLocked == 1) {};

}

//: MCS Unlock
void vMcsUnlock(sMcsLock *lock, sMcsNode *node) {
	if (node->next == NULL) {
        if (__sync_bool_compare_and_swap(lock, node, NULL)) return;
		while (node->next == NULL) {};
	}
	node->next->iIsLocked = 0;
}


/*************************************************************************************************
* THREAD HEADER
*************************************************************************************************/
void lock_rmw(int id, int* lock) {
	// One Thread checks the value of *lock permanently in the
	//  while loop. If *lock == 1 the loop will be continued
	//  and *lock = 1 will be done. Else *lock == 0 the loop
	//  will be stopped, but *lock = 1 will be written
	//  before leaving the function.
	if (DEBUG) printf("Thread%d: entered lock_rmw with *lock = %d\n", id, *lock);
	while (__sync_lock_test_and_set(lock, 1)) {}
}

void unlock_rmw(int* lock) {
	if (!lock) {
		if (DEBUG) printf("ERROR: Thread tried to realease not locked mutex.\n");
	}
	__sync_lock_release(lock);

}

void vIncCounter(int iMyId) {

	if (DEBUG) printf("Thread%d: entered vIncCounter\n",iMyId);
    pthread_barrier_wait(&barrier);
    if (iMode == 0) {
        //: increment counter MUTEX
        for (int i = 0; i < (iC/iNumThreads); ++i) {
            pthread_mutex_lock(&mutex);
            ++iCounter;
            pthread_mutex_unlock(&mutex);
        }
    }
    else if (iMode == 1) {
        //: increment counter ATOMIC
        for (int i = 0; i < (iC/iNumThreads); ++i) {
            __sync_add_and_fetch(&iCounter, 1);
        }
    }
    else if (iMode == 2) {
		//: increment counter LOCK_RMW
		for (int i = 0; i < (iC/iNumThreads); ++i) {
			if (DEBUG) printf("Thread%d: try to lock mutex\n",iMyId);
			lock_rmw(iMyId, &myLock);
			++iCounter;
			unlock_rmw(&myLock);
		}
    }
    else if (iMode == 3) {
        for (int i = 0; i < (iC/iNumThreads); ++i) {
            //printf("Thread%d: try to lock MCS\n", iMyId);
            vMcsLock(&lock, &node);
            ++iCounter;
            vMcsUnlock(&lock, &node);
            //printf("Thread%d: iCounter: %d\n", iMyId, iCounter);
        }
    }
}

void *vWorker(void *arg) {
    int iMyId = *((int *) arg);

    vIncCounter(iMyId);

    return NULL;
}

/*************************************************************************************************
* MAIN
*************************************************************************************************/
int main(int argc, char *argv[]) {
    int i;
    int *p;

    double dStart = 0.0;
    double dTime = 0.0;

    //: pthreads init
    pthread_t *threads;

    if (argc != 4) {
        vPrintUsage();
        return EXIT_FAILURE;
    }

    iC          = atoi(argv[1]);
    iNumThreads = atoi(argv[2]);
    iMode       = atoi(argv[3]);

    if (iMode > 3) {
        vPrintUsage();
        return EXIT_FAILURE;
    }

    //: barrier init
    if(pthread_barrier_init(&barrier, NULL, iNumThreads)) {
        printf("\nERROR: Could not initialize barrier!");
        return EXIT_FAILURE;
    }

    //: mutex init
    if(pthread_mutex_init(&mutex, NULL)) {
        printf("\nERROR: Could not initialize mutex!");
        return EXIT_FAILURE;
    }

    threads = (pthread_t *) malloc(iNumThreads * sizeof(pthread_t));

    dStart = dStartMeasurement();

    // Create threads
    for (i = 0; i < iNumThreads; i++) {
        p = (int *) malloc(sizeof(int));
        *p = i;
        if(pthread_create(&threads[i], NULL, vWorker, (void *)(p))) {
            printf("\nERROR: Could not create thread");
        }
    }

    for (i = 0; i < iNumThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    dTime = dStopMeasurement(dStart);

    pthread_mutex_destroy(&mutex);

    printf("\n#==============================================================\n#");
    printf("\n# SHARED COUNTER (Parallel POSIX Threads Version) \n#");
    printf("\n#--------------------------------------------------------------\n#");
    printf("\n# Mode               %s", modes[iMode].c_str());
    printf("\n# thread_count       %d", iNumThreads);
    printf("\n# iCounter           %d", iCounter);
    printf("\n# time_total         %.5lf s", dTime);
    printf("\n# updates_per_second %.2lf", ((double) iCounter)/dTime);
    printf("\n# time_per_increment %.8lf s", dTime/iCounter);
    printf("\n#\n#==============================================================\n\n");

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
