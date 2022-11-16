#include <stdio.h>
#define INTERVALS 1000000

int main(int arc, char* argv[])
{
	double area;	/* The final anser */
	double ysum;	/* Sum of rectangle heights */
	double xi;	/* Midpoint of interval */
	int i;	

	ysum = 0.0;

	for (i=0; i < INTERVALS; i++)
	{
		xi=((1.0/INTERVALS)*(i+0.5));
		ysum+=4.0/(1.0+xi*xi);
	}

	area = ysum * (1.0/INTERVALS);
	printf("pi is %13.11f\n", area);
	return 0;
}
