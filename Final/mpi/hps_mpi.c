/* ------------------------------------------------------------------------
    hps_mpi.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

/*

The program can be used to calculate harmonic progression sum.

The simplest harmonic progression is
	1/, 1/2, 1/3, ...
Let S(n) = 1/1 + 1/2 + 1/3 + ... + 1/i.

For example S(7) = 2.592857142857 to 12 digits of precison.

*/

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>			// Message Passing Interface

/** ----------------------------- Structures ---------------------------- **/
typedef struct procInfo proc;
typedef struct procInfo * pProc;

struct procInfo {
    int pid;				//process rank/pid
    int start;				//process start
    int end;				//process end
};
/** ----------------------------- Macros -------------------------------- **/
/*
 * Function Keys
 *      i : pid/rank
 *      n : max value
 *      p : total processes
 */
#define FIND_START(i,n,p)	floor((i*n)/(p))
#define FIND_END(i,n,p)     (floor((i+1)*(n)/(p)))-1
/** ----------------------------- Prototypes ---------------------------- **/
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
int main(int argc, const char **argv)
{

    /*** *** *** *** *** *** ***
    *  Program Initialization
    *** *** *** *** *** *** ***/
    /* Initialize Local Variables */
    long unsigned int *localDigits;
    long unsigned int *globalDigits;
    long unsigned int remainder, div, mod;
    int i, localDigitIndex, globalDigitIndex;

    double elapsed_time;
    int numProcs, rank;

    /*** *** *** *** *** *** ***
     *    Parse User Input
     *** *** *** *** *** *** ***/

    /* Read in Input */
    if (argc != 3)
    {
        printf("usage: hps n d\n");
        printf("    n: value of N (Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5)\n");
        printf("    d: numbers of Digits (Example: 5 => 0,xxxxx) \n");
        exit(-1);
    }

    /* Save Input */
    int n = atoi((char*)argv[1]);	// value of N
    int d = atoi((char*)argv[2]);	// numbers of localDigits

    /* Allocate Memory for Global Digits */
    localDigits = (long unsigned int *)malloc(sizeof(long unsigned int) * (d + 11));
    if (!localDigits)
    {
        printf("memory allocation failed\n");
        exit(-1);
    }

    /*** *** *** *** *** *** ***
    *  Process Initialization
    *** *** *** *** *** *** ***/
    /* Parallelization Init */
    MPI_Init(&argc, &argv);			//Initializes the MPI execution environment
    MPI_Barrier (MPI_COMM_WORLD);	//Blocks caller until all processes in the communicator have called it

    elapsed_time = - MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);		//Determines the rank (PID) of the calling process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);	//Determines size of the processor group associated

    printf("Process %d starting... \n", rank);
    fflush(stdout);

    /* Initialize Process Information */
    proc process;

    process.start = process.end = 0;
    process.pid = rank;

    /* Determine Process Section */
    FindRange(&process, n, numProcs);


    /*** *** *** *** *** *** *** *** ***
     *  Allocate Local Digits Variable
     *** *** *** *** *** *** *** *** ***/
    /* Allocate Memory for Local Digits */
    localDigits = (long unsigned int *)malloc(sizeof(long unsigned int) * (d + 11));
    if (!localDigits)
    {
        printf("Process %d Error: memory allocation failed\n", process.pid);
        exit(-1);
    }

    /* Init Local Digits to 0 */
    for (localDigitIndex = 0; localDigitIndex < d + 11; ++localDigitIndex)
        localDigits[localDigitIndex] = 0;

    /*** *** *** *** *** *** ***
     *      Calculate HPS
     *** *** *** *** *** *** ***/
    /* Calculate HPS saving each localDigitIndex in localDigits[i] */
    for (i = process.start; i <= process.end; ++i)
    {
        remainder = 1;
        for (localDigitIndex = 0; localDigitIndex < d + 11 && remainder; ++localDigitIndex) 	{
            div = remainder / i;
            mod = remainder % i;
            localDigits[localDigitIndex] += div;
            remainder = mod * 10;
        }
    }

    /* step r1:
     *      init regrouping, from d+11-1 to 1
     *      regroup any localDigits[i] > 10
     */
    for (i = d + 11 - 1; i > 0; --i)
    {
        localDigits[i - 1] += localDigits[i] / 10;
        localDigits[i] %= 10;
    }

    //todo: REMOVE BELOW
    /* Print Digits Left of Decimal */
    printf("%d[%d]: %ld.", process.pid, "0", localDigits[0]);

    /* Print Digits Right of Decimal */
    for (i = 1; i <= d; ++i)
    {
        printf("%d[%d]: %ld", process.pid, i, localDigits[i]);
    }
    printf("\n");
    //todo: end Remove

    /*** *** *** *** *** *** ***
     * Terminate Parallelization
     *** *** *** *** *** *** ***/
    MPI_Reduce (&localDigits,
                &globalDigits,
                1,
                MPI_INT,
                MPI_SUM,
                0,
                MPI_COMM_WORLD);				//Reduces values on all processes within a group
    elapsed_time += MPI_Wtime();				//Calculate elapsed time

    MPI_Finalize();								//Terminates MPI execution environment


    /* step r2:
     *      skip all the localDigits after d
     *      round if localDigits[i] > 5
     */
    if (globalDigits[d + 1] >= 5)
    {
        ++globalDigits[d];
    }

    /* step r3:
     *      init regrouping again because of r2
     *      regroup any localDigits[i] > 10
     */
    for (i = d; i > 0; --i)
    {
        globalDigits[i - 1] += globalDigits[i] / 10;
        globalDigits[i] %= 10;
    }

    /*** *** *** *** *** *** ***
     *      Print Sum
     *** *** *** *** *** *** ***/
    /* Print Digits Left of Decimal */
    printf("%ld.", globalDigits[0]);

    /* Print Digits Right of Decimal */
    for (i = 1; i <= d; ++i)
    {
        printf("%ld", globalDigits[i]);
    }
    printf("\n");

    /*** *** *** *** *** *** ***
     *Deallocate Digits Variable
     *** *** *** *** *** *** ***/
    free(localDigits);
    free(globalDigits);

    /*** *** *** *** *** *** ***
    *  Program Termination
     *** *** *** *** *** *** ***/

    /* Calculate Runtime */
    clock_t end = clock();
    double programTime = (double)(end - start) / CLOCKS_PER_SEC; //calculate program runtime

    printf("\n%-1s %-10s %-5.4fs %-1s\n\n",
           "<","Program Runtime:", programTime, ">");

    return 0;
}

/* ------------------------------------------------------------------------
  Name -            FindRange
  Purpose -         Finds range for process using block allocation
  Parameters -      p:		  pointer to process structure
					max:	  range max (n)
					numProcs: number of process to split between
  Returns -         None: data is placed in process pointed to by p
  Side Effects -    Process structure is updated
  ----------------------------------------------------------------------- */
void FindRange(pProc p, int max, int numProcs){

    p->start = FIND_START(p->pid, max, numProcs) + (double) 1;
    p->end = FIND_END(p->pid, max, numProcs)+ (double) 1;

    printf("Process %d searching SID range %d-%d\n", p->pid, p->start, p->end);
    fflush(stdout);
}
