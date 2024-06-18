#
# Makefile for omp_pX Programs
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

ALL =   omp_p1 \
	omp_p2 \
	omp_p3 \
	omp_p4 \
 

.PHONY:  all
all:  $(ALL)

%: %.c 
	$(OMPC) $(CFLAGS) $< -o $@ $(LFLAGS)

.PHONY:  clean
clean:
	-rm $(ALL)

