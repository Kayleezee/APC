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
* Last Change : 28. Juni 2015
*
*************************************************************************************************/

#include "rbtree.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>

#define MAX         10000000

/*************************************************************************************************
* HANDLING HEADER
*************************************************************************************************/
void vPrintUsage() {
    printf("\nUsage:\n\n./mtlb\n"
            "\n\t<O: number of operations>");
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
* HELPER HEADER
*************************************************************************************************/


int main(int argc, char **argv) {
    int iOps;           // number of search and add operations
    int iAmountSearch;  // number of search operations
    int iAmountAdd;     // number of add operations

    int iValue;
    int iKey;

    int i;
    int j;
    int k;

    double dStart = 0.0;
    double dTime = 0.0;

    if(argc == 2) {
        iOps = atoi(argv[1]);
    }
    else {
        vPrintUsage();
        return EXIT_FAILURE;
    }

    // define ratio 90/10
    iAmountSearch = iOps/10 * 9;
    iAmountAdd = iOps/10;

    // create red-black tree
    rbtree myTree = rbtree_create();

    // fill red-black tree with predefined amount of nodes
    for(i = 0; i < MAX; i++) {
        iValue = rand() % 1000000;
        iKey = i;
        rbtree_insert(myTree, (void*)iKey, (void*)iValue, int_compare);
    }

    dStart = dStartMeasurement();

    for(j = 0; j < iAmountAdd; j++) {
        // generate value and key
        iValue = rand() % 1000;
        iKey = rand() % MAX;

        // add to tree
        rbtree_insert(myTree, (void*)iKey, (void*)iValue, int_compare);
    }

    for(k = 0; k < iAmountSearch; k++) {
        // generate value and key
        iValue = rand() % 1000;
        iKey = rand() % MAX;

        // search in tree
        rbtree_lookup(myTree, (void*)iKey, int_compare);
    }

    dTime = dStopMeasurement(dStart);

    printf("\n#==============================================================\n#");
    printf("\n# RED-BLACK TREE (SEQUENTIAL) \n#");
    printf("\n# Operations:         %d", iOps);
    printf("\n# AddOperations:      %d", iAmountAdd);
    printf("\n# SearchOperations:   %d", iAmountSearch);
    printf("\n# Time total:         %.5lf s", dTime);
    printf("\n# Ops per sec.:       %.0lf", iOps/dTime);
    printf("\n#==============================================================\n#");

    return EXIT_SUCCESS;
}
