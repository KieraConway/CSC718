/* ------------------------------------------------------------------------
    seq_p1.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // // //
//
//	Sequential Program
//
// // // // // // // //

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NRA 620                 /* number of rows in matrix A */
#define NCA 150                 /* number of columns in matrix A */
#define NCB 70                  /* number of columns in matrix B */

int main (int argc, char *argv[])
{
	
	int	tid, nthreads, i, j, k, chunk;
	double	a[NRA][NCA],           /* matrix A to be multiplied */
			b[NCA][NCB],           /* matrix B to be multiplied */
			c[NRA][NCB];           /* result matrix C */
	double start, stop;
		
	chunk = 10;                    /* set loop iteration chunk size */

	/* Fork Threads */
	omp_set_num_threads(5);
	#pragma omp parallel shared(a,b,c, nthreads) private(tid, i, j, k)
	{
		tid = omp_get_thread_num();
		
		/* Only Master Thread */
		if (tid == 0){
			nthreads = omp_get_num_threads();
			printf("Number of threads = %d\n\n\n", nthreads);
			start = omp_get_wtime();	//program start time
		}
	
		/*** Initialize matrices ***/
		#pragma omp for nowait
		for (i=0; i<NRA; i++)
			for (j=0; j<NCA; j++)
      			a[i][j]= i+j;
	
		#pragma omp for nowait
	  	for (i=0; i<NCA; i++)
			for (j=0; j<NCB; j++)
      			b[i][j]= i*j;
      			
      	#pragma omp for		
		for (i=0; i<NRA; i++)
			for (j=0; j<NCB; j++)
      			c[i][j]= 0;
	
		#pragma omp for
		for (i=0; i<NRA; i++){
			for(j=0; j<NCB; j++)
				for (k=0; k<NCA; k++)
					c[i][j] += a[i][k] * b[k][j];
    	}
	
	} /* End of Parallel Region */
	
	/*** Print results ***/
	printf("******************************************************\n");
	printf("Result Matrix:\n");
	for (i=0; i<NRA; i++){
	  	for (j=0; j<NCB; j++)
			printf("%6.2f   ", c[i][j]);
	  		printf("\n");
	}
	printf("******************************************************\n");
	printf ("Done.\n");
	
	/* Calculate Runtime */
	stop = omp_get_wtime();
	printf("\n<Program Runtime: %.4fs>\n\n", stop - start);

	return 0;

}
