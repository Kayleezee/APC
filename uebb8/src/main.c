/*************************************************************************************************
*
* Heidelberg University - APC Exercise 07
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        rbtreemain.c
*
* Purpose :     RED-BLACK TREE
*
* Last Change : 13. Juni 2015
*
*************************************************************************************************/

#include "rbtree.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define INIT_TREE_ELEMENTS  10000000

pthread_rwlock_t lock;
rbtree RBTree;

int iNumThreads;
int iOperations;
int iNumSearch;
int iNumInsert;
int iKeyInsert;
int iValueInsert;

/*************************************************************************************************
* USAGE HEADER
*************************************************************************************************/
void vPrintUsage() {
    printf("\nUsage:\n\n./redblacktree\n"
            "\n\t<T: number of threads>"
            "\n\t<O: number of operations>"
            "\n\t<I: amount of insert operations in percent>"
            "\n");
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
* PARALLEL ADD / SEARCH FUNCTIONS
*************************************************************************************************/
void vOperationStream_parallel(int *iId) {
    int i = 0;
    int j;
    int iKey;
    int iAmountOpsPerThread;
    int iThreadId = (intptr_t) iId;

    iAmountOpsPerThread = ((iThreadId + 1)*iOperations)/iNumThreads - (iThreadId*iOperations)/iNumThreads;

    while(i < iAmountOpsPerThread) {
        for(j = 0; j < iNumInsert; j++) {
            iValueInsert = rand();
            iKeyInsert = rand();
            rbtree_insert(RBTree, (void*)iKeyInsert, (void*)iValueInsert, int_compare);
            i++;
        }

        for(j = 0; j < iNumSearch; j++) {
            iKey = j;
            rbtree_lookup(RBTree, (void*)iKey, int_compare);
            i++;
        }
    }
}

int main(int argc, char **argv) {
    FILE *f;
    int i;

    int iOperation;
    int iThreads;
    int iSearchRatio;
    int iInsertRatio;

    int lNumThreads[8] = {2, 4, 8, 12, 16, 24, 32, 48};
    // experiment ratio 10/90
    int lSearchOperations[7] = {10000,  50000, 100000,  500000, 1000000,  2500000,  5000000};
    int lInsertOperations[7] = {90000, 450000, 900000, 4500000, 9000000, 22500000, 45000000};

    double dStart = 0.0;
    double dTime_parallel = 0.0;

    pthread_t * pThreads;

    // create red-black tree
    RBTree = rbtree_create();

    if(pthread_rwlock_init(&lock, NULL)) {
        printf("\nERROR: Could not initialize lock!");
        return EXIT_FAILURE;
    }

    // fill red-black tree with predefined amount of nodes
    for(i = 0; i < INIT_TREE_ELEMENTS; i++) {
        iValueInsert = rand();
        iKeyInsert = rand();
        rbtree_insert(RBTree, (void*)iKeyInsert, (void*)iValueInsert, int_compare);
    }

    f = fopen("experiment.dat", "w+");
    fprintf(f, "Operations Threads time[s] OpsPerSecond\n");

    for(iOperation = 0; iOperation < 7; iOperation++) {
        for(iThreads = 0; iThreads < 8; iThreads++) {
            iNumInsert = lInsertOperations[iOperation];
            iNumSearch = lSearchOperations[iOperation];

            iOperations = iNumSearch + iNumInsert;

            iNumThreads = lNumThreads[iThreads];

            pThreads = (pthread_t*) malloc(iNumThreads * sizeof(pthread_t));

            printf("\nStarting...");
            printf("\nOperations %d", iOperations);
            printf("\nSearch Operations %d", iNumSearch);
            printf("\nInsert Operations %d", iNumInsert);
            printf("\nThreads %d", iNumThreads);

            dStart = 0.0;
            dStart = dStartMeasurement();

            for(i = 0; i < iNumThreads; i++) {
                if(pthread_create(&pThreads[i], NULL, (void*)vOperationStream_parallel, (void*)(intptr_t)i) != 0) {
                    printf("\nExiting program ...\nThread creation failed!");
                    free(pThreads);
                    return EXIT_FAILURE;
                }
            }

            for (i = 0; i < iNumThreads; i++) {
                pthread_join(pThreads[i], NULL);
            }

            dTime_parallel = dStopMeasurement(dStart);

            if(pthread_rwlock_destroy(&lock)) {
                printf("\nERROR: Could not initialize lock!");
                return EXIT_FAILURE;
            }

            printf("\n# OpsPerSecond    %.0lf", iOperations/dTime_parallel);

            fprintf(f, "%d %d %.5lf %.0lf\n", iOperations, iNumThreads, (iOperations/dTime_parallel));
        }
        printf("\n");
        iThreads = 0;
    }

    fclose(f);

    return EXIT_SUCCESS;
}
