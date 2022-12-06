/* ------------------------------------------------------------------------
    proj_omp.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // //
// Program:
//
//	OMP
//
//	parse a large text file, query the data, return the number of occurrences
//	for a pre-specified string of characters
//
// // // // // // //


/* Standard Libraries */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>
#include <omp.h>

/** ----------------------------- Structures ---------------------------- **/


/** ----------------------------- Macros -------------------------------- **/
# define getName(var, str)  sprintf(str, "%d", var)

/** ----------------------------- Constants ----------------------------- **/
/* User Input Limits */
#define MAX_STR 100             //max searchTerm length
#define THREAD_LIMIT 10         //max number of threads

/* Default Settings */
#define DEFAULT_THREADS 5
#define DEFAULT_FILE "testFile.txt"
#define DEFAULT_TERM "life"
#define DEFAULT_CASE false
#define DEFAULT_VERBOSE false

// // // // //
// Note:
//      If you wish to modify the default settings,
//      change them here. The code uses these to update
//      the global/local variables.
//
//      DO NOT change settings under Global section or
//      any constants below this note
// // // // //

#define OUTPUT_FILE_SIZE 10     //output file name size. do not change


/** ----------------------------- Global -------------------------------- **/
char fileName[MAX_STR];         	//file to search
char searchTerm[MAX_STR];       	//term to search for

char threadFileNames[THREAD_LIMIT][OUTPUT_FILE_SIZE];
FILE* pThreadFiles[THREAD_LIMIT];         //unique pointer to file

bool caseSensitive;             	//flag: is search case-sensitive
bool verbose;                   	//flag: use verbose mode
bool defaults[2] = {true, true};    //flag: use default settings {file, searchTerm}

int numThreads = 0;                 //number of threads (value set in main)
double startTime, stopTime;

/** ----------------------------- Prototypes ---------------------------- **/
void Usage();
void ProcessArgs(int argc, char ** argv);
char* TrimLeft(char* str, const char* trimChars);
char* TrimRight(char* str, const char* trimChars);
char* ChangeToLower(char* str);
void FileSplitter();
/** ----------------------------- Functions ----------------------------- **/

/*****************************************************
 *
 *  Function:  main()
 *
 *  Parameters:
 *
 *      argc - main parameter for argument globalCount
 *      argv - main parameter for argument values
 *
******************************************************/
int main(int argc, char *argv[]) {

    /*** *** *** *** *** *** ***
    *  Program Initialization
    *** *** *** *** *** *** ***/
    int tid;
    int globalCount = 0;                //total occurrences of searchTerm in file
    //int localCount = 0;                 //occurrences of searchTerm by thread

    char word[MAX_STR];            //word currently being searched
    char tFileName[OUTPUT_FILE_SIZE];   //thread/temp file name
    FILE* pInputFile;                    //unique pointer to file

    memset(tFileName, 0, sizeof(tFileName));

    /* Save Program Start Time */
    startTime = omp_get_wtime();

    /* Set Defaults */
    numThreads = DEFAULT_THREADS;

    memcpy(fileName, DEFAULT_FILE, strlen(DEFAULT_FILE)+1);
    memcpy(searchTerm, DEFAULT_TERM, strlen(DEFAULT_TERM) + 1);

    caseSensitive = DEFAULT_CASE;
    verbose = DEFAULT_VERBOSE;

    /*** *** *** *** *** *** ***
     *    Parse User Input
     *** *** *** *** *** *** ***/
    ProcessArgs(argc,argv);

    /* Manage Case */
    if(!caseSensitive){                                         //if not case-sensitive
        ChangeToLower(searchTerm);                           //change to lowercase
    }

    /* Initialize Local Variables */
    globalCount = 0;                        //initialize globalCount

    /*** *** *** *** *** *** ***
     *    Split Input File
     *** *** *** *** *** *** ***/
    FileSplitter();

    /* Print Variables */
    printf("\n%d Threads are Searching \'%s\' for \'%s\'...\n\n",
           numThreads, fileName, searchTerm);
    fflush(stdout);


    /*** *** *** *** *** *** ***
     *  Begin Parallelization
     *** *** *** *** *** *** ***/
    /* Fork a team of 'numThreads' threads */
#pragma omp parallel shared(globalCount) private(tid, pInputFile, tFileName, word) num_threads(numThreads)
    {
        int tmpCount;               //saves updated count (for debugging/verbose)
        int localCount = 0;
        /* Set Thread ID */
        tid = omp_get_thread_num();

        if(verbose){
            /* Print Status Update */
            printf("Thread %d Starting...\n", tid);
            fflush(stdout);
        }

        /* Set Local Thread Variables */
        pInputFile = pThreadFiles[tid];
        memcpy(tFileName, threadFileNames[tid], strlen(threadFileNames[tid]));

        /* Parse Temp File */
        rewind(pInputFile);

        while (fscanf(pInputFile, "%s", word) == 1) {       //while still file data

            /* Compare to Target String */
            if(strcmp(searchTerm, word) == 0){                           //if strings match
#pragma omp critical
                {
                    globalCount++;                                                //increment globalCount
                    tmpCount = globalCount;
                }
                localCount++;                                           //increment localCount
            }
        }

        /* Close and Remove File */
        fclose(pInputFile);
        if(remove(tFileName) == -1){     //if removal fails
            // Display Error Message
            fprintf(stderr,
                    "Thread %d IO Error: Unable to remove temporary file %s\n<%s>\n",
                    tid, tFileName, strerror(errno));                               //print error message
            fflush(stderr);
        }

        /* Print Status Update if Verbose */
        if(verbose){
            printf("Thread %d has found %d matches, the total match count is now %d\n",
                   tid, localCount, tmpCount);
            fflush(stdout);
        }

    }/* End of parallel region */



    /*** *** *** *** *** *** ***
     *  Program Termination
     *** *** *** *** *** *** ***/
    /* Print Results */
    printf("\nThe word \'%s\' appears %d %s\n\n",
           searchTerm, globalCount, globalCount == 1 ? "time" : "times");
    fflush(stdout);

    /* Calculate Runtime */
    stopTime = omp_get_wtime();
    double time_spent = (double)(stopTime - startTime);
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);

    return 0;
}

/* ------------------------------------------------------------------------
  Name -            FileSplitter
  Purpose -         Reads in file and splits into n files
  Parameters -      none
  Returns -         none, exits on failure
  Side Effects -    creates n temp files on system
  ----------------------------------------------------------------------- */
void FileSplitter(){

    /*** *** *** *** *** *** ***
    *  Function Initialization
    *** *** *** *** *** *** ***/
    FILE* pInputFile;               //unique pointer to file
    char currentWord[MAX_STR];        //currentWord being searched

    memset(threadFileNames, 0, sizeof(threadFileNames));


    /*** *** *** *** *** *** ***
    *       Open Files
    *** *** *** *** *** *** ***/
    /* Open Input File */
    if ((pInputFile = fopen(fileName, "r") ) == NULL) {

        /* Print Error Message */
        fprintf(stderr,
                "Unable to open file \'%s\'\n[%d]: %s\n",
                fileName, errno, strerror(errno));
        fflush(stderr);
        exit (-1);
    }
    else if (verbose){
        printf("File \'%s\' opened successfully\n\n", fileName);
        fflush(stdout);
    }

    /* Open Output Files */
    for(int i = 0 ; i < numThreads; i++){

        /* Get File Name */
        char threadFile[OUTPUT_FILE_SIZE];                          //create variable
        memset(threadFile, 0, sizeof(threadFile));      //init variable
        getName(i, threadFile);                                     //create file named i
        //sprintf(threadFile, "%d", i)					//create file named i

        /* Open File */
        if ((pThreadFiles[i] = fopen(threadFile, "a+") ) == NULL) {

            /* Print Error Message */
            fprintf(stderr,
                    "Unable to open file \'%s\'\n[%d]: %s\n",
                    fileName, errno, strerror(errno));
            fflush(stderr);

            exit (-1);
        }
        else if (verbose){
            printf("Temp File \'%s\' created and opened successfully\n", threadFile);
            fflush(stdout);
        }


        /* Save Created File Name */
        memcpy(threadFileNames[i], threadFile, strlen(threadFile));
    }


    /*** *** *** *** *** *** ***
    *   Parse and Split File
    *** *** *** *** *** *** ***/
    int i = 0;
    while (fscanf(pInputFile, "%s", currentWord) == 1) {       //while still file data

        /* Clean Text Formatting */
        TrimRight(currentWord, "\t\n\v\f\r .,!?:;-""'");       //trim excess char (right)
        TrimLeft(currentWord, "\t\n\v\f\r -");                 //trim excess char (left)

        if(!caseSensitive){                                         //if not case-sensitive
            ChangeToLower(currentWord);                                //change to lowercase
        }

        fputs(currentWord, pThreadFiles[i]);            //write word to necessary file
        fputs(" ", pThreadFiles[i]);                    //write space

        i = (i+1) % numThreads;                                   //increment threads
    }


    /*** *** *** *** *** *** ***
     *     Close Input File
     *** *** *** *** *** *** ***/
    fclose(pInputFile);

    if (verbose){
        printf("\nFile \'%s\' parsed and closed successfully\n", fileName);
        fflush(stdout);
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
     *  Program Initialization
     *** *** *** *** *** *** ***/
    int input = 0;
    int threads = 0;

    /*** *** *** *** *** *** ***
     *     Process Arguments
     *** *** *** *** *** *** ***/
    /* Get Input */
    input = getopt( argc, argv,"cvt:hf:s:" );

    /* Handle if No Arguments */
    if (input == -1){
        printf("Note: Defaults Set [No Arguments Given]\n\n");        //inform user
        fflush(stdout);
        return;
    }

    /* Process All Arguments */
    while (input != -1) {
        switch (input) {

            /* Case Sensitive */
            case 'c':
                caseSensitive = true;
                break;

                /* Verbose Mode */
            case 'v':
                verbose = true;
                break;


            /* Specify Thread Amount */
            case 't':

                threads = atoi(optarg);     //set thread value

                if(threads<=THREAD_LIMIT){                 //verify thread range
                    numThreads = threads;
                }
                else{
                    fprintf(stderr,
                            "Invalid thread request <%s> - value must be between 0-10.\n "
                            "Defaulting to %d %s",
                            optarg, numThreads, numThreads == 1 ? "thread" : "threads" );		//print error message
                    fflush(stderr);

                }

                break;

                /* Specify File */
            case 'f':
                memcpy(fileName, optarg,
                       strlen(DEFAULT_FILE) >= strlen(optarg) ? strlen(DEFAULT_FILE)+1 : strlen(optarg)+1);
                fflush(stderr);

                break;

                /* Specify Target String */
            case 's':
                memcpy(searchTerm, optarg,
                       strlen(DEFAULT_TERM) >= strlen(optarg) ? strlen(DEFAULT_TERM)+1 :  strlen(optarg)+1 );
                fflush(stderr);
                break;

                /* Access Help Menu */
            case 'h':
            default:
                Usage();
                exit(0);
        }

        input = getopt( argc, argv, "cvt:hf:s:" );
    }
}


/* ------------------------------------------------------------------------
  Name -            TrimLeft
  Purpose -         Removes characters from the beginning of a string
  Parameters -      str:         pointer to string to update
                    trimChars:   characters to be removed [optional]
  Returns -         Pointer to updated string
  Side Effects -    none
  ----------------------------------------------------------------------- */
char* TrimLeft(char* str, const char* trimChars) {


    /*** *** *** *** *** *** ***
    *  Function Initialization
    *** *** *** *** *** *** ***/
    int i;
    char* newStartingPos;


    /*** *** *** *** *** *** *** ***
    *  Remove Specified Characters
    *** *** *** *** *** *** *** ***/
    /* Set Defaults */
    if (trimChars == NULL) {            //if trimChars is null
        trimChars = "\t\n\v\f\r ";
    }

    /* Set Variables */
    i = 0;
    newStartingPos = str;

    /* Loop While Specified Character Shows */
    while (i < strlen(str) && strchr(trimChars, str[i]) != NULL) {
        i++;                //increment index
        newStartingPos++;   //move start position once to the right
    }

    /*** *** *** *** *** *** ***
     *  Function Termination
     *** *** *** *** *** *** ***/
    /* Copy newStartingPos to str */
    strcpy(str, newStartingPos);

    /* Return Updated String */
    return str;

}


/* ------------------------------------------------------------------------
  Name -            TrimRight
  Purpose -         Removes characters from the end of a string
  Parameters -      str:         pointer to string to update
                    trimChars:   characters to be removed [optional]
  Returns -         Pointer to updated string
  Side Effects -    none
  ----------------------------------------------------------------------- */
char* TrimRight(char* str, const char* trimChars) {

    /*** *** *** *** *** *** ***
    *  Function Initialization
    *** *** *** *** *** *** ***/
    int i;


    /*** *** *** *** *** *** *** ***
    *  Remove Specified Characters
    *** *** *** *** *** *** *** ***/
    /* Set Defaults */
    if (trimChars == NULL) {            //if trimChars is null
        trimChars = "\t\n\v\f\r ";
    }

    /* Set Index */
    i = strlen(str) - 1;                        //move index to last char

    /* Loop While Specified Character Shows */
    while (i >= 0 && strchr(trimChars, str[i]) != NULL) {
        str[i] = '\0';      //replace character with null
        i--;                //decrement string
    }

    /*** *** *** *** *** *** ***
     *  Function Termination
     *** *** *** *** *** *** ***/
    /* Return Updated String */
    return str;
}


/* ------------------------------------------------------------------------
  Name -            ChangeToLower
  Purpose -         Changes input string to lower case
  Parameters -      str:         pointer to string to update
  Returns -         Pointer to updated string
  Side Effects -    none
  ----------------------------------------------------------------------- */
char* ChangeToLower(char* str) {

    /*** *** *** *** *** *** ***
    *  Function Initialization
    *** *** *** *** *** *** ***/
    int i = 0;


    /*** *** *** *** *** *** *** ***
    *  Update Specified Characters
    *** *** *** *** *** *** *** ***/
    /* Loop Entire String */
    while (i <= (strlen(str))) {
        str[i] = tolower(str[i]);   //change to lowercase, overwrite letter
        i++;                            //increment index
    }

    /*** *** *** *** *** *** ***
     *  Function Termination
     *** *** *** *** *** *** ***/
    /* Return Updated String */
    return str;
}

/* ------------------------------------------------------------------------
  Name -            Usage
  Purpose -         Prints Help Menu
  Parameters -      None
  Returns -         None
  Side Effects -    None
  ----------------------------------------------------------------------- */
void Usage(){

    printf("\nAn openMP Program for finding occurrences of a specified string in a large file\n"
           "ver 1.0, 2022\n\n"
           "Usage: proj_omp -h -c -v -t <1-10> -f <file> -s <string>\n\n"
           "\t%-14s %-24s %-16s\n\n"

           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n\n"

           "\t%-14s %-24s %-8s %-12d %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-2s\n\n",

           "-h:", "Display Usage summary", "Example: proj_seq -h",

           "-c:", "Set Case Sensitivity", "Default:", DEFAULT_CASE == true ? "True" : "False", "|", "Example: proj_seq -c",
           "-v:", "Set Verbose Mode", "Default:", DEFAULT_VERBOSE == true ? "True" : "False", "|", "Example: proj_seq -v",

           "-t <1-10>:", "Set Number of Threads", "Default:", DEFAULT_THREADS, "|", "Example: proj_seq -t 3",
           "-f <file>:", "Specify File", "Default:", DEFAULT_FILE, "|", "Example: proj_seq -f myFile.txt",
           "-s <string>:", "Specify Search Term", "Default:", DEFAULT_TERM, "|", "Example: proj_seq -s search_term");

}
