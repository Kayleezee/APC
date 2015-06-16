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

rbtree RBTree;

int iNumThreads;
int iOperations;
int iNumSearch;
int iNumInsert;
int iKey;
int iValue;

/*************************************************************************************************
* USAGE HEADER
*************************************************************************************************/
void vPrintUsage() {
    printf("\nUsage:\n\n./mtlb\n"
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
    int iAmountOpsPerThread;
    int iThreadId = (intptr_t) iId;

    iAmountOpsPerThread = iOperations / iNumThreads;

    while(i < iAmountOpsPerThread) {
        for(j = 0; j < iNumInsert; j++) {
            iValue = rand();
            iKey = rand();
            rbtree_insert_parallel(RBTree, (void*)iKey, (void*)iValue, int_compare);
            i++;
        }

        for(j = 0; j < iNumSearch; j++) {
            iKey = rand();
            rbtree_lookup_parallel(RBTree, (void*)iKey, int_compare);
            i++;
        }
    }
}

int main(int argc, char **argv) {
    int i;

    double dStart = 0.0;
    double dTime_parallel = 0.0;

    pthread_t * pThreads;

    if(argc == 4) {
        iNumThreads = atoi(argv[1]);
        iNumInsert = atoi(argv[2]);
        iNumSearch = atoi(argv[3]);
    }
    else {
        vPrintUsage();
        return EXIT_FAILURE;
    }

    iOperations = iNumInsert + iNumSearch;

    //printf("\nAmount of insert operations: %d", iNumInsert);
    //printf("\nAmount of search operations: %d", iNumSearch);

    // create red-black tree
    RBTree = rbtree_create();

    // fill red-black tree with predefined amount of nodes
    for(i = 0; i < INIT_TREE_ELEMENTS; i++) {
        iValue = rand();
        iKey = rand();
        rbtree_insert(RBTree, (void*)iKey, (void*)iValue, int_compare);
    }

    /* PARALLEL PART */
    pThreads = (pthread_t*) malloc(iNumThreads * sizeof(pthread_t));

    if(pthread_rwlock_init(&lock, NULL)) {
        printf("\nERROR: Could not initialize lock!");
        return EXIT_FAILURE;
    }

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


    printf("\n#==============================================================\n#");
    printf("\n# RED-BLACK TREE \n#");
    printf("\n#--------------------------------------------------------------\n#");
    printf("\n# Operations:            %d", iOperations);
    printf("\n# InsertOperations:      %d", iNumInsert);
    printf("\n# SearchOperations:      %d", iNumSearch);
    printf("\n# Time:                  %.5lf s", dTime_parallel);
    printf("\n# OpsPerSecond:          %.0lf", iOperations/dTime_parallel);
    printf("\n#==============================================================\n#");

    return EXIT_SUCCESS;
}
