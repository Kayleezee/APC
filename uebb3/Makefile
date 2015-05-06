#*************************************************************************************************
#
# Heidelberg University - APC Exercise 03
#
# Group :       APC03
# Participant : Christoph Klein
#               Klaus Naumann
#
# File :        shared_counter_pthreads.c
#
# Purpose :     MULTI-THREAD LOAD BANDWIDTH (Parallel POSIX Threads Version)
#
# Last Change : 28. April 2015
#
#*************************************************************************************************

CPP=g++
CFLAGS = -pthread

mtlb: ./src/shared_counter_pthreads.cpp
	$(CPP) $(CFLAGS)	src/shared_counter_pthreads.cpp -o sc

.PHONY: clean
clean :
	rm -f sc
