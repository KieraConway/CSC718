EXECS=student_mpi
MPICC?=mpicc

all: ${EXECS} transfer

student_mpi: student_mpi.c
	@echo Building student_mpi.c
	@${MPICC} -o student_mpi student_mpi.c -lm
	@echo Completed

transfer:
	@echo Transferring files to Washington, Lincoln, and Roosevelt
	@scp -rp $(CURDIR)/* washington:$(CURDIR) > /dev/null
	@scp -rp $(CURDIR)/* lincoln:$(CURDIR) > /dev/null
	@scp -rp $(CURDIR)/* roosevelt:$(CURDIR) > /dev/null
	@echo Transfer Complete

clean:
	@echo Removing ${EXECS}
	@rm ${EXECS}
