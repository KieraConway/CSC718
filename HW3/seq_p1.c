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

#include <stdio.h>
#include <time.h>
#define INTERVALS 1000000

int main(int arc, char* argv[])
{
	/* Program Start Time */
	clock_t start = clock();
	
	double area;		/* The final anser */
	double ysum;		/* Sum of rectangle heights */
	double xi;			/* Midpoint of interval */
	int i;	

	ysum = 0.0;

	for (i=0; i < INTERVALS; i++)
	{
		xi=((1.0/INTERVALS)*(i+0.5));
		ysum+=4.0/(1.0+xi*xi);
	}

	area = ysum * (1.0/INTERVALS);
	printf("pi is %13.11f\n", area);
	
	/* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);

	return 0;
}
