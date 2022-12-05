/* ------------------------------------------------------------------------
    Project - Sequential.c

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
#include <ctype.h>
#include <getopt.h>
#include <time.h>
#include <string.h>

/** ----------------------------- Structures ---------------------------- **/


/** ----------------------------- Macros -------------------------------- **/


/** ----------------------------- Constants ----------------------------- **/
#define MAX_STR 100

/** ----------------------------- Global -------------------------------- **/
int count;                      //occurrences of searchTerm in file
char fileName[MAX_STR];         //file to search
char searchTerm[MAX_STR];       //term to search for
bool caseSensitive = false;     //flag: is search case-sensitive
bool defaults = false;          //flag: use default settings (for testing)

/** ----------------------------- Prototypes ---------------------------- **/
void Usage();
void ProcessArgs();
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

    /*
     * Program Initialization
     */

    /* Initialize Local Variables */
    FILE* pInputFile;               //pointer to file
    char word[MAX_STR];             //word currently being searched

    count = 0;                      //initialize counter

    /* Save Program Start Time */
    clock_t start = clock();

    /*
     * Parse User Input
     */
    ProcessArgs(argc,argv);

    /* Set Defaults */
    if(defaults){               	//if defaults flag is set

        memcpy(fileName, "testFile.txt", strlen("testFile.txt")+1);
        memcpy(searchTerm, "life", strlen("life") + 1);

    }

    /* Print Variables */
    printf("\nSearching \'%s\' for \'%s\'...", fileName, searchTerm);

    /*
     * Open File
     */
    if ((pInputFile = fopen(fileName, "r") ) == NULL) {
        fprintf(stderr,
                "Unable to open file \'%s\'\n[%d]: %s\n",
                fileName, errno, strerror(errno));
        return (-1);
    }

    /*
     * Parse File
     */
    while (fscanf(pInputFile, "%s", word) == 1) {       //while still file data

        /* Clean Text Formatting */
        TrimRight(word, "\t\n\v\f\r .,!?:;-""'");       //trim excess char (right)
        TrimLeft(word, "\t\n\v\f\r -");                 //trim excess char (left)

        if(!caseSensitive){                             //if not case-sensitive
            ChangeToLower(word);                        //change to lowercase
            ChangeToLower(searchTerm);                  //change to lowercase
        }

        /* Compare to Target String */
        if(strcmp(searchTerm, word) == 0){              //if strings match
            count++;                                    //increment counter
        }

    }

    /*
    * Function Termination
    */

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
  Name -            Usage
  Purpose -         Prints Help Menu
  Parameters -      None
  Returns -         None
  Side Effects -    None
  ----------------------------------------------------------------------- */
void Usage(){
    printf("\nA Sequential Program for finding occurrences of a specified string in a large file\n"
           "ver 1.0, 2022\n\n"
           "Usage: proj_seq -h -d -c -f <file> -s \"string\"\n"
           "\t%-16s Display Usage summary \n"
           "\t%-16s Use Default Settings [\'testFile.txt\',\'life\'] \tExample: proj_seq -d\n"
           "\t%-16s Case Sensitive [Default: False]\t\tExample: proj_seq -c\n\n"
           "\t%-16s Specify File\t\tExample: proj_seq -f myFile.txt\n"
           "\t%-16s Specify Search Term\tExample: proj_seq -s \"term\"\n",
           "-h:","-d:", "-c:", "-f <file>:", "-s \"string\":");
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

    /*** Process Arguments ***/
    /* Get Input */
    int input = 0;
    input = getopt( argc, argv,"dhcf:s:" );

    /* Handle if No Arguments */
    if (input == -1){
        Usage();            //print usage
        exit(0);        	//exit
    }

    /* Process All Arguments */
    while (input != -1) {
        switch (input) {

            /* Default Flag */
            case 'd':
                defaults = true;
                break;

            /* Case Sensitive */
            case 'c':
                caseSensitive = true;
                break;

            /* Specify File */
            case 'f':
                memcpy(fileName, optarg, strlen(optarg)+1);
                break;

            /* Specify Target String */
            case 's':
                memcpy(searchTerm, optarg, strlen(optarg) + 1);
                break;

            /* Access Help Menu */
            case 'h':
            default:
                Usage();
                exit(0);
        }

        input = getopt( argc, argv, "dhcf:s:" );
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
        i++;                		//increment index
        newStartingPos++;   		//move start position once to the right
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
    i = strlen(str) - 1;				//set index to end of string

    /* Loop While Specified Character Shows */
    while (i >= 0 && strchr(trimChars, str[i]) != NULL) {
        str[i] = '\0';      			//replace character with null
        i--;                			//decrement string
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
        str[i] = tolower(str[i]);		//change to lowercase, overwrite letter
        i++;                            //increment index
    }


    /* Return Updated String */
    return str;

}