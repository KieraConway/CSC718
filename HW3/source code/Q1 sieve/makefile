EXECS=sieve1 sieve2 sieve3
MPICC?=mpicc

all: ${EXECS} transfer

sieve1: sieve1.c
	@echo Building sieve1.c
	@${MPICC} -o sieve1 sieve1.c
	@echo Completed

sieve2: sieve2.c
	@echo Building sieve2.c
	@${MPICC} -o sieve2 sieve2.c
	@echo Completed

sieve3: sieve3.c
	@echo Building sieve3.c
	@${MPICC} -o sieve3 sieve3.c
	@echo Completed

transfer:
	@echo Transferring files to Washington, Lincoln, and Roosevelt
	@scp -rp $(CURDIR)/* washington:$(CURDIR) > /dev/null
	@scp -rp $(CURDIR)/* lincoln:$(CURDIR) > /dev/null
	@scp -rp $(CURDIR)/* roosevelt:$(CURDIR) > /dev/null
	@echo Tranfer Complete

clean:
	@echo Removing ${EXECS}
	@rm ${EXECS}
