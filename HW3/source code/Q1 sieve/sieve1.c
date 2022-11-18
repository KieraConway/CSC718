/* Sieve of eratosthenes */
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//#include "MyMPI.h"

#define MIN(a,b)  ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW(((id)+1),p,n)-1)
#define BLOCK_SIZE(id,p,n) ((BLOCK_LOW(((id)+1),p,n))-(BLOCK_LOW(id,p,n)))
#define BLOCK_OWNER(index,p,n) (((p)*(index)+1)-1)/(n))

int main (int argc, char *argv[]){
	
int count;				/* local prime count	*/
double elapsed_time; 	/* execution time	*/
int first;				/* index of the first sieve	*/
int global_count;		/* global count of prime numbers */
int high_value; 		/* highest value assigned to this process */
int i;					/* loop counter		*/
int id;					/* this process id	*/
int index;				/* index of the current sieve	*/
int low_value;			/* lowest value assigned to this process */
int *marked;			/* array elements to be  marked	*/
int n;					/* value of the largest number	*/
int p; 					/* number of processes		*/
int proc0_size;			/* number of elements assigned to process zero */
						/* this is to find if process zero has all primes */
int prime;				/* current prime or sieve	*/
int size;				/* elements in marked array 	*/


/* 
 *
 * Pre Algorithm 
 *
 */
 
/* Parallelization Init */
MPI_Init (&argc, &argv);                        //Initializes the MPI execution environment
MPI_Barrier(MPI_COMM_WORLD);					//Blocks caller until all processes in the communicator have called 

elapsed_time = -MPI_Wtime();					//Start timer

MPI_Comm_rank (MPI_COMM_WORLD, &id);           	//Determines the rank (PID) of the calling process
MPI_Comm_size (MPI_COMM_WORLD, &p);       		//Determines size of the processor group associated




/* Parse User Input */
// Input Incomplete:
if (argc != 2) {								//If argument is missing
	
	if (!id)									//Process 0 prints error message
		printf ("Command line: %s <m>\n", argv[0]);

	MPI_Finalize();								//Terminates MPI execution environment
	exit (1);									//Returns 1 (execution failed)
}

// Input Complete:
n = atoi(argv[1]);								//Convert input string to an integer



/* Determine Contiguous Block */
low_value = 2 + BLOCK_LOW(id,p,n-1);			//Calculate low value for process
high_value = 2 + BLOCK_HIGH(id,p,n-1);			//Calculate high value for process	
size = BLOCK_SIZE(id,p,n-1);					//Calculate number of elements assigned to process

/* Verify Acceptable Block Size */

// Important!
//
//	this algorithm works only if the square of
//	the largest value in process 0 is greater than 
//	the upper limit of the sieve.
//
//	this section of code verifies that
//

proc0_size = (n-1)/p;								//Calculate size of Process 0

if ((2 + proc0_size) < (int) sqrt((double) n)) {	//If condition (explained above) not met 
	if (!id){										//Process 0 prints error message
		printf ("Too many processes\n");
	}

	MPI_Finalize();									//Terminates MPI execution environment
	exit (1);										//Returns 1 (execution failed)
}


/* Allocate Memory for Process's Share of Array */
marked = (int *) malloc (size * sizeof(int));	//Allocate memory for block_size * int (4)

//Error Check: Malloc Return Value
if (marked == NULL){							//If malloc returned NULL 
	printf ("Cannot allocate enough memory\n");	//Print error message
	MPI_Finalize();								//Terminates MPI execution environment
	exit (1);									//Returns 1 (execution failed)
}




/* 
 *
 * Algorithm Step 1:  Create list of unmarked natural numbers 2, 3, …, n 
 *
 */
for (i = 0; i < size; i++) { 						//for values 0 to block size
	marked[i] = 0;								//create array of 'unmarked' values
}

/* 
 *
 * Algorithm Step 2:  Set 'Prime' to 2 
 *
 */
if (!id){ 										//If Process 0:
	index = 0;									//Set array index to 0
}
prime = 2;										//Set current prime being sieved to 2

/* 
 *
 * Algorithm Step 3: Repeat until prime^2 > n:
 *
 *	(a) Mark all multiples of prime between prime^2 and n
 *	(b) prime = (smallest unmarked number) if > prime
 *	(c) Process 0 broadcasts prime to rest of processes 
 *
 *
 */

do {

	/* 
	 *	Algorithm Step 3a
	 */
	
	/* Set Index to First Integer Needing Marked */
	if (prime * prime > low_value){					//if prime^2 is > smallest value in array
		first = prime * prime - low_value;			//set first to  (prime^2 - smallest value)
	}	
	else {											//if prime^2 is <= smallest value in array		
		if (!(low_value % prime)) 					//if low_value is multiple of prime
			first = 0;								//set first to 0
		else 										//if low_value is NOT multiple of prime
			first = prime - (low_value % prime);	//set first to [prime - (low_value % prime)]
	}
	
	/* Sieve Data: Mark multiples of prime from first to n*/
	for (i = first; i < size; i += prime) {
		marked[i] = 1;
	}
	

	/* 
	 *	Algorithm Step 3b
	 */

	/* Locate next Unmarked Location */
	if (!id) {										//if Process 0
		while (marked[++index]);					//locate next unmarked location (loops until marked[index]=0)
		prime = index + 2;							//increment prime	
	}
	
	
	/* 
	 *	Algorithm Step 3c
	 */
	/* Process 0 broadcasts prime to rest of processes */
	MPI_Bcast (&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);		//Root process broadcasts a message to all other processes
	
} while (prime * prime <= n);								//Continue for all values between prime^2 and n


/* 
 *
 * Algorithm Step 4: Return all unmarked numbers as primes  
 *
 */
 
count = 0;						//initialize count variable

for (i = 0; i < size; i++){		//for values 0 to process size
	if (!marked[i]){ 			//if value is unmarked
		count++;				//increment prime counter
	}	
}

MPI_Reduce (&count, 
			&global_count, 
			1, 
			MPI_INT, 
			MPI_SUM, 
			0, 
			MPI_COMM_WORLD);	//Reduces and sum local count value on all processes to global count

/* 
 *
 * Post Algorithm 
 *
 */

elapsed_time += MPI_Wtime();	//Calculate elapsed time	

if (!id) {						//Process 0 Prints Results
	printf ("%d primes are less than or equal to %d\n", global_count, n);
	printf ("Total elapsed time: %10.6f\n", elapsed_time);
}

MPI_Finalize ();				//Terminates MPI execution environment

return 0;						//Returns 0 (execution success)
}

