#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define VECLEN 100000000

float a[VECLEN], b[VECLEN], sum;

float dotprod ()
{
int i,tid;

  for (i=0; i < VECLEN; i++)
    {
    sum = sum + (a[i]*b[i]);
    }
}


int main (int argc, char *argv[]) {
int i;

for (i=0; i < VECLEN; i++)
  a[i] = b[i] = 1.0 * i;
sum = 0.0;

  dotprod();

printf("Sum = %f\n",sum);

}

