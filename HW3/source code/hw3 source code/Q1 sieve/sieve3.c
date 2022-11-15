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

int main (int argc, char *argv[])
{
 int count;		/* local prime count	*/
 double elapsed_time; /* execution time	*/
 int first;		/* index of the first sieve	*/
 int global_count;	/* global count of prime numbers */
 int high_value; 	/* highest value assigned to this process */
 int i;		/* loop counter		*/
 int id;		/* this process id	*/
 int index;		/* index of the current sieve	*/
 int low_value;	/* lowest value assigned to this process */
 int *marked;		/* array elements to be  marked	*/
 int *sieves;		/* array of sieving primes from k -> sqrt(n)	*/
 int n;		/* value of the largest number	*/
 int sqrtN;		/* square root of n	*/
 int p; 		/* number of processes		*/
 int proc0_size;	/* number of elements assigned to process zero */
			/* this is to find if process zero has all primes */
 int prime;		/* current prime or sieve	*/
 int size;		/* elements in marked array 	*/

 MPI_Init (&argc, &argv);
 /* start timer */
 MPI_Barrier(MPI_COMM_WORLD);
 elapsed_time = -MPI_Wtime();
 MPI_Comm_rank (MPI_COMM_WORLD, &id);
 MPI_Comm_size (MPI_COMM_WORLD, &p);

 if (argc != 2)
 {
  if (!id)
   printf ("Command line: %s <m>\n", argv[0]);

  MPI_Finalize();
  exit (1);
 }

 n = atoi(argv[1]);

 /* find how many elements are assigned to this process */
 low_value = 2 + BLOCK_LOW(id,p,n-1);
 high_value = 2 + BLOCK_HIGH(id,p,n-1);
 size = BLOCK_SIZE(id,p,n-1);
 proc0_size = (n-1)/p;
 sqrtN = (int) sqrt((double) n);
 if ((2 + proc0_size) < sqrtN)
 {
  if (!id)
   printf ("Too many processes\n");

  MPI_Finalize();
  exit (1);
 }

 marked = (int *) malloc (size * sizeof(int));
 sieves = (int *) malloc ((sqrtN) * sizeof(int));

 if ((marked == NULL) || (sieves == NULL))
 {
  printf ("Cannot allocate enough memory\n");
  MPI_Finalize();
  exit (1);
 }

 for (i = 0; i < size; i++)
  if (!(low_value % 2))
   marked[i] = i%2==0 ? 1:0;
  else
   marked[i] = i%2==0 ? 0:1;

 for (i=0; i < (sqrtN-2); i++) sieves[i]= i%2==0 ? 1:0;

 if (!id)
  marked[0] = 0;

 //Create a list of sieving primes on each process from 2 -> SQRT(n)
 sieves[0] = 0;
 index = 1;
 prime = 3;
 do 
 {
  for(i=prime-2+prime; i<(sqrtN-2); i+=prime) sieves[i] = 1;
  while (sieves[index]) index += 2;
  prime = index + 2;
  index += 2;
 } while (prime * prime <= sqrtN);

 //Mark all of the sieving primes from 2 -> n
 index = 1;
 prime = 3;
 do {
  if (prime * prime > low_value)
   first = prime * prime - low_value;
  else {
   if (!(low_value % prime)) first = 0;
   else first = prime - (low_value % prime);
  }
  for (i = first; i < size; i += prime) marked[i] = 1;

  while (sieves[index]) index += 2;
  prime = index + 2;
  index += 2;

 } while (prime * prime <= n);

 //Count the primes in each process 
 count = 0;
 for (i = 0; i < size; i++)
  if (!marked[i]) count++;

 //SUM each count into process 0.
 MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

 elapsed_time += MPI_Wtime();

 if (!id) {
  printf ("%d primes are less than or equal to %d\n", global_count, n);
  printf ("Total elapsed time: %10.6f\n", elapsed_time);
 }

 MPI_Finalize ();

 return 0;
}

