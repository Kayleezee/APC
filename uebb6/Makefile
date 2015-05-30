##################################################################
#
## FILENAME    - Makefile
#
## DESCRIPTION - Advanced Parallel Computing, SS15 HD, Sheet 6
#
## AUTHOR      - Naumann, Klein
#
## LAST CHANGE - 2015-05-30
#
##################################################################

CPP = g++
CFLAGS = -I./inc/ -pthread -std=c++0x 
DEPS = ./inc/time_measurement.h src/thread_handler.h src/thread_arg.h src/thread_routines.h
OBJ = ./src/main.o ./src/thread_routines.o
DOCU = docu_6

.PHONY: build
build: $(OBJ)
	$(CPP) $(CFLAGS) -o ./bin/parallel_prefix_sum ./src/*.o

./src/%.o: ./src/%.cc $(DEPS)
	$(CPP) -I./inc -c -o $@ $< $(CFLAGS) -I./inc
	
.PHONY: clean
clean :
	rm -f ./src/*.o ./bin/barrier_analysis
	rm -f ./$(DOCU).pdf ./$(DOCU).aux ./$(DOCU).log
	
.PHONY: docu
docu: ./doc/$(DOCU).tex
	pdflatex ./doc/$(DOCU).tex
