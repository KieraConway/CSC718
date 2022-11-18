/* ------------------------------------------------------------------------
    seq_p3.c

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
#include <time.h>

#define VECLEN 100000000

float a[VECLEN], b[VECLEN], sum;

float dotprod (){
	
	int i,tid;

	for (i=0; i < VECLEN; i++){
		sum = sum + (a[i]*b[i]);
	}
}


int main (int argc, char *argv[]) {
	
	/* Program Start Time */
	clock_t start = clock();
	
	int i;

	for (i=0; i < VECLEN; i++)
	  a[i] = b[i] = 1.0 * i;
	sum = 0.0;

	  dotprod();

	printf("Sum = %f\n",sum);
	
	/* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);

	return 0;

}

