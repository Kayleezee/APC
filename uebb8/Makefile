##################################################################
#
## FILENAME    - Makefile
#
## DESCRIPTION - Advanced Parallel Computing, SS15 HD, Sheet 8
#
## AUTHOR      - Naumann, Klein
#
## LAST CHANGE - 2015-06-14
#
##################################################################

CPP = g++
#CFLAGS = -I./inc/ -pthread -std=c++0x -O3 
#DEPS = ./inc/time_measurement.h inc/thread_handler.h src/thread_arg.h src/thread_routines.h
#OBJ = ./src/main.o
DOCU = docu_8

.PHONY: build
build: $(OBJ)
#	$(CPP) $(CFLAGS) -o ./bin/parallel_prefix_sum ./src/*.o

#./src/%.o: ./src/%.cc $(DEPS)
	$(CPP) -I./inc -c -o $@ $< $(CFLAGS) -I./inc
	
.PHONY: clean
clean :
#	rm -f ./src/*.o ./bin/parallel_prefix_sum
	rm -f ./$(DOCU).pdf ./$(DOCU).aux ./$(DOCU).log
	
.PHONY: rebuild
rebuild:
	make clean
	make

.PHONY: docu
docu: ./doc/$(DOCU).tex
	pdflatex ./doc/$(DOCU).tex
