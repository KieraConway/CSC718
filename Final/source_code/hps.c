
/*

The program can be used to calculate harmonic progression sum.

The simplest harmonic progression is
	1/, 1/2, 1/3, ...
Let S(n) = 1/1 + 1/2 + 1/3 + ... + 1/i.

For example S(7) = 2.592857142857 to 12 digits of precison.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, const char **argv)
{

    /*** *** *** *** *** *** ***
    *  Program Initialization
    *** *** *** *** *** *** ***/

    /* Initialize Local Variables */
    long unsigned int *digits;
    long unsigned int remainder, div, mod;
    int i, digit;

    /* Save Program Start Time */
    clock_t start = clock();

    /*** *** *** *** *** *** ***
     *    Parse User Input
     *** *** *** *** *** *** ***/

    /* Read in Input */
    if (argc != 3)
    {
        printf("usage: hps n d\n");
        printf("    n: value of N (Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5)\n");
        printf("    d: numbers of Digits (Example: 5 => 0,xxxxx) \n");
        exit(-1);
    }

    /* Save Input */
    int n = atoi((char*)argv[1]);	// value of N
    int d = atoi((char*)argv[2]);	// numbers of digits


    /*** *** *** *** *** *** ***
     * Allocate Digits Variable
     *** *** *** *** *** *** ***/
    /* Allocate Memory for Digits */
    digits = (long unsigned int *)malloc(sizeof(long unsigned int) * (d+11));
    if (!digits)
    {
        printf("memory allocation failed\n");
        exit(-1);
    }

    /* Init Digits to 0 */
    for (digit = 0; digit < d + 11; ++digit)
        digits[digit] = 0;

    /*** *** *** *** *** *** ***
     *      Calculate HPS
     *** *** *** *** *** *** ***/
    /* Calculate HPS saving each digit in digits[i] */
    for (i = 1; i <= n; ++i)
    {
        remainder = 1;
        for (digit = 0; digit < d + 11 && remainder; ++digit) 	{
            div = remainder / i;
            mod = remainder % i;
            digits[digit] += div;
            remainder = mod * 10;
        }
    }

    /* step r1:
     *      init regrouping, from d+11-1 to 1
     *      regroup any digits[i] > 10
     */
    for (i = d + 11 - 1; i > 0; --i)
    {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    /* step r2:
     *      skip all the digits after d
     *      round if digits[i] > 5
     */
    if (digits[d + 1] >= 5)
    {
        ++digits[d];
    }

    /* step r3:
     *      init regrouping again because of r2
     *      regroup any digits[i] > 10
     */
    for (i = d; i > 0; --i)
    {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    /*** *** *** *** *** *** ***
     *      Print Sum
     *** *** *** *** *** *** ***/
    /* Print Digits Left of Decimal */
    printf("%ld.", digits[0]);

    /* Print Digits Right of Decimal */
    for (i = 1; i <= d; ++i)
    {
        printf("%ld",digits[i]);
    }
    printf("\n");

    /*** *** *** *** *** *** ***
     *Deallocate Digits Variable
     *** *** *** *** *** *** ***/
    free(digits);

    /*** *** *** *** *** *** ***
    *  Program Termination
     *** *** *** *** *** *** ***/

    /* Calculate Runtime */
    clock_t end = clock();
    double programTime = (double)(end - start) / CLOCKS_PER_SEC; //calculate program runtime

    printf("\n%-1s %-10s %-5.4fs %-1s\n\n",
           "<","Program Runtime:", programTime, ">");

    return 0;
}
