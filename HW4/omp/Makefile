#
# Makefile for pX_omp Programs
#
# "make program" to make one program
# "make" or "make all" to make all executables
# "make clean" to remove executables
#

OMPC        = gcc
CFLAGS      = -O3 -fopenmp   ### -Wall -pedantic

OMPCC       = g++
CCFLAGS     = -O3 -fopenmp   ### -Wall -pedantic 

LFLAGS      = -lm 

ALL =   student_omp

.PHONY:  all
all:  $(ALL)

%: %.c 
	$(OMPC) $(CFLAGS) $< -o $@ $(LFLAGS)

.PHONY:  clean
clean:
	-rm $(ALL)

