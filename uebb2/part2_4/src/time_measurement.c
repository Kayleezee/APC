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
#include <sys/time.h>
#include "time_measurement.h"

/* Start time-measurement with gettimeofday() */
double dStartMeasurement(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec+(t.tv_usec/1000000.0);
}

/* Stop time-measurement with gettimeofday() */
double dStopMeasurement(double dStartTime)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (t.tv_sec+(t.tv_usec/1000000.0)) - dStartTime;
}
