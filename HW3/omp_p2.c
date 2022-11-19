/* ------------------------------------------------------------------------
    omp_p2.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // // //
//
//	OMP Program
//
// // // // // // // //

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 1000000

int main (int argc, char *argv[])
{
	
	int   i, n;
	int tid, nthreads;
	float a[N], b[N], sum, localsum;
	double start, stop;
	
	/* Fork Threads */
	omp_set_num_threads(5);
	#pragma omp parallel shared(a,b,sum) private(tid, i, localsum)
	{
	
		tid = omp_get_thread_num();
		
		/* Only Master Thread */
		if (tid == 0){
			nthreads = omp_get_num_threads();
			printf("Number of threads = %d\n\n\n", nthreads);
			start = omp_get_wtime();	//program start time
		}
		
		/* Some initializations */
		#pragma omp for
		for (i=0; i < N; i++){
			a[i] = b[i] = i * 1.0;
		}
		
		localsum = 0.0;

		/* Compute Local Sum */
		#pragma omp for
	  	for (i=0; i < N; i++){
			localsum = localsum + (a[i] * b[i]*b[i]*b[i]);			
		}
		
		#pragma omp barrier
		/* Compute Global Sum */	
		#pragma omp critical
		sum = sum+=localsum;
		
	} /* End of Parallel Region */
	
	printf("Sum = %f\n",sum);
	
	/* Calculate Runtime */
	stop = omp_get_wtime();
	printf("\n<Program Runtime: %.4fs>\n\n", stop - start);

	return 0;
}
