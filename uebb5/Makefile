#########################################
#
# Heidelberg University - APC Exercise 04
#
# Group :       APC03
# Participant : Christoph Klein
#               Klaus Naumann
#
# File :        Makefile
#
# Last Change : 25. Mai 2015
#
#########################################

CPP = g++
CFLAGS = -I./inc/ -pthread 
DEPS = ./inc/time_measurement.h 
OBJ = ./src/main.o ./src/barrier.o
DOCU = docu_5

.PHONY: build
build: $(OBJ)
	$(CPP) $(CFLAGS) -o ./bin/barrier_analysis ./src/*.o

./src/%.o: ./src/%.cc $(DEPS)
	$(CPP) -I./inc -c -o $@ $< $(CFLAGS) -I./inc
	
.PHONY: clean
clean :
	rm -f ./src/*.o ./bin/barrier_analysis
	rm -f ./$(DOCU).pdf ./$(DOCU).aux ./$(DOCU).log
	
.PHONY: docu
docu: ./doc/$(DOCU).tex
	pdflatex ./doc/$(DOCU).tex
