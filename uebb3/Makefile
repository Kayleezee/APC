#*************************************************************************************************
#
# Heidelberg University - APC Exercise 03
#
# Group :       APC03
# Participant : Christoph Klein
#               Klaus Naumann
#
# File :        Makefile
#
# Purpose :     SHARED COUNTER (Parallel POSIX Threads Version)
#
# Last Change : 11. Mai 2015
#
#*************************************************************************************************

CPP=g++
CFLAGS = -pthread

mtlb: ./src/shared_counter_pthreads.cpp
	$(CPP) $(CFLAGS)	src/shared_counter_pthreads.cpp -o sc

.PHONY: clean
clean :
	rm -f sc
