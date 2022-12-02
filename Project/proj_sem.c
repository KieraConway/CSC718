/* ------------------------------------------------------------------------
    Project - Semaphore.c

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
//	parse a large text file, query the data, return the number of occurrences 
//	for a pre-specified string of characters
//
// // // // // // //


/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>

#include <pthread.h>
#include <semaphore.h>
/** ----------------------------- Structures ---------------------------- **/


/** ----------------------------- Macros -------------------------------- **/


/** ----------------------------- Constants ----------------------------- **/
#define MAX_STR 100
#define THREAD_LIMIT 10

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
//      DO NOT change settings under Global section
// // // // //

/** ----------------------------- Global -------------------------------- **/
int globalCount;                    //occurrences of searchTerm in file
char fileName[MAX_STR];         	//file to search
char searchTerm[MAX_STR];       	//term to search for

bool caseSensitive;             	//flag: is search case-sensitive
bool verbose;                   	//flag: use verbose mode
bool defaults[2] = {true, true};    //flag: use default settings {file, searchTerm}

int maxThreads = 0;

sem_t mutex;                       //semaphore mutex
sem_t display;                     //semaphore display
sem_t counter;                     //semaphore counter

/** ----------------------------- Prototypes ---------------------------- **/
void Usage();
void ProcessArgs();
char* TrimLeft(char* str, const char* trimChars);
char* TrimRight(char* str, const char* trimChars);
char* ChangeToLower(char* str);
void ThreadHandler(int tid);

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

    /* Initialize Local Variables */
    globalCount = 0;                        //initialize counter

    /* Thread Variables */
    pthread_t threadIDs[maxThreads];		//tid IDS
    //int tid[maxThreads];				    //tid

    /* Save Program Start Time */
    clock_t start = clock();

    /* Set Defaults */
    maxThreads = DEFAULT_THREADS;

    memcpy(fileName, DEFAULT_FILE, strlen(DEFAULT_FILE)+1);
    memcpy(searchTerm, DEFAULT_TERM, strlen(DEFAULT_TERM) + 1);

    caseSensitive = DEFAULT_CASE;
    verbose = DEFAULT_VERBOSE;
	
    /* Semaphore Init */
    for (int i = 0; i < maxThreads; i++) {

        // Mutex Semaphore
        if ((sem_init(&mutex, 0, i)) < 0) {
            fprintf(stderr,
                    "Semaphore Error: Unable to create semaphore mutex\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }

        // Displaying to Screen Semaphore
        if ((sem_init(&display, 0, i)) < 0) {
            fprintf(stderr,
                    "Semaphore Error: Unable to create semaphore display\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }

        // Accepting Connections Semaphore
        if ((sem_init(&counter, 0, i)) < 0) {
            fprintf(stderr,
                    "Semaphore Error: Unable to create semaphore counter\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }
    }

    /*** *** *** *** *** *** ***
    *     Parse User Input
    *** *** *** *** *** *** ***/
    ProcessArgs(argc,argv);


    /* Manage Case */
    if(!caseSensitive){                                         //if not case-sensitive
       ChangeToLower(searchTerm);                           //change to lowercase
    }

    /* Print Variables */
    printf("\n%d Threads are Searching \'%s\' for \'%s\'...",
           maxThreads, fileName, searchTerm);


    /*** *** *** *** *** *** ***
    *   Begin Parallelization
    *** *** *** *** *** *** ***/

    for (int i = 0; i < maxThreads; i++) {
        //tid[i] = i;					        //save tid

        ////////////////////////////////////
        //// todo
        //// if thread id problem, update &i to i
        //// or update int tid to int * tid
        /////////////////////////////////////
        if (pthread_create(&threadIDs[i], NULL,
                           (void *(*)(void *)) &ThreadHandler,
                           &i) != 0) {                      //attempt tid creation

            // Display Error Message
            sem_wait(&display);                                     	//access display semaphore
            fprintf(stderr,
                    "Thread Error: Unable to create tid\n<%s>\n",
                    strerror(errno));                               //print error message
            sem_post(&display);                                     	//release display semaphore

            exit(-1);
        }
    }

    /*** *** *** *** *** *** ***
    *   Program Termination
    *** *** *** *** *** *** ***/

    /* Join Threads */
    for (int i = 0; i < maxThreads; i++) {
        pthread_join(threadIDs[i], NULL);

        /* If Verbose */
        if(verbose){
            /* Print Status Update */
            sem_wait(&display);							//access display semaphore
            printf("Thread %d has terminated successfully", threadIDs[i]);
            sem_post(&display);							//release display semaphore
        }
    }

    /* Release Semaphores */
    sem_destroy(&mutex);
    sem_destroy(&display);
    sem_destroy(&counter);

    /* Print Results */
    printf("\nThe word \'%s\' appears %d %s\n\n",
           searchTerm, globalCount, globalCount == 1 ? "time" : "times");

    /* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);

    return 0;
}
/* ------------------------------------------------------------------------
  Name -            ThreadHandler
  Purpose -         Handles each thread
  Parameters -      none
  Returns -         none, exits on failure
  Side Effects -    todo
  ----------------------------------------------------------------------- */
void ThreadHandler(int tid){

    /*** *** *** *** *** *** ***
    *  Function Initialization
    *** *** *** *** *** *** ***/
    FILE* pInputFile;               //unique pointer to file
    char localWord[MAX_STR];        //localWord currently being searched
    int localCount = 0;             //local counter
    /*
     * Open File
     */
    if ((pInputFile = fopen(fileName, "r") ) == NULL) {

        /* Print Error Message */
        sem_wait(&display);							//access display semaphore
        fprintf(stderr,
                "Unable to open file \'%s\'\n[%d]: %s\n",
                fileName, errno, strerror(errno));
        sem_post(&display);							//release display semaphore

        exit (-1);
    }

    /* If Verbose */
    if(verbose){
        /* Print Status Update */
        sem_wait(&display);							//access display semaphore
        printf("Thread %d has successfully opened %s\n "
               "Thread %d is searching for matches... ",
               tid, fileName, tid);
        sem_post(&display);							//release display semaphore
    }

    /*
     * Parse File
     */
    while (fscanf(pInputFile, "%s", localWord) == 1) {       //while still file data

        /* Clean Text Formatting */
        TrimRight(localWord, "\t\n\v\f\r .,!?:;-""'");       //trim excess char (right)
        TrimLeft(localWord, "\t\n\v\f\r -");                 //trim excess char (left)

        if(!caseSensitive){                                         //if not case-sensitive
            ChangeToLower(localWord);                                //change to lowercase
        }

        /* Compare to Target String */
        if(strcmp(searchTerm, localWord) == 0){                           //if strings match
            localCount++;                                                //increment counter
        }

    }

    /* Close File */
    fclose(pInputFile);

    /* If Verbose */
    if(verbose){
        /* Print Status Update */
        sem_wait(&display);							//access display semaphore
        printf("Thread %d has successfully completed searching %s for \'%s\'\n ",
               tid, fileName, searchTerm);
        sem_post(&display);							//release display semaphore
    }

    /* Update Global Count */
    sem_wait(&counter);							//access counter semaphore
    globalCount+=localCount;                            //add localCount to globalCount
    int tmpCount = globalCount;                         //obtains the new count value after update
    sem_post(&counter);							//release counter semaphore

    /* If Verbose */
    if(verbose){
        /* Print Status Update */
        sem_wait(&display);							//access display semaphore						//access display semaphore
        printf("Thread %d has found %d matches, the total match count is now %d",
               tid, localCount, tmpCount);
        sem_post(&display);							//release display semaphore
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

    printf("\nA Sequential Program for finding occurrences of a specified string in a large file\n"
           "ver 1.0, 2022\n\n"
           "Usage: proj_seq -h -c -v -t <1-10> -f <file> -s <string>\n"
           "\t%-14s %-24s %-16s\n"

           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"

           "\t%-14s %-24s %-8s %-12d %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-2s\n",

           "-h:", "Display Usage summary", "Example: proj_seq -h",

           "-c:", "Case Sensitive", "Default:", DEFAULT_CASE == true ? "True" : "False", "|", "Example: proj_seq -c",
           "-v:", "Use Verbose Mode", "Default:", DEFAULT_VERBOSE == true ? "True" : "False", "|", "Example: proj_seq -v",

           "-t <1-10>:", "Set Number of Threads", "Default:", DEFAULT_THREADS, "|", "Example: proj_seq -t 3",
           "-f <file>:", "Specify File", "Default:", DEFAULT_FILE, "|", "Example: proj_seq -f myFile.txt",
           "-s <string>:", "Specify Search Term", "Default:", DEFAULT_TERM, "|", "Example: proj_seq -s search_term");

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
    *    Process Arguments
    *** *** *** *** *** *** ***/
    /* Get Input */
    input = getopt( argc, argv,"hcvf:s:t:" );

    /* Handle if No Arguments */
    if (input == -1){
        Usage();            //print usage
        exit(0);        //exit
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

            /* Specify Thread Number */
            case 't':

                threads = atoi(optarg);     //set thread value

                if(threads<=THREAD_LIMIT){                 //verify thread range
                    maxThreads = threads;
                }
                else{
                    fprintf(stderr,
                            "Invalid thread request <%s> - value must be between 0-10.\n "
                            "Defaulting to %d %s",
                            optarg, maxThreads, maxThreads == 1 ? "thread" : "threads" );		//print error message
                }

                break;

            /* Specify File */
            case 'f':
                memcpy(fileName, optarg,
                       strlen(DEFAULT_FILE) >= strlen(optarg) ? strlen(DEFAULT_FILE)+1 : (strlen(optarg)+(strlen(DEFAULT_FILE) - strlen(optarg))));

                break;

            /* Specify Target String */
            case 's':
                memcpy(searchTerm, optarg,
                       strlen(DEFAULT_TERM) >= strlen(optarg) ? strlen(DEFAULT_TERM)+1 : (strlen(optarg)+(strlen(DEFAULT_TERM) - strlen(optarg))));
                break;

            /* Access Help Menu */
            case 'h':
            default:
                Usage();
                exit(0);
        }

        input = getopt( argc, argv, "hcvf:s:t:" );
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

    /*
     * Function Initialization
     */
    int i;
    char* newStartingPos;

    /*
     * Set defaults if trimChars is null
     */
    if (trimChars == NULL) {
        trimChars = "\t\n\v\f\r ";
    }

    /*
     * Remove Specified Characters
     */

    /* Set Variables */
    i = 0;
    newStartingPos = str;

    /* Loop While Specified Character Shows */
    while (i < strlen(str) && strchr(trimChars, str[i]) != NULL) {
        i++;                //increment index
        newStartingPos++;   //move start position once to the right
    }

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

    /*
     * Function Initialization
     */
    int i;

    /*
     * Set defaults if trimChars is null
     */
    if (trimChars == NULL) {
        trimChars = "\t\n\v\f\r ";
    }

    /*
     * Remove Specified Characters
     */
    i = strlen(str) - 1;                        //set index to end of string

    /* Loop While Specified Character Shows */
    while (i >= 0 && strchr(trimChars, str[i]) != NULL) {
        str[i] = '\0';      //replace character with null
        i--;                //decrement string
    }

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

    /*
     * Function Initialization
     */
    int i = 0;


    /*
     * Loop Entire String
     */
    while (i <= (strlen(str))) {
        str[i] = tolower(str[i]);   //change to lowercase, overwrite letter
        i++;                            //increment index
    }


    /* Return Updated String */
    return str;

}