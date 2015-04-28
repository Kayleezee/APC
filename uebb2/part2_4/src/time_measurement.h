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
#ifndef TIME_MEASUREMENT_H
#define TIME_MEASUREMENT_H

/*************************************************************************************************
* Function description:		Starts a time-measurement, based on the gettimeofday() functions
*               		    It has a resolution up to one microsecond.
* Parameter:			    void
*
* Return:      			    double: elapsed seconds this day (is the parameter for dstopMesGTOD())
*************************************************************************************************/
double dStartMeasurement(void);

/*************************************************************************************************
* Function description:		Stops the time-measurement, based on the gettimeofday() functions.
*               		    It has a resolution up to one microsecond.
* Parameter:			    double: return-value of dstartMesGTOD()
*
* Return:			        double: elapsed seconds since dstartMesGTOD()
*************************************************************************************************/
double dStopMeasurement(double);

#endif
