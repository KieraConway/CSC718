/* ------------------------------------------------------------------------
    HW2-4.c

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
//	computer π using the rectangle rule with 1,000,000 intervals.
//
// // // // // // //


#include <stdio.h>
#include <mpi.h>			// Message Passing Interface
#include <math.h>

/** ----------------------------- Structures ---------------------------- **/
typedef struct procInfo proc;
typedef struct procInfo * pProc;

struct procInfo {
    int pid;            //process rank/pid
    int start;         //process start
    int end;           //process end
    int size;           //process size (end-start)
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
#define INTERVALS 1000000

/** ----------------------------- Global ----------------------------- **/


/** ----------------------------- Prototypes ---------------------------- **/
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

    double area_Local;	    // The final answer
    double area_Global;	    // The final answer
    double ysum;	        // Sum of rectangle heights
    double xi;              // Midpoint of interval
    int i;

    int numProcs, rank, nameLen;
    double elapsed_time;

    ysum = 0.0;

    /* Parallelization Init */
    MPI_Init(&argc, &argv);                         //Initializes the MPI execution environment
    MPI_Barrier (MPI_COMM_WORLD);					//Blocks caller until all processes in the communicator have called it

    elapsed_time = - MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);           //Determines the rank (PID) of the calling process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);       //Determines size of the processor group associated

    /* Initialize Process Information */
    proc process;

    process.start = process.end = 0;
    process.pid = rank;



    /* Determine Process Section */
    FindRange(&process, INTERVALS, numProcs);




    for(int i = process.start; i < process.end; i++){
        xi=((1.0/process.size)*(i+0.5));
        ysum+=4.0/(1.0+xi*xi);
    }

    area_Local = ysum * (1.0/process.size);

    MPI_Reduce (&area_Local,
                &area_Global,
                1,
                MPI_INT,
                MPI_SUM,
                0,
                MPI_COMM_WORLD);                    //Reduces values on all processes within a group
    elapsed_time += MPI_Wtime();                    //calculate elapsed time

    MPI_Finalize();									//Terminates MPI execution environment

    if (!rank) {                                        //if process 0

        /* Print Results */
        printf("Before Division:\n");
        printf("pi is %13.11f\n", area_Global);
        fflush (stdout);

        area_Global = area_Global / numProcs;

        printf("After Division:\n");
        printf("pi is %13.11f\n", area_Global);
        fflush (stdout);

        printf("\n<Program Runtime: %.4fs>\n\n", elapsed_time);		//print elapsed time
        fflush (stdout);
    }



    return 0;

}

int FindRange(pProc p, int max, int numProcs){

    p->start = FIND_START(p->pid, max, numProcs);
    p->end = (FIND_END(p->pid, max, numProcs));

    p->size = (p->end - p->start)+1;

//    /* Set Start and End Values to Odd */
//    if(p->start % 2 == 0){    //if starts on an even value
//        p->start+=1;          //plus 1 to increment to next odd
//    }
//
//    if(p->end != max){      //if not last value
//
//        if(p->end % 2 == 0){    //if it ends on an even value
//            p->end+=1;          //plus 1 to increment to next odd
//        }
//        else{
//            p->end+=2;          //plus 2 to increment to next odd
//
//        }
//    }

    printf("%d: %d-%d\n", p->pid, p->start, p->end);
}