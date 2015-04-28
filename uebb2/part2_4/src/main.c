/*************************************************************************************************
*
* Heidelberg University - APC Exercise 02
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        time_measurement.c
*
* Purpose :     MULTI-THREAD LOAD BANDWIDTH (Parallel POSIX Threads Version)
*
* Last Change : 28. April 2015
*
*************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include "time_measurement.h"

int iSize;                // INTEGER: array-size variable
int iNumThreads;          // INTEGER: number of threads

double * dArrayA;
double * dArrayB;

void print_usage() {
    printf("\nUsage:\n\n./mtlb\n"
            "\n\t<S: size of array>"
            "\n\t<T: number of threads>");
}

void vCopyArray(int *Id) {

}

int main(int argc, char** argv){
    /* Integer declaration */
    int i;                    // INTEGER: loop variable
    int j;                    // INTEGER: loop variable
    int k;                    // INTEGER: loop variable

    /* Double declaration */
    double dStart     = 0.0;
    double dTimePar   = 0.0;

    /* Pointer declaration */
    pthread_t * pThreads;

    /* check arguments */
    if(argc!=3)
    {
        print_usage();
        return 1;
    }

    /* Get command line arguments */
    iSize         = atoi(argv[1]);
    iNumThreads   = atoi(argv[2]);

    /* Memory allocation pointer to threads: tPointer(iNumThreads) */
    pThreads = (pthread_t *) malloc(iNumThreads*sizeof(pthread_t));

    /* Memory allocation pointer to array: tArray(iSize) */
    dArrayA = malloc(iSize * sizeof(double *));
    dArrayB = malloc(iSize * sizeof(double *));

    /* begin time measurement */
    dStart = dStartMeasurement();

    /* Start */
    /*
    for(i = 0; i < iNumThreads; i++)
    {
        if(pthread_create(&pThreads[i], NULL, (void *)function, (void *)(intptr_t)i) != 0)
        {
            printf("\nExiting program ...\nThread creation failed!");
            free(pThreads);
            return 1;
        }
    }

    for(j = 0; j < iNumThreads; j++)
        pthread_join(pThreads[j], NULL);
    }
    */

    /* stop time measurement */
    dTimePar = dStopMeasurement(dStart);


    printf("\n#########################################################\n#\n#\n#");
    printf("\n#\t Bandwidth: \n#");
    printf("\n#\t Number of Threads : %lf \n#", iNumThreads);
    printf("\n#\t Bandwidth (GB / s): %lf \n#", iSize);
    printf("\n#");
    printf("\n#\n#\n#########################################################\n\n\n");

    return 0;
}
