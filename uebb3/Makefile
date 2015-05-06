#*************************************************************************************************
#
# Heidelberg University - APC Exercise 02
#
# Group :       APC03
# Participant : Christoph Klein
#               Klaus Naumann
#
# File :        mtlb_pthreads.c
#
# Purpose :     MULTI-THREAD LOAD BANDWIDTH (Parallel POSIX Threads Version)
#
# Last Change : 28. April 2015
#
#*************************************************************************************************

CPP=g++
CFLAGS = -pthread

mtlb: ./src/mtlb_pthreads.cpp
	$(CPP) $(CFLAGS)	src/mtlb_pthreads.cpp -o mtlb

.PHONY: clean
clean :
	rm -f mtlb
