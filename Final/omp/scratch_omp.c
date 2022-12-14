/* ------------------------------------------------------------------------
    hps_omp.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

/*

The program can be used to calculate harmonic progression sum.

The simplest harmonic progression is
	1/, 1/2, 1/3, ...
Let S(n) = 1/1 + 1/2 + 1/3 + ... + 1/i.

For example S(7) = 2.592857142857 to 12 digits of precison.

*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <omp.h>

/** ----------------------------- Constants ----------------------------- **/
#define debugging true
/** ----------------------------- Global -------------------------------- **/
/** ----------------------------- Macros -------------------------------- **/
/*
 * Function Keys
 *      i : pid/rank
 *      n : max value
 *      p : total processes
 */
#define FIND_START(i,n,p)	floor((i*n)/(p))
#define FIND_END(i,n,p)     (floor((i+1)*(n)/(p)))-1
/** ----------------------------- Prototypes ---------------------------- **/

/** ----------------------------- Functions ----------------------------- **/

/*****************************************************
 *
 *  Function:  main()
 *
 *  Parameters:
 *
 *      argc - main parameter for argument count
 *      argv - main parameter for argument values
 *
******************************************************/
int main(int argc, char *argv[]) {

    /*** *** *** *** *** *** ***
    *  Program Initialization
    *** *** *** *** *** *** ***/

    /* Initialize Local Variables */
    long unsigned int *localDigits;
    long unsigned int remainder, div, mod;
    int i, localDigit;

    int tid, nThreads;
    int blockStart, blockEnd;
    double start, stop;

    /* Save Program Start Time */
    start = omp_get_wtime();	//program start time

    /*** *** *** *** *** *** ***
     *    Parse User Input
     *** *** *** *** *** *** ***/

    /* Read in Input */
    if (argc != 4)
    {
        printf("usage: hps n d t\n");
        printf("    n: value of N (Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5)\n");
        printf("    d: numbers of Digits (Example: 5 => 0,xxxxx) \n");
        printf("    t: numbers of Threads \n");
        exit(-1);
    }

    /* Save Input */
    int n = atoi((char*)argv[1]);	// value of N
    int d = atoi((char*)argv[2]);	// numbers of localDigits
    int t = atoi((char*)argv[3]);	// numbers of threads

#pragma omp parallel shared(nThreads) private(tid, localDigits, localDigit, blockStart, blockEnd, div, mod, remainder) num_threads(t)
    {
        /*** *** *** *** *** *** ***
         * Print Thread Information
         *** *** *** *** *** *** ***/
         /* General Thread Information */
        tid = omp_get_thread_num();

#pragma omp single
        {
            //omp single: executed by single thread, implicit barrier upon completion of the region

            nThreads = omp_get_num_threads();
            printf("Number of threads = %d\n\n", nThreads);
            fflush(stdout);
        }

        printf("Thread %d starting...\n",tid);
        fflush(stdout);

        /* Block Allocation */
        blockStart =  FIND_START(tid, n, nThreads) + (double) 1;
        blockEnd = FIND_END(tid, n, nThreads) + (double) 1;

        printf("Thread %d calculating harmonic progression sum for [1/%d] -> [1/%d]\n",
               tid, blockStart, blockEnd);
        fflush(stdout);

        /*** *** *** *** *** *** ***
         * Allocate Digits Variable
         *** *** *** *** *** *** ***/
        /* Allocate Memory for Digits */
        localDigits = (long unsigned int *)malloc(sizeof(long unsigned int) * (d + 11));
        if (!localDigits) {
            printf("Thread %d Error: memory allocation failed\n", tid);
            exit(-1);
        }
        /* Init Digits to 0 */
        for (localDigit = 0; localDigit < d + 11; ++localDigit)
            localDigits[localDigit] = 0;

        /*** *** *** *** *** *** ***
         *      Calculate HPS
         *** *** *** *** *** *** ***/
        /* Calculate HPS saving each localDigit in localDigits[i] */
        for (i = blockStart; i <= blockEnd; ++i)
        {
            remainder = 1;
            for (localDigit = 0; localDigit < d + 11 && remainder; ++localDigit) 	{
                div = remainder / i;
                mod = remainder % i;
                localDigits[localDigit] += div;
                remainder = mod * 10;
            }
        }
        /* step r1:
         *      init regrouping, from d+11-1 to 1
         *      regroup any localDigits[i] > 10
         */
        for (i = d + 11 - 1; i > 0; --i)
        {
            localDigits[i - 1] += localDigits[i] / 10;
            localDigits[i] %= 10;
        }

    }





    /* step r2:
     *      skip all the localDigits after d
     *      round if localDigits[i] > 5
     */
    if (localDigits[d + 1] >= 5)
    {
        ++localDigits[d];
    }

    /* step r3:
     *      init regrouping again because of r2
     *      regroup any localDigits[i] > 10
     */
    for (i = d; i > 0; --i)
    {
        localDigits[i - 1] += localDigits[i] / 10;
        localDigits[i] %= 10;
    }

    /*** *** *** *** *** *** ***
     *      Print Sum
     *** *** *** *** *** *** ***/
    /* Print Digits Left of Decimal */
    printf("%ld.", localDigits[0]);

    /* Print Digits Right of Decimal */
    for (i = 1; i <= d; ++i)
    {
        printf("%ld", localDigits[i]);
    }
    printf("\n");

    /*** *** *** *** *** *** ***
     *Deallocate Digits Variable
     *** *** *** *** *** *** ***/
    free(localDigits);

    /*** *** *** *** *** *** ***
     *  Program Termination
     *** *** *** *** *** *** ***/
    /* Calculate Runtime */
    stop = omp_get_wtime();
    double programTime = (double)(stop - start);  //calculate program runtime

    printf("\n%-1s %-10s %-5.4fs %-1s\n\n",
           "<","Program Runtime:", programTime, ">");

    return 0;
}
