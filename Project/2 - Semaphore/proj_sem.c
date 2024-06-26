/* ------------------------------------------------------------------------
    proj_sem.c

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

------------------------------------------------------------------------ */

// // // // // // //
// Program:
//
//	Semaphore
//
//	parse a large text file, query the data, return the number of occurrences
//	for a pre-specified search term
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
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
/** ----------------------------- Structures ---------------------------- **/

/** ----------------------------- Macros -------------------------------- **/

/** ----------------------------- Constants ----------------------------- **/
/* User Input Limits */
#define MAX_STR 100						//max searchTerm length
#define THREAD_LIMIT 10					//max number of threads

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

#define OUTPUT_FILE_SIZE 10			//output file name size. do not change

/** ----------------------------- Global -------------------------------- **/
int globalCount;					//occurrences of searchTerm in file
char fileName[MAX_STR];				//file to search
char searchTerm[MAX_STR];			//term to search for

char threadFileNames[THREAD_LIMIT][OUTPUT_FILE_SIZE];
FILE* pThreadFiles[THREAD_LIMIT];	//unique pointer to file

bool caseSensitive;					//flag: is search case-sensitive
bool verbose;						//flag: use verbose mode


int maxThreads = 0;

sem_t mutex;						//semaphore mutex
sem_t display;						//semaphore display
sem_t counter;						//semaphore counter

/** ----------------------------- Prototypes ---------------------------- **/
void Usage();
void ProcessArgs(int argc, char ** argv);
char* TrimLeft(char* str, const char* trimChars);
char* TrimRight(char* str, const char* trimChars);
char* ChangeToLower(char* str);
void ThreadHandler(int tid);
void FileSplitter();
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

    /* Save Program Start Time */
    clock_t start = clock();

    /* Set Defaults */
    maxThreads = DEFAULT_THREADS;

    memcpy(fileName, DEFAULT_FILE, strlen(DEFAULT_FILE)+1);
    memcpy(searchTerm, DEFAULT_TERM, strlen(DEFAULT_TERM) + 1);

    caseSensitive = DEFAULT_CASE;
    verbose = DEFAULT_VERBOSE;

    /*** *** *** *** *** *** ***
     *    Parse User Input
     *** *** *** *** *** *** ***/
    ProcessArgs(argc,argv);

    /* Manage Case */
    if(!caseSensitive){						//if not case-sensitive
        ChangeToLower(searchTerm);			//change to lowercase
    }

    /* Initialize Local Variables */
    globalCount = 0;						//initialize counter


    /* Thread Variables */
    pthread_t threadIDs[maxThreads];


    /*** *** *** *** *** *** ***
     *  Initialize Semaphores
     *** *** *** *** *** *** ***/
    for (int i = 0; i < maxThreads; i++) {

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
     *    Split Input File
     *** *** *** *** *** *** ***/
    FileSplitter();

    /* Print Variables */
    printf("\n%d Threads are Searching \'%s\' for \'%s\'...\n",
           maxThreads, fileName, searchTerm);
    fflush(stdout);


    /*** *** *** *** *** *** ***
     *  Begin Parallelization
     *** *** *** *** *** *** ***/
    /* Save Parse Start Time */
    clock_t parseStart = clock();

    for (int i = 0; i < maxThreads; i++) {

        if (pthread_create(&threadIDs[i], NULL,
                           (void *(*)(void *)) &ThreadHandler,
                           i) != 0) {					//attempt tid creation

            // Display Error Message
            sem_wait(&display);							//access display semaphore
            fprintf(stderr,
                    "Thread Error: Unable to create tid\n<%s>\n",
                    strerror(errno));					//print error message
            fflush(stderr);
            sem_post(&display);							//release display semaphore

            exit(-1);
        }
    }


    /*** *** *** *** *** *** ***
     *  Program Termination
     *** *** *** *** *** *** ***/
    /* Join Threads */
    for (int i = 0; i < maxThreads; i++) {
        pthread_join(threadIDs[i], NULL);

        /* If Verbose */
        if(verbose){
            /* Print Status Update */
            sem_wait(&display);							//access display semaphore
            printf("Thread %d has terminated successfully\n", i);
            fflush(stdout);
            sem_post(&display);							//release display semaphore
        }
    }

    /* Release Semaphores */
    sem_destroy(&display);
    sem_destroy(&counter);

    /* Print Results */
    printf("The word \'%s\' appears %d %s\n\n",
           searchTerm, globalCount, globalCount == 1 ? "time" : "times");
    fflush(stdout);

    /* Calculate Runtime */
    clock_t end = clock();
    double programTime = (double)(end - start) / CLOCKS_PER_SEC; //calculate program runtime
    double parseTime = (double)(end - parseStart) / CLOCKS_PER_SEC; //calculate parse runtime

    printf("\n%-1s %-10s %-5.4fs %-1s %-10s %-5.4fs %-1s\n\n",
           "<","Program Runtime:", programTime, "|", "File Parse Time:", parseTime,">");



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
    FILE* pInputFile = pThreadFiles[tid];			//unique pointer to file
    char localWord[MAX_STR];						//localWord currently being searched
    int localCount = 0;								//local counter
    char tFileName[OUTPUT_FILE_SIZE];				//thread/temp file name

    memset(tFileName, 0, sizeof(tFileName));
    memcpy(tFileName, threadFileNames[tid], strlen(threadFileNames[tid]));

    /* If Verbose */
    if(verbose){
        /* Print Status Update */
        sem_wait(&display);							//access display semaphore
        printf("Thread %d Starting...\n", tid);
        fflush(stdout);
        sem_post(&display);							//release display semaphore
    }


    /*** *** *** *** *** *** ***
     *     Parse Temp File
     *** *** *** *** *** *** ***/
    rewind(pInputFile);
    while (fscanf(pInputFile, "%s", localWord) == 1) {	//while still file data

        /* Compare to Target String */
        if(strcmp(searchTerm, localWord) == 0){		//if strings match
            localCount++;							//increment counter
        }
    }


    /*** *** *** *** *** *** ***
     *  Close and Remove File
     *** *** *** *** *** *** ***/
    fclose(pInputFile);

    if(remove(tFileName) == -1){					//if removal fails
        // Display Error Message
        sem_wait(&display);							//access display semaphore
        fprintf(stderr,
                "Thread %d IO Error: Unable to remove temporary file %s\n<%s>\n",
                tid, tFileName, strerror(errno));	//print error message
        fflush(stderr);
        sem_post(&display);							//release display semaphore
    }


    /*** *** *** *** *** *** ***
     *  Function Termination
     *** *** *** *** *** *** ***/
    /* Update Global Count */
    sem_wait(&counter);								//access counter semaphore
    globalCount+=localCount;						//add localCount to globalCount
    int tmpCount = globalCount;						//obtains the new count value after update
    sem_post(&counter);								//release counter semaphore

    /* If Verbose */
    if(verbose){
        /* Print Status Update */
        sem_wait(&display);							//access display semaphore
        printf("Thread %d has found %d matches, the total match count is now %d\n",
               tid, localCount, tmpCount);
        fflush(stdout);
        sem_post(&display);							//release display semaphore
    }
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
    FILE* pInputFile;								//unique pointer to file
    char currentWord[MAX_STR];						//currentWord being searched

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
    for(int i = 0 ; i < maxThreads; i++){

        /* Get File Name */
        char threadFile[OUTPUT_FILE_SIZE];			//create variable
        memset(threadFile, 0, sizeof(threadFile));	//init variable
        sprintf(threadFile, "%d", i);					//create file named i
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
    while (fscanf(pInputFile, "%s", currentWord) == 1) {	//while still file data

        /* Clean Text Formatting */
        TrimRight(currentWord, "\t\n\v\f\r .,!?:;-""'");	//trim excess char (right)
        TrimLeft(currentWord, "\t\n\v\f\r -");				//trim excess char (left)

        if(!caseSensitive){									//if not case-sensitive
            ChangeToLower(currentWord);						//change to lowercase
        }

        fputs(currentWord, pThreadFiles[i]);				//write word to necessary file
        fputs(" ", pThreadFiles[i]);						//write space

        i = (i+1)%maxThreads;								//increment threads
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
        printf("Note: Defaults Set [No Arguments Given]\n\n");
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

                threads = atoi(optarg);					//set thread value

                if(threads<=THREAD_LIMIT){				//verify thread range
                    maxThreads = threads;
                }
                else{
                    // Print Error Message
                    fprintf(stderr,
                            "Invalid thread request <%s> - value must be between 0-10.\n "
                            "Defaulting to %d %s",
                            optarg, maxThreads, maxThreads == 1 ? "thread" : "threads" );
                    fflush(stderr);

                }

                break;

			/* Specify File */
            case 'f':
                memcpy(fileName, optarg,
                       strlen(DEFAULT_FILE) >= strlen(optarg) ? strlen(DEFAULT_FILE)+1 : strlen(optarg)+1);

                break;

			/* Specify Target String */
            case 's':
                memcpy(searchTerm, optarg,
                       strlen(DEFAULT_TERM) >= strlen(optarg) ? strlen(DEFAULT_TERM)+1 :  strlen(optarg)+1 );

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
    if (trimChars == NULL) {
        trimChars = "\t\n\v\f\r ";
    }

    /* Set Variables */
    i = 0;
    newStartingPos = str;

    /* Loop While Specified Character Shows */
    while (i < strlen(str) && strchr(trimChars, str[i]) != NULL) {
        i++;                		//increment index
        newStartingPos++;   		//move start position once to the right
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
    if (trimChars == NULL) {
        trimChars = "\t\n\v\f\r ";
    }

    /* Set Index */
    i = strlen(str) - 1;			//move index to last char

    /* Loop While Specified Character Shows */
    while (i >= 0 && strchr(trimChars, str[i]) != NULL) {
        str[i] = '\0';				//replace character with null
        i--;						//decrement string
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
        str[i] = tolower(str[i]);	//change to lowercase, overwrite letter
        i++;						//increment index
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

    printf("\nA Multithreaded Program for finding occurrences of a specified search term in a large file\n"
           "ver 1.0, 2022\n\n"
           "Usage: proj_sem -h -c -v -t <1-10> -f <file> -s <string>\n\n"
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
