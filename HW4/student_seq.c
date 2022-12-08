/* ------------------------------------------------------------------------
    student_seq.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // //
// Program:
//
//	Sequential
//
//	counts 'acceptable' unique student id numbers
//
//  'acceptable' student ids:
//	    are a six-digit combination of the numerals 0-9
//	    do not contain 0 as the first digit
//      do not contain two consecutive digits of the same value
//	    do not sum to 7, 11, or 13
//          (i.e. 13126 would be invalid as it sums to 13)
// // // // // // //


/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>

/** ----------------------------- Structures ---------------------------- **/

/** ----------------------------- Macros -------------------------------- **/

/** ----------------------------- Constants ----------------------------- **/
#define MAX_ID 999999

/** ----------------------------- Global -------------------------------- **/

/** ----------------------------- Prototypes ---------------------------- **/
bool ContainsConsecutiveValues(int studentID);
bool ComputesInvalidSum(int studentID);

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
    int sid = 100000;               //starts here since 0 cannot be first value
    int count = 0;                  //number of valid sids

    /* Save Program Start Time */
    clock_t start = clock();

    /*** *** *** *** *** *** ***
     *     Find Valid SIDs
     *** *** *** *** *** *** ***/
    while(sid != MAX_ID){

        if((!ContainsConsecutiveValues(sid)) && (!ComputesInvalidSum(sid))){
            count += 1;
        }
        sid+=1;
    }

    /*** *** *** *** *** *** ***
     *  Program Termination
     *** *** *** *** *** *** ***/
    /* Print Results */
    setlocale(LC_NUMERIC, "");
    printf("There are %'d valid student IDs\n", count);

    /* Calculate Runtime */
    clock_t end = clock();
    double programTime = (double)(end - start) / CLOCKS_PER_SEC; //calculate program runtime

    printf("\n%-1s %-10s %-5.4fs %-1s\n\n",
           "<","Program Runtime:", programTime, ">");


    return 0;
}


/* ------------------------------------------------------------------------
  Name -            ContainsConsecutiveValues
  Purpose -         Checks if studentID contains any consecutive values
  Parameters -      studentID:  ID to check
  Returns -         true:       sid contains consecutive values
                    false:      sid does not contain consecutive values
  Side Effects -    none
  ----------------------------------------------------------------------- */
bool ContainsConsecutiveValues(int studentID){

    int digit, prevDigit;
    bool firstIteration = true;

    /* Iterate StudentID */
    while (studentID != 0)
    {
        digit = studentID % 10;     //obtain far right value

        if(!firstIteration && prevDigit == digit){  //compare to previous value
                return true;        //return true if values match
        }

        prevDigit = digit;          //save new 'previous value'
        firstIteration = false;     //switch flag
        studentID /= 10;            //Increment one digit left
    }

    return false;
}


/* ------------------------------------------------------------------------
  Name -            ComputesInvalidSum
  Purpose -         Checks if studentID computes 7, 11, or 13
  Parameters -      studentID:  ID to check
  Returns -         true:       sid computes 7, 11, or 13
                    false:      sid does not compute 7, 11, or 13
  Side Effects -    none
  ----------------------------------------------------------------------- */
bool ComputesInvalidSum(int studentID){
    int sum = 0, digit;

    /* Calculate Sum */
    while (studentID != 0)
    {
        digit = studentID % 10;     //obtain far right value
        sum += digit;               //add digit to sum
        studentID /= 10;            //Increment one digit left
    }

    /* Verify Sum is Acceptable */
    if (sum == 7 ||  sum ==11 || sum ==13){     //check sum value
        return true;                //return true if sum is 7, 11, or 13
    }
    else{
        return false;               //otherwise, return false
    }
}
