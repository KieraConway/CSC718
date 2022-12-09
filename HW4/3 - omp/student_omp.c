/* ------------------------------------------------------------------------
    student_omp.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // //
// Program:
//
//	openMP
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
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <omp.h>
#include <getopt.h>

/** ----------------------------- Structures ---------------------------- **/

/** ----------------------------- Macros -------------------------------- **/

/** ----------------------------- Constants ----------------------------- **/
#define MAX_ID 999999
#define THREAD_LOW 1
#define THREAD_MAX 10

/** ----------------------------- Global -------------------------------- **/
int maxThreads = 5;     //number of threads [default:5]
bool verbose = false;
/** ----------------------------- Prototypes ---------------------------- **/
bool ContainsConsecutiveValues(int studentID);
bool ComputesInvalidSum(int studentID);
void ProcessArgs(int argc, char ** argv);
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
    /*** *** *** *** *** *** ***
    *  Program Initialization
    *** *** *** *** *** *** ***/
    /* Program Initialization */
    int sid = 100000;               //starts here since 0 cannot be first value
    int count = 0;		            //total number of valid sids

    int localSID;
    int tid, nThreads;
    double start, stop;

    /* Save Program Start Time */
    start = omp_get_wtime();	//program start time
    
    /*** *** *** *** *** *** ***
     *    Parse User Input
     *** *** *** *** *** *** ***/
    ProcessArgs(argc,argv);
    
#pragma omp parallel shared(nThreads) private(tid) num_threads(maxThreads)
{

    /*** *** *** *** *** *** ***
     * Print Thread Information
     *** *** *** *** *** *** ***/
    tid = omp_get_thread_num();

    if (tid == 0 && verbose) {
        nThreads = omp_get_num_threads();
        printf("Number of threads = %d\n", nThreads);
    }

    if(verbose){
        printf("Thread %d starting...\n",tid);
    }


    /*** *** *** *** *** *** ***
     *     Find Valid SIDs
     *** *** *** *** *** *** ***/
#pragma omp for reduction(+:count)
    {
        for(localSID = sid; localSID <= MAX_ID; localSID++){

            if((!ContainsConsecutiveValues(localSID)) && (!ComputesInvalidSum(localSID))){
                count = count + 1;
            }
        }
    }

}

    /*** *** *** *** *** *** ***
     *  Program Termination
     *** *** *** *** *** *** ***/
    /* Print Results */
    setlocale(LC_NUMERIC, "");
    printf("There are %'d valid student IDs\n", count);

    /* Calculate Runtime */
    stop = omp_get_wtime();
    double programTime = (double)(stop - start);  //calculate program runtime

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


/* ------------------------------------------------------------------------
  Name -            ProcessArgs
  Purpose -         Process user arguments
  Parameters -      argc:   number of arguments passed to program
                    argv:   pointer to arguments passed to program
  Returns -         None
  Side Effects -    When necessary, updates global variables
  ----------------------------------------------------------------------- */
void ProcessArgs(int argc, char ** argv){

    /*** *** *** *** *** *** ***
      *     Process Arguments
      *** *** *** *** *** *** ***/
    /* Get Input */
    int input = 0;
    int threads = 0;

    input = getopt( argc, argv,"hvt:" );

    /* Handle if No Arguments */
    if (input == -1){
        printf("Note: Defaults Set [No Arguments Given]\n\n");
        fflush(stdout);
        return;
    }

    /* Process All Arguments */
    while (input != -1) {
        switch (input) {

            /* Verbose Mode */
            case 'v':
                verbose = true;
                break;

            /*Specify Thread Amount */
            case 't':

                threads = atoi(optarg);     //set thread value

                if(threads > THREAD_LOW && threads < THREAD_MAX){                 //verify thread range
                    maxThreads = threads;
                }
                else{
                    fprintf(stderr,
                            "Invalid thread request <%s> - value must be between 0-10.\n "
                            "Defaulting to %d %s",
                            optarg, maxThreads, maxThreads == 1 ? "thread" : "threads" );		//print error message
                }

                break;

            /* Access Help Menu */
            case 'h':
            default:
                Usage();
                exit(0);
        }

        input = getopt( argc, argv, "hvt:" );
    }
}


/* ------------------------------------------------------------------------
  Name -            Usage
  Purpose -         Prints Help Menu
  Parameters -      None
  Returns -         None
  Side Effects -    None
  ----------------------------------------------------------------------- */
void Usage(){

    printf("\nAn openMP Program for counting 'acceptable' unique student id numbers\n"
           "ver 1.0, 2022\n\n"
           "Usage: student_omp -h -v -t <nThreads>\n\n"
           "\t%-14s %-24s %-16s\n\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12d %-3s %-5s\n",

           "-h:", "Display Usage summary", "Example: proj_seq -h",
           "-v:", "Set Verbose Mode", "Default:", verbose == true ? "True" : "False", "|", "Example: proj_seq -v",
           "-t <nThreads>:", "Specify File", "Default:", maxThreads, "|", "Example: proj_seq -f myFile.txt");
}

