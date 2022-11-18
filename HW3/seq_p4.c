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
#include <time.h>

#define NRA 620                 /* number of rows in matrix A */
#define NCA 150                 /* number of columns in matrix A */
#define NCB 70                  /* number of columns in matrix B */

int main (int argc, char *argv[])
{
	/* Program Start Time */
	clock_t start = clock();
	
int	tid, nthreads, i, j, k, chunk;
double	a[NRA][NCA],           /* matrix A to be multiplied */
	b[NCA][NCB],           /* matrix B to be multiplied */
	c[NRA][NCB];           /* result matrix C */

chunk = 10;                    /* set loop iteration chunk size */


  /*** Initialize matrices ***/
  for (i=0; i<NRA; i++)
    for (j=0; j<NCA; j++)
      a[i][j]= i+j;

  for (i=0; i<NCA; i++)
    for (j=0; j<NCB; j++)
      b[i][j]= i*j;

  for (i=0; i<NRA; i++)
    for (j=0; j<NCB; j++)
      c[i][j]= 0;


  for (i=0; i<NRA; i++)
    {
    for(j=0; j<NCB; j++)
      for (k=0; k<NCA; k++)
        c[i][j] += a[i][k] * b[k][j];
    }


	/*** Print results ***/
	printf("******************************************************\n");
	printf("Result Matrix:\n");
	for (i=0; i<NRA; i++)
	  {
	  for (j=0; j<NCB; j++)
		printf("%6.2f   ", c[i][j]);
	  printf("\n");
	  }
	printf("******************************************************\n");
	printf ("Done.\n");

	/* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);

	return 0;

}
