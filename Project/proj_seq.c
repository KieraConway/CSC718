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
#include <stdio.h>
#include <stdlib.h>
/** ----------------------------- Structures ---------------------------- **/


/** ----------------------------- Macros -------------------------------- **/


/** ----------------------------- Constants ----------------------------- **/
#define MAX_STR 100
#define MAX_CHUNK 512

/** ----------------------------- Global -------------------------------- **/
int count;
char fileName[MAX_STR];
char targetStr[MAX_STR];
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
    FILE* pInputFile;
    char word[MAX_STR];

    count = 0;                      //initialize counter

    /* Save Program Start Time */
    clock_t start = clock();

    /*
     * Parse User Input
     */
    ProcessArgs(argc,argv);

    /* Set Defaults */
    if(defaults){               //if default flag set

        memcpy(fileName, "testFile.txt", strlen("testFile.txt")+1);
        memcpy(targetStr, "life", strlen("life")+1);

    }



    //check if argc > 1
        //fileName = argv[1]
        //targetStr = argv[2]
    // else Process Arguments

    //set fileName and SearchString
    //memcpy(fileName, "testFile.txt", strlen(src)+1);
    //memcpy(targetStr, "life", strlen(src)+1);

    //printf( "Searching for {} in {})
    printf("\nSearching \'%s\' for \'%s\'...", fileName, targetStr);

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

        if(!caseSensitive){                                         //if not case-sensitive
            ChangeToLower(word);                                //change to lowercase
            ChangeToLower(targetStr);                           //change to lowercase
        }

        /* Compare to Target String */
        if(strcmp(targetStr, word) == 0){                           //if strings match
            count++;                                                //increment counter
        }

    }

    /*
    * Function Termination
    */

    /* Close File */
    fclose(pInputFile);

    /* Print Results */
    printf("\nThe word \'%s\' appears %d %s\n\n", targetStr,count, count == 1 ? "time" : "times");

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
  Parameters -      None todo!
  Returns -         None
  Side Effects -    None
  ----------------------------------------------------------------------- */
void ProcessArgs(int argc, char ** argv){

    /*** Process Arguments ***/
    /* Get Input */
    int input = 0;
    input = getopt( argc, argv,"dhcf:s:" );

    /* No Arguments */
    if (input == -1){
        Usage();            //print usage
        exit(0);        //exit
    }

    /* Process All Arguments */
    while (input != -1) {
        switch (input) {

            /* Default Flag */
            case 'd':
                defaults = true;
                break;

            /* Specify File */
            case 'f':
                memcpy(fileName, optarg, strlen(optarg)+1);
                break;

            /* Specify Target String */
            case 's':
                memcpy(targetStr, optarg, strlen(optarg)+1);
                break;

            /* Case Sensitive */
            case 'c':
                caseSensitive = true;
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

/**************************************************
*
* TrimLeft()
*
*   Removes characters from the beginning of a string,
*   defaults to whitespace charactersis trimChars
*   is null
*
**************************************************/
char* TrimLeft(char* str, const char* trimChars) {
    int i;
    char* newStartingPos;
    //int maxBufferSize = (strlen(str) + 1);	//took out, couldn't get gcc to work with strcpy_S

    //check for these characters//
    if (trimChars == NULL) {
        trimChars = "\t\n\v\f\r ";
    }

    //identify variables//
    i = 0;
    newStartingPos = str;

    //loop each time above character shows, move start position once to the right//
    while (i < strlen(str) && strchr(trimChars, str[i]) != NULL) {
        i++;
        newStartingPos++;
    }

    //copy newStartingPos to str//
    //strcpy_s(str, maxBufferSize, newStartingPos);	//wasn't working with gcc
    strcpy(str, newStartingPos);

    return str;

}

/**************************************************
 *
 * TrimRight()
 *
 *  Removes characters from the end of a string,
 *  defaults to whitespace
 *  characters is trimChars is null.
 *
**************************************************/
char* TrimRight(char* str, const char* trimChars) {
    int i;

    if (trimChars == NULL) {
        trimChars = "\t\n\v\f\r ";
    }

    i = strlen(str) - 1;
    while (i >= 0 && strchr(trimChars, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}

/**************************************************
 *
 * ChangeToLower
 *
 *  Changes the inputFile to uppercase and saves
 *  to ouputFile
 *
**************************************************/
char* ChangeToLower(char* str) {

    int i = 0;

    /* read as long as there are characters to be read */
    while (i <= (strlen(str))) {
        str[i] = tolower(str[i]);   //change to lowercase, overwrite letter
        i++;
    }
    return str;

}