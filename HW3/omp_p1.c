/* ------------------------------------------------------------------------
    omp_p1.c

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

#define INTERVALS 100000
#define THREADS	2


int main(int arc, char* argv[])
{
	
	double area;		/* The final anser */
	double ysum;		/* Sum of rectangle heights */
	double xi;			/* Midpoint of interval */
	int i;	
	int tid;
	int nthreads;
	double start, stop;

	ysum = 0.0;
	
	/* Fork Threads */
	omp_set_num_threads(5);
	#pragma omp parallel private(tid, i)
	{
	
		tid = omp_get_thread_num();
		
		/* Only Master Thread */
		if (tid == 0){
			nthreads = omp_get_num_threads();
			printf("Number of threads = %d\n\n\n", nthreads);
			start = omp_get_wtime();	//program start time
		}
		
		#pragma omp for schedule(static)
		for (i=0; i < INTERVALS; i++){
			xi=((1.0/INTERVALS)*(i+0.5));
			ysum+=4.0/(1.0+xi*xi);

		}

	} /* End of Parallel Region */

	area = ysum * (1.0/INTERVALS);
	printf("pi is %13.11f\n\n", area);
	
	/* Calculate Runtime */
	stop = omp_get_wtime();
	printf("\n<Program Runtime: %.4fs>\n\n", stop - start);

	return 0;
}
