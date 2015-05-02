/*************************************************************************************************
*
* Heidelberg University - APC Exercise 02
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        mtlb_pthreads.c
*
* Purpose :     MULTI-THREAD LOAD BANDWIDTH (Parallel POSIX Threads Version)
*
* Last Change : 28. April 2015
*
*************************************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>

#define KB 1024
#define MB 1024 * 1024
#define GB 1024 * 1024 * 1024

//: define global variables
int iNumThreads;
int iSize;
double *dArrayA;
double *dArrayB;

/*************************************************************************************************
* HANDLING HEADER
*************************************************************************************************/
void vPrintUsage() {
    printf("\nUsage:\n\n./mtlb\n"
            "\n\t<S: size of array>"
            "\n\t<T: number of threads>");
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
* ARRAY HANDLING HEADER
*************************************************************************************************/
double *dAllocArrayA() {
    int i;
    double *dValues;

    //: allocate values
    dValues = (double *) malloc (iSize * sizeof(double));

    for(i = 0; i < iSize; i++) {
        dValues[i] = i;
    }

    return dValues;
}

double *dAllocArrayB() {
    int i;
    double *dValues;

    //: allocate values
    dValues = (double *) malloc (iSize * sizeof(double));

    for(i = 0; i < iSize; i++) {
        dValues[i] = 0.0;
    }

    return dValues;
}

void vPrintArray(double *dArray) {
    int i;

    for(i = 0; i < 8; i++) {
        printf("%lf ",  dArray[i]);
    }
}

void vCopyArray(int iMyId) {
    int volatile i;

    //: compute bounds for thread
    int iStart = iMyId * iSize / iNumThreads;
    int iEnd = (iMyId + 1) * (iSize / iNumThreads);

    //: copy elements
    for (i = iStart; i <= iEnd; i++) {
        dArrayB[i] = dArrayA[i];
    }
}

void *vWorker(void *arg) {
    int iMyId = *((int *) arg);

    vCopyArray(iMyId);

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

    double dBandWidth = 0.0;

    pthread_t *threads;

    if (argc != 3) {
        vPrintUsage();
        return EXIT_FAILURE;
    }

    iSize         = atoi(argv[1]);
    iNumThreads   = atoi(argv[2]);

    dArrayA = dAllocArrayA();
    dArrayB = dAllocArrayB();

    //:
    threads = (pthread_t *) malloc(iNumThreads * sizeof(pthread_t));

    dStart = dStartMeasurement();

    // Create threads
    for (i = 0; i < iNumThreads; i++) {
        p = (int *) malloc(sizeof(int));
        *p = i;
        pthread_create(&threads[i], NULL, vWorker, (void *)(p));
    }

    for (i = 0; i < iNumThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    dTime = dStopMeasurement(dStart);

    printf("\n");
    printf("#==============================================================\n#\n");
    printf("# MULTI-THREAD LOAD BANDWIDTH (Parallel POSIX Threads Version) \n#\n");
    printf("#--------------------------------------------------------------\n#\n");
    printf("# Time needed:             %.5lf s \n", dTime);
    printf("# Bytes  per second:       %.5lf \n", 1. * iSize * sizeof(double) / dTime);
    printf("# kBytes per second:       %.5lf \n", 1. * iSize * sizeof(double) / (dTime * KB));
    printf("# MBytes per second:       %.5lf \n", 1. * iSize * sizeof(double) / (dTime * MB));
    printf("# GBytes per second:       %.5lf \n", 1. * iSize * sizeof(double) / (dTime * GB));
    printf("#\n#==============================================================\n\n");

    return EXIT_SUCCESS;
}
