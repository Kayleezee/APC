/*************************************************************************************************
*
* Heidelberg University - APC Exercise 03
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        shared_counter_pthreads.cpp
*
* Purpose :     SHARED COUNTER (Parallel POSIX Threads Version)
*
* Last Change : 05. Mai 2015
*
*************************************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>

//: define global variables
int iMode;
int iNumThreads;
int iC;
volatile int iCounter = 0;

//: define shared barrier
pthread_barrier_t barrier;

//: define shared rw_lock


//: define shared mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*************************************************************************************************
* HANDLING HEADER
*************************************************************************************************/
void vPrintUsage() {
    printf("\nUsage:\n\n./mtlb\n"
            "\n\t<C: number of increments>"
            "\n\t<T: number of threads>"
            "\n\t<M: mode [0: MUTEX | 1: ATOMIC | 2: LOCK_RMW]\n");
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
* THREAD HEADER
*************************************************************************************************/
void vIncCounter(int iMyId) {

    pthread_barrier_wait(&barrier);
    if (iMode == 0) {
        //: increment counter MUTEX
        for (int i = 0; i < (iC/iNumThreads); i++) {
            pthread_mutex_lock(&mutex);
            iCounter += 1;
            pthread_mutex_unlock(&mutex);
        }
    }
    else if (iMode == 1) {
        //: increment counter ATOMIC
        for (int i = 0; i < (iC/iNumThreads); i++) {
            __sync_add_and_fetch(&iCounter, 1);
        }
    }
    else if (iMode == 2) {

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

    //: barrier init
    if(pthread_barrier_init(&barrier, NULL, iNumThreads)) {
        printf("\nERROR: Could not initialize barrier!");
        return EXIT_FAILURE;
    }

    //: mutex init
    if(pthread_mutex_init(&mutex, NULL)) {
        printf("\nError: Could not initialize mutex!");
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

    pthread_mutex_destroy(&mutex);

    dTime = dStopMeasurement(dStart);

    printf("\niCounter: %d", iCounter);
    printf("\n");
    pthread_exit(NULL);

    return EXIT_SUCCESS;
}

