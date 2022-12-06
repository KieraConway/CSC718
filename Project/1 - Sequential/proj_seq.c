/* ------------------------------------------------------------------------
    proj_seq.c

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
//	for a pre-specified search term
//
// // // // // // //


/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <errno.h>
/** ----------------------------- Structures ---------------------------- **/


/** ----------------------------- Macros -------------------------------- **/


/** ----------------------------- Constants ----------------------------- **/
/* User Input Limits */
#define MAX_STR 100             //max searchTerm length

/* Default Settings */
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

/** ----------------------------- Global -------------------------------- **/
int count;                      //occurrences of searchTerm in file
char fileName[MAX_STR];         //file to search
char searchTerm[MAX_STR];       //term to search for

bool caseSensitive;             	//flag: is search case-sensitive
bool verbose;                   	//flag: use verbose mode


/** ----------------------------- Prototypes ---------------------------- **/
void Usage();
void ProcessArgs(int argc, char ** argv);
char* TrimLeft(char* str, const char* trimChars);
char* TrimRight(char* str, const char* trimChars);
char* ChangeToLower(char* str);

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
    FILE* pInputFile;               //pointer to file
    char word[MAX_STR];             //word currently being searched

    count = 0;                      //initialize counter

    /* Save Program Start Time */
    clock_t start = clock();

    /* Set Defaults */
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


    /* Print Variables */
    printf("\nSearching \'%s\' for \'%s\'...\n\n", fileName, searchTerm);
    fflush(stdout);

    /*** *** *** *** *** *** ***
     *         Open File
     *** *** *** *** *** *** ***/

    if ((pInputFile = fopen(fileName, "r") ) == NULL) {
        fprintf(stderr,
                "Unable to open file \'%s\'\n[%d]: %s\n",
                fileName, errno, strerror(errno));
        return (-1);
    }
    else if(verbose){
        printf("File \'%s\' has opened successfully\n"
               "Beginning File Parsing ...\n\n",
               fileName);
        fflush(stdout);
    }

    /*** *** *** *** *** *** ***
     *        Parse File
     *** *** *** *** *** *** ***/
    while (fscanf(pInputFile, "%s", word) == 1) {       //while still file data

        /* Clean Text Formatting */
        TrimRight(word, "\t\n\v\f\r .,!?:;-""'");       //trim excess char (right)
        TrimLeft(word, "\t\n\v\f\r -");                 //trim excess char (left)

        if (!caseSensitive) {                             //if not case-sensitive
            ChangeToLower(word);                        //change to lowercase
            ChangeToLower(searchTerm);                  //change to lowercase
        }

        /* Compare to Target String */
        if (strcmp(searchTerm, word) == 0) {              //if strings match
            count++;                                    //increment counter
        }
    }

    if(verbose){
        printf("File \'%s\' was parsed successfully\n",
               fileName);
        fflush(stdout);
    }

    /*** *** *** *** *** *** ***
     *  Program Termination
     *** *** *** *** *** *** ***/
    /* Close File */
    fclose(pInputFile);

    /* Print Results */
    printf("\nThe word \'%s\' appears %d %s\n\n",
           searchTerm, count, count == 1 ? "time" : "times");

    /* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n<Program Runtime: %.4fs>\n\n", time_spent);

    return 0;
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

    printf("\nA Sequential Program for finding occurrences of a specified search term in a large file\n"
           "ver 1.0, 2022\n\n"
           "Usage: proj_seq -h -c -v -f <file> -s <string>\n\n"
           "\t%-14s %-24s %-16s\n\n"

           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n\n"

           "\t%-14s %-24s %-8s %-12s %-3s %-5s\n"
           "\t%-14s %-24s %-8s %-12s %-3s %-2s\n\n",

           "-h:", "Display Usage summary", "Example: proj_seq -h",

           "-c:", "Set Case Sensitivity", "Default:", DEFAULT_CASE == true ? "True" : "False", "|", "Example: proj_seq -c",
           "-v:", "Set Verbose Mode", "Default:", DEFAULT_VERBOSE == true ? "True" : "False", "|", "Example: proj_seq -v",

           "-f <file>:", "Specify File", "Default:", DEFAULT_FILE, "|", "Example: proj_seq -f myFile.txt",
           "-s <string>:", "Specify Search Term", "Default:", DEFAULT_TERM, "|", "Example: proj_seq -s search_term");

}
