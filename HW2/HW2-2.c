/* ------------------------------------------------------------------------
    HW2-2.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // //
// Program:
//
//	Parallel Program using openMPI
//
//	Finds the number of times that two consecutive
//	odd integers are both prime numbers from 2 to 1,000,000
//
// // // // // // //


/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <locale.h>
#include <mpi.h>			// Message Passing Interface
#include <math.h>

/** ----------------------------- Structures ---------------------------- **/
typedef struct procInfo proc;
typedef struct procInfo * pProc;

struct procInfo {
    int pid;				//process rank/pid
    int start;				//process start
    int end;				//process end
};

/** ----------------------------- Macros ---------------------------- **/
/*
 * Function Keys
 *      i : pid/rank
 *      n : max value
 *      p : total processes
 */
#define FIND_START(i,n,p)	floor((i*n)/(p))
#define FIND_END(i,n,p)     (floor((i+1)*(n)/(p)))-1

/** ----------------------------- Constants ----------------------------- **/
int N = 1000000;

/** ----------------------------- Prototypes ---------------------------- **/
bool isPrime(int num);
void FindRange(pProc p, int max, int numProcs);

/** ----------------------------- Functions ----------------------------- **/

/*****************************************************
 *
 *  Function:  main()
 *
 *  Parameters:
 *
 *      argc - main parameter for argument count
 *      argv - main parameter for argument values
 *
******************************************************/
int main(int argc, char *argv[]) {

    /*** Function Initialization ***/
    int curr_Val = 0;               //current prime value
    int prev_Val = 0;               //previous prime value
	
    int local_con = 0;				//total consecutive local count
    int total_con = 0;				//total consecutive 'global' count

    double elapsed_time;
    int numProcs, rank, nameLen;

    /* Parallelization Init */
    MPI_Init(&argc, &argv);			//Initializes the MPI execution environment
    MPI_Barrier (MPI_COMM_WORLD);	//Blocks caller until all processes in the communicator have called it

    elapsed_time = - MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);		//Determines the rank (PID) of the calling process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);	//Determines size of the processor group associated

    /* Initialize Process Information */
    proc process;

    process.start = process.end = 0;
    process.pid = rank;

    /* Determine Process Section */
    FindRange(&process, N, numProcs);

    /* Iterate through Section */
    for(int i = process.start; i <= process.end; i+=2){
		
        /* Determine if value is Prime */
        if (isPrime(i)) {


			curr_Val = i;                               	//save i as current

			/* Determine if Primes are Consecutive */
			if (i == 3 || curr_Val - prev_Val == 2) {
				local_con++;                                //increase consecutive counter
			}

            prev_Val = i;                                   //save i as previous
        }
    }

    MPI_Reduce (&local_con,
                &total_con,
                1,
                MPI_INT,
                MPI_SUM,
                0,
                MPI_COMM_WORLD);				//Reduces values on all processes within a group
    elapsed_time += MPI_Wtime();				//Calculate elapsed time

    MPI_Finalize();								//Terminates MPI execution environment

    if (!rank) {								//if process 0
	
        /* Print Results */
        if (total_con == 1) {
            printf("There is only 1 instance of consecutive primes for all integers less than %d.\n", N);
            fflush (stdout);
        } else {
            setlocale(LC_NUMERIC, "");
            printf("There are %d instances of consecutive primes for all integers less than %'d.\n", total_con, N);
            fflush (stdout);
        }

        printf("\n<Program Runtime: %.4fs>\n\n", elapsed_time);		//print elapsed time
        fflush (stdout);
    }

    return 0;
}

/* ------------------------------------------------------------------------
  Name -            isPrime
  Purpose -         Determines if a Value is Prime
  Parameters -      num: an Integer to Determine if Prime
  Returns -         true:   Number is Prime
                    false:  Number is Not Prime
  Side Effects -    None
  ----------------------------------------------------------------------- */
bool isPrime(int num){

    int i;

    /* Verify Number is not 0 or 1 */
    if (num == 0 || num == 1){
        return false;
    }

    /* Determine if Number is Prime */
    for(i=2; i <= num / 2; i++){            //for each value (i) 2 to num/2

        if(num % i != 0)                    //if i does not divide evenly
            continue;                       //continue loop for remaining i
        else                                //else
            return false;                   //number is not prime
    }

    return true;                            //number is prime

}

/* ------------------------------------------------------------------------
  Name -            FindRange
  Purpose -         Finds range for process using block allocation
  Parameters -      p:		  pointer to process structure
					max:	  range max (N)
					numProcs: number of process to split between	
  Returns -         None: data is placed in process pointed to by p
  Side Effects -    Process structure is updated
  ----------------------------------------------------------------------- */
void FindRange(pProc p, int max, int numProcs){

    p->start = FIND_START(p->pid, max, numProcs);
    p->end = (FIND_END(p->pid, max, numProcs));

    /* Set Start and End Values to Odd */
    if(p->start % 2 == 0){    	//if starts on an even value
        p->start+=1;         	//plus 1 to increment to next odd
    }

    if(p->end != max){      	//if not last value

        if(p->end % 2 == 0){    //if it ends on an even value
            p->end+=1;          //plus 1 to increment to next odd
        }
        else{
            p->end+=2;          //plus 2 to increment to next odd

        }
    }

    printf("%d: %d-%d\n", p->pid, p->start, p->end);
}