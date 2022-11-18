/* ------------------------------------------------------------------------
    seq_p2.c

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
#include <math.h>
#include <time.h>

int main (int argc, char *argv[])
{
	/* Program Start Time */
	clock_t start = clock();
	
	int   i, n;
	float a[1000000], b[1000000], sum;

	/* Some initializations */
	n = 1000000;
	for (i=0; i < n; i++)
		a[i] = b[i] = i * 1.0;

	sum = 0.0;

  	for (i=0; i < n; i++)
		sum = sum + (a[i] * b[i]*b[i]*b[i]);

	printf("   Sum = %f\n",sum);
	
	/* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);

	return 0;
}
