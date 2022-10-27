
/* ------------------------------------------------------------------------
    Midterm - Question 4
	
	MPI parallel program that computes the sum 1+2+...+p 

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */
/*

	As a way of double checking the result, process 0 should also compute and print the value p(p+1)/2. 

*/

#include <stdio.h>
#include <mpi.h>			// Message Passing Interface

#define VERIFY_SUMMATION(p)	((p*(p+1))/(2))

int MAX_VALUE = 10;			// p

int main(int argc, char *argv[]){
    int i, numProcs, rank, nameLen, sum;
    char procName[MPI_MAX_PROCESSOR_NAME];

    double elapsed_time;
    int count; 										//Local Sum
    int global_count;								//Global Sum

    sum = VERIFY_SUMMATION(MAX_VALUE);				//obtain correct sum for verification

    MPI_Init(&argc, &argv);                         //Initializes the MPI execution environment
    MPI_Barrier (MPI_COMM_WORLD);					//Blocks caller until all processes in the communicator have called it

    elapsed_time = - MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);           //Determines the rank (PID) of the calling process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);       //Determines size of the processor group associated
    MPI_Get_processor_name(procName, &nameLen);     //Gets the name of the processor

    count = 0;										//Initialize count
    for (i = rank; i <= MAX_VALUE; i += numProcs){	//Ensure cyclic allocation
        count += i;
    }

    MPI_Reduce (&count,
                &global_count,
                1,
                MPI_INT,MPI_SUM,
                0,
                MPI_COMM_WORLD);					//Reduces values on all processes to a single value

    elapsed_time += MPI_Wtime();

    printf("%s [Processor %d of %d] has completed.\n",
           procName, rank, numProcs);				//Print Completion update
    fflush (stdout);

    MPI_Finalize();									//Terminates MPI execution environment

    if (!rank){										//if process 0

        if (global_count == sum){

            printf ("\nThe sum of all values between 1 and %d is %d\n", 
                    MAX_VALUE, 
                    global_count);					//Print total sum
            fflush (stdout);
        }
        else{

            printf("Error: Cyclical Summation Failed\n");	//print error message
            fflush (stdout);
        }


        printf ("Elapsed time: %f.\n", elapsed_time);		//print elapsed time
        fflush (stdout);
    }

    return 0;
}