/* ------------------------------------------------------------------------
    omp_p3.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // // //
//
//	openMP
//
// // // // // // // //

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VECLEN 100000000

float a[VECLEN], b[VECLEN], sum;

float dotprod (){
	
	int i, tid, nthreads;

	#pragma omp for  private(i) reduction(+:sum)
	for (i=0; i < VECLEN; i++){
		int p = sum;
		sum = sum + (a[i]*b[i]);
		
		//printf("%d[%d]\n", tid, i);
		//printf("%d[%d]:a(%lf)x b(%lf) = %f | %d -> %lf\n", tid, i, a[i], b[i],a[i]*b[i], p, sum);
	}


	
}


int main (int argc, char *argv[]) {
		
	int tid, i, nthreads;
	double start, stop;

		/* Fork Threads */
	omp_set_num_threads(5);
	#pragma omp parallel shared(a,b, sum) private(tid, i)
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
		for (i=0; i < VECLEN; i++){
			a[i] = b[i] = 1.0 * i;
		}
		sum = 0.0;
	
	}
	
	dotprod();
		

	printf("Sum = %f\n", sum);
	
	/* Calculate Runtime */
	stop = omp_get_wtime();
	printf("\n<Program Runtime: %.4fs>\n\n", stop - start);

	return 0;

}

