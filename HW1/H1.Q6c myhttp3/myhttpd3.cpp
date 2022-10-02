/* ------------------------------------------------------------------------
    myhttpd3.cpp

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

    Note: Simulate client at http://localhost:8080
------------------------------------------------------------------------ */

/* Standard Libraries */
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <errno.h>
#include <iostream>     #todo: comment out after FILE removal
#include <unistd.h>
#include <sys/stat.h>

//
//#include <arpa/inet.h>
//#include <netinet/in.h>
//#include <sys/socket.h>
//#include <time.h>
//
//#include <assert.h>
//#include <pthread.h>
//#include <semaphore.h>

int main(int argc, char *argv[]) {

    /*** Function Initialization ***/
    pid_t pid = 0;      //process id
    pid_t sid = 0;      //session id
    FILE *fp= NULL;     //file pointer for log file

    /*** Create Child Process ***/
    // fork() Return Values
    //  Success:    Child PID   > Parent
    //                  0       > Child
    //  Failure:    -1
    // // // // // // //
    if ((pid = fork()) == -1){                  //attempt fork process
        fprintf(stderr,
                "Fork Error: Unable to fork process\n<%s>\n\n"
                "Terminating Program ...\n\n",
                strerror(errno));		//print error message
        exit(-1);
    }

    /*** Terminate Parent Process ***/
    if(pid > 0){
        fprintf(stdout,
                "Fork Successful: Child Process [%d] Created\n"
                "Terminating Parent Process ...\n\n",
                pid);
        exit(0);                        //terminate parent
    }

    /*** Run Child in New Session ***/
    umask(0);               //clear inherited permissions
    if((sid = setsid()) < 0){   //set child as leader to new session/group
        fprintf(stderr,
                "Session Error: Unable to create new session\n<%s>\n\n"
                "Terminating Program ...\n\n",
                strerror(errno));		//print error message
        exit(-1);
    }

    //
    // Child Process is now a Daemon Process
    //

    /*** Update Configuration ***/
    // Change Working Directory to Root
    chdir("/");

    // Eliminate Error Reporting Methods
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Forward Error Reporting to Log File
    fp = fopen ("Log.txt", "w+");

    /*** Call Main Daemon Function ***/

        //            Change the working directory of the daemon process to root and close stdin, stdout and stderr file descriptors.
        //            Let the main logic of daemon process run.
        // Enter multithreader here?


    fclose(fp); //close logfile
    return (0);         //program end
}
