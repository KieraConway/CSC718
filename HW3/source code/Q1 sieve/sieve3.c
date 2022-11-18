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
int *sieves;			/* array of sieving primes from k -> sqrt(n)	*/
int n;					/* value of the largest number	*/
int sqrtN;				/* square root of n	*/
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

sqrtN = (int) sqrt((double) n);						//Calculate square root of n
if ((2 + proc0_size) < sqrtN) {						//If condition (explained above) not met 
	if (!id){										//Process 0 prints error message
		printf ("Too many processes\n");
	}

	MPI_Finalize();									//Terminates MPI execution environment
	exit (1);										//Returns 1 (execution failed)
}


/* Allocate Memory for Process's Share of Array */
 marked = (int *) malloc (size * sizeof(int));		//Allocate memory for block_size * int (4)
 sieves = (int *) malloc ((sqrtN) * sizeof(int));

//Error Check: Malloc Return Value
if ((marked == NULL) || (sieves == NULL)) {			//If either malloc returned NULL 
	printf ("Cannot allocate enough memory\n");		//Print error message
	MPI_Finalize();									//Terminates MPI execution environment
	exit (1);										//Returns 1 (execution failed)
}




/* 
 *
 * Algorithm Step 1:  Create list of unmarked natural numbers 2, 3, …, n 
 *
 */
 /* Create Marked List and Mark all Even Numbers*/
for (i = 0; i < size; i++) {					//for values 0 to block size
	if (!(low_value % 2)) {						//if lowest value is even
		marked[i] = i%2==0 ? 1:0;				//marked[even] = even
	}
	else {										//if lowest value is odd
		marked[i] = i%2==0 ? 0:1;				//marked[odd] = even
	}
}

 /* Create Sieve List and Set all Even Indexes*/
for (i=0; i < (sqrtN-2); i++) {					//for values 0 to square root of n
	sieves[i]= i%2==0 ? 1:0;					//sieves[even] = 1
}

/* 
 *
 * Algorithm Step 2:  Set 'Prime' 
 *
 */

if (!id){ 										//If Process 0:
	marked[0] = 0;								//unmarks first value in array
}


prime = 3;										//Set current prime being sieved to 3

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
 
/*
 *	Algorithm Step 3a
 */
 
 /* Create List of Sieving Primes from 2 to SQRT(n) */
sieves[0] = 0;									//unset first value in array
index = 1;										//Set array index to 1

do {
	
	/* Sieve Data: Set Non-primes from 4 to sqrt of n*/
	for(i=prime-2+prime; i<(sqrtN-2); i+=prime){
		sieves[i] = 1;							//
	}	
	
	/* 
	 *	Algorithm Step 3b
	 */
	 
	/* Locate next Unmarked Location */
	while (sieves[index]){						//loop until sieves[index]=0
		index += 2;								//increment loop by 2
	}
	
	prime = index + 2;							//increment prime
	index += 2;									//increment index

} while (prime * prime <= sqrtN);

//Mark all of the sieving primes from 2 -> n
index = 1;										//Set current array index to 1
prime = 3;										//Set current prime being sieved to 3


 
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

	/* Locate next Unmarked Sieves Location to determine new Prime */
	while (sieves[index]){						//loop until sieves[index]=0
		index += 2;								//increment loop by 2
	}
	prime = index + 2;							//increment prime
	index += 2;									//increment index

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

