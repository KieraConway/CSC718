/* ------------------------------------------------------------------------
    HW2-1.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // //
// Program:
//
//	Sequential Program
//
//	Finds the number of times that two consecutive
//	odd integers are both prime numbers from 2 to 1,000,000
//
// // // // // // //


/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <locale.h>

/** ----------------------------- Constants ----------------------------- **/
int N = 1000000;

/** ----------------------------- Global ----------------------------- **/
int total_cons = 0;		//total consecutive

/** ----------------------------- Prototypes ---------------------------- **/
bool isPrime(int num);
void Usage();

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

    /*** Function Initialization ***/
    int prev_Val = 0;
    int next_Val = 0;

    clock_t start = clock();    	//program start time

    /*** Process Arguments ***/
    int input = 0;
    input = getopt( argc, argv,"hn:" );

    if (input != -1)
    {
        switch (input)
        {
            /* Change Port */
            case 'n':
                N = atoi(optarg);
                break;

            /* Access Help Menu */
            case 'h':

            default:
                Usage();
                exit(0);
        }
    }

    /* Iterate through 0 to N */
    for (int i = 0; i < N; i++) {

        /* Determine if value is Prime */
        if (isPrime(i)) {

            if (i != 2){
                next_Val = i;                               //save i as next

                /* Determine if Primes are Consecutive */
                if (i == 3 || next_Val - prev_Val == 2) {
                    total_cons++;                           //increase consecutive counter
                    //printf("%d: %d, %d\n", total_cons, prev_Val, next_Val);
                }
            }
                prev_Val = i;                               //save i as previous
        }

    }

    /* Print Results */
    if (total_cons == 1){
        printf("There is only 1 instance of consecutive primes for all integers less than %d.\n", N);
    }
    else{
        setlocale(LC_NUMERIC, "");
        printf("There are %d instances of consecutive primes for all integers less than %'d.\n", total_cons, N);
    }

    /* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);
	
	return 0;
}

/* ------------------------------------------------------------------------
  Name -            isPrime
  Purpose -         Determines if a Value is Prime
  Parameters -      num: an Integer to Determine if Prime
  Returns -         true:   Number is Prime
                    false:  Number is Not Prime
  Side Effects -    None
  ----------------------------------------------------------------------- */
bool isPrime(int num){

    int i;

    /* Verify Number is not 0 or 1 */
    if (num == 0 || num == 1){
        return false;
    }

    /* Determine if Number is Prime */
    for(i=2; i <= num / 2; i++){            //for each value (i) 2 to num/2

        if(num % i != 0)                    //if i does not divide evenly
            continue;                       //continue loop for remaining i
        else                                //else
            return false;                   //number is not prime
    }

    return true;                            //number is prime

}

/* ------------------------------------------------------------------------
  Name -            Usage
  Purpose -         Prints Help Menu
  Parameters -      None
  Returns -         None
  Side Effects -    None
  ----------------------------------------------------------------------- */
void Usage(){
    printf("\nHW2-1.c, A Sequential Program for finding Consecutive Primes\n"
           "ver 1.0, 2022\n"
           "Usage: HW2-1 -h -n max_values\n"
           "	-h: Display Usage summary\n"
           "	-n: Change max value threshold   Default: 1,000,000    |   example: -n 1000 \n\n");
}
