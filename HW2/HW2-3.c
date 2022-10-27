/* ------------------------------------------------------------------------
    HW2-3.c

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
//	Finds the largest gap between a pair of consecutive
//	prime numbers for all integers from 2 to 1,000,000
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
    int pid;            //process rank/pid
    int start;         //process start
    int end;           //process end
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

/** ----------------------------- Global ----------------------------- **/
/** ----------------------------- Prototypes ---------------------------- **/
bool isPrime(int num);
void Usage();
int FindRange(pProc p, int max, int numProcs);

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
    int curr_Val = 0;               //next prime value
    int prev_Val = 0;               //previous prime value

    int gap_Start = 0;
    int gap_End = 0;
    int local_Gap = 0;              //local largest consecutive gap
    int largest_Gap = 0;            //'global' largest consecutive gap
    int curr_Gap = 0;               //current gap

    double elapsed_time;
    int numProcs, rank, nameLen;

    /* Begin Parallelization */
    MPI_Init(&argc, &argv);                         //Initializes the MPI execution environment
    MPI_Barrier (MPI_COMM_WORLD);					//Blocks caller until all processes in the communicator have called it

    elapsed_time = - MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);           //Determines the rank (PID) of the calling process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);       //Determines size of the processor group associated


//    /*** Process Arguments ***/
//    int input = 0;
//    input = getopt( argc, argv,"hn:" );
//
//    if (input != -1)
//    {
//        switch (input)
//        {
//            /* Change Port */
//            case 'n':
//                N = atoi(optarg);
//                break;
//
//                /* Access Help Menu */
//            case 'h':
//
//            default:
//                Usage();
//                exit(0);
//        }
//    }

/* Initialize Process Information */
    proc process;

    process.start = process.end = 0;
    process.pid = rank;

    /* Determine Process Section */
    FindRange(&process, N, numProcs);

    /* Iterate through Section */
    bool found_Next = false;            //specifies if next consecutive set has been found
    for(int i = process.start; (i <= process.end|| !found_Next) && i <= N; i+=2){

        /* Determine if value is Prime */
        if (isPrime(i)) {
            curr_Val = i;                               //save i as next

            /* Determine if Primes are Consecutive */
            if (i == 3 || curr_Val - prev_Val == 2) {
                gap_End = prev_Val;                   //save prev value as end of gap

                /* If Past end of Section */
                if ( i > process.end){
                    found_Next = true;
                }

                /* Calculate Gap */
                if(i!=3 && gap_End >= gap_Start && gap_Start != 0){
                    curr_Gap = gap_End - gap_Start;

                    if (curr_Gap > local_Gap){
                        local_Gap = curr_Gap;
                    }
                }

                printf("%d, %d\n", prev_Val, curr_Val);
                printf("%d-%d = %d        [[%d]]\n", gap_End, gap_Start, curr_Gap, local_Gap);

                /* Update Start for next Loop */
                gap_Start = curr_Val;                   //save last value as beginning of gap


            }

            prev_Val = i;                               //save i as previous
        }

    }

    MPI_Reduce (&local_Gap,
                &largest_Gap,
                1,
                MPI_INT,
                MPI_MAX,
                0,
                MPI_COMM_WORLD);                    //Reduces values on all processes within a group
    elapsed_time += MPI_Wtime();                    //calculate elapsed time

    MPI_Finalize();									//Terminates MPI execution environment

    if (!rank) {                                        //if process 0

        /* Print Results */
        printf("The largest gap between a pair of consecutive prime numbers "
               "for all integers from 2 to %'d is %'d.\n", N, largest_Gap);

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
  Name -            Usage
  Purpose -         Prints Help Menu
  Parameters -      None
  Returns -         None
  Side Effects -    None
  ----------------------------------------------------------------------- */
void Usage(){
    printf("\nHW2-1.c, A Sequential Program for finding Consecutive Primes\n"
           "ver 1.0, 2022\n"
           "Usage: HW2-1 -h -n max_values\n"
           "	-h: Display Usage summary\n"
           "	-n: Change max value threshold   Default: 1,000,000    |   example: -n 1000 \n\n");
}

int FindRange(pProc p, int max, int numProcs){

    p->start = FIND_START(p->pid, max, numProcs);
    p->end = (FIND_END(p->pid, max, numProcs));

    /* Set Start and End Values to Odd */
    if(p->start % 2 == 0){    //if starts on an even value
        p->start+=1;          //plus 1 to increment to next odd
    }

    if(p->end != max){      //if not last value

        if(p->end % 2 == 0){    //if it ends on an even value
            p->end+=1;          //plus 1 to increment to next odd
        }
        else{
            p->end+=2;          //plus 2 to increment to next odd

        }
    }

    printf("%d: %d-%d\n", p->pid, p->start, p->end);
}