/* ------------------------------------------------------------------------
    student_mpi.c

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
//	counts 'acceptable' unique student id numbers
//
//  'acceptable' student ids:
//	    are a six-digit combination of the numerals 0-9
//	    do not contain 0 as the first digit
//      do not contain two consecutive digits of the same value
//	    do not sum to 7, 11, or 13
//          (i.e. 13126 would be invalid as it sums to 13)
// // // // // // //


/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
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
/** ----------------------------- Macros -------------------------------- **/
/*
 * Function Keys
 *      i : pid/rank
 *      n : max value
 *      p : total processes
 */
#define FIND_START(i,n,p)	floor((i*n)/(p))
#define FIND_END(i,n,p)     (floor((i+1)*(n)/(p)))-1

/** ----------------------------- Constants ----------------------------- **/
#define MAX_ID 999999

/** ----------------------------- Global -------------------------------- **/
bool verbose = false;

/** ----------------------------- Prototypes ---------------------------- **/
bool ContainsConsecutiveValues(int studentID);
bool ComputesInvalidSum(int studentID);
void FindRange(pProc p, int max, int numProcs);
void ProcessArgs(int argc, char ** argv);
void Usage();
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
    /*** *** *** *** *** *** ***
    *  Program Initialization
    *** *** *** *** *** *** ***/
    /* Program Initialization */
    int localCount = 0;				//local number of valid sids (one process)
    int totalCount = 0;		        //total number of valid sids (all processes)
    double elapsed_time;
    int numProcs, rank, nameLen;

    /* Parallelization Init */
    MPI_Init(&argc, &argv);			//Initializes the MPI execution environment
    MPI_Barrier (MPI_COMM_WORLD);	//Blocks caller until all processes in the communicator have called it

    elapsed_time = - MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);		//Determines the rank (PID) of the calling process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);	//Determines size of the processor group associated

    if(verbose){
        printf("Process %d starting... \n", rank);
    }
    
    /* Initialize Process Information */
    proc process;

    process.start = process.end = 0;
    process.pid = rank;

    /* Determine Process Section */
    FindRange(&process, MAX_ID, numProcs);

    /*** *** *** *** *** *** ***
     *     Find Valid SIDs
     *** *** *** *** *** *** ***/
    if(verbose){
        printf("Process %d searching sid range: %d-%d \n", rank, process.start, process.end);
    }
    for(int localSID = process.start; localSID <= process.end; localSID++){

        if(localSID > 100000){
            if((!ContainsConsecutiveValues(localSID)) && (!ComputesInvalidSum(localSID))){
                localCount += 1;
            }
        }
    }
    if(verbose){
        printf("Process %d found %d valid SIDs \n", rank, localCount);
    }

    /*** *** *** *** *** *** ***
     * Terminate Parallelization
     *** *** *** *** *** *** ***/
    MPI_Reduce (&localCount,
                &totalCount,
                1,
                MPI_INT,
                MPI_SUM,
                0,
                MPI_COMM_WORLD);				//Reduces values on all processes within a group
    elapsed_time += MPI_Wtime();				//Calculate elapsed time

    MPI_Finalize();								//Terminates MPI execution environment

    /*** *** *** *** *** *** ***
     *  Program Termination
     *** *** *** *** *** *** ***/
    if (!rank) {								//if process 0
        /* Print Results */
        setlocale(LC_NUMERIC, "");
        printf("There are %'d valid student IDs\n", totalCount);
        fflush (stdout);

        printf("\n%-1s %-10s %-5.4fs %-1s\n\n",
               "<","Program Runtime:", elapsed_time, ">");
        fflush (stdout);
    }

    return 0;
}


/* ------------------------------------------------------------------------
  Name -            ContainsConsecutiveValues
  Purpose -         Checks if studentID contains any consecutive values
  Parameters -      studentID:  ID to check
  Returns -         true:       sid contains consecutive values
                    false:      sid does not contain consecutive values
  Side Effects -    none
  ----------------------------------------------------------------------- */
bool ContainsConsecutiveValues(int studentID){

    int digit, prevDigit;
    bool firstIteration = true;

    /* Iterate StudentID */
    while (studentID != 0)
    {
        digit = studentID % 10;     //obtain far right value

        if(!firstIteration && prevDigit == digit){  //compare to previous value
            return true;        //return true if values match
        }

        prevDigit = digit;          //save new 'previous value'
        firstIteration = false;     //switch flag
        studentID /= 10;            //Increment one digit left
    }

    return false;
}


/* ------------------------------------------------------------------------
  Name -            ComputesInvalidSum
  Purpose -         Checks if studentID computes 7, 11, or 13
  Parameters -      studentID:  ID to check
  Returns -         true:       sid computes 7, 11, or 13
                    false:      sid does not compute 7, 11, or 13
  Side Effects -    none
  ----------------------------------------------------------------------- */
bool ComputesInvalidSum(int studentID){
    int sum = 0, digit;

    /* Calculate Sum */
    while (studentID != 0)
    {
        digit = studentID % 10;     //obtain far right value
        sum += digit;               //add digit to sum
        studentID /= 10;            //Increment one digit left
    }

    /* Verify Sum is Acceptable */
    if (sum == 7 ||  sum ==11 || sum ==13){     //check sum value
        return true;                //return true if sum is 7, 11, or 13
    }
    else{
        return false;               //otherwise, return false
    }
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


/* ------------------------------------------------------------------------
  Name -            ProcessArgs
  Purpose -         Process user arguments
  Parameters -      argc:   number of arguments passed to program
                    argv:   pointer to arguments passed to program
  Returns -         None
  Side Effects -    When necessary, updates global variables
  ----------------------------------------------------------------------- */
void ProcessArgs(int argc, char ** argv){

    /*** *** *** *** *** *** ***
      *     Process Arguments
      *** *** *** *** *** *** ***/
    /* Get Input */
    int input = 0;
    int threads = 0;

    input = getopt( argc, argv,"hvt:" );

    /* Handle if No Arguments */
    if (input == -1){
        printf("Note: Defaults Set [No Arguments Given]\n\n");
        fflush(stdout);
        return;
    }

    /* Process All Arguments */
    while (input != -1) {
        switch (input) {

            /* Verbose Mode */
            case 'v':
                verbose = true;
                break;

            /* Access Help Menu */
            case 'h':
            default:
                Usage();
                exit(0);
        }

        input = getopt( argc, argv, "hvt:" );
    }
}


/* ------------------------------------------------------------------------
  Name -            Usage
  Purpose -         Prints Help Menu
  Parameters -      None
  Returns -         None
  Side Effects -    None
  ----------------------------------------------------------------------- */
void Usage(){

    printf("\nAn MPI Program for counting 'acceptable' unique student id numbers\n"
           "ver 1.0, 2022\n\n"
           "Usage: student_mpi -h -v\n\n"
           "\t%-14s %-24s %-16s\n\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n",

           "-h:", "Display Usage summary", "Example: proj_seq -h",
           "-v:", "Set Verbose Mode", "Default:", verbose == true ? "True" : "False", "|", "Example: proj_seq -v");
}

