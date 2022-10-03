/* ------------------------------------------------------------------------
    myhttpd3.cpp

    CSC 718
    Operating Systems & Parallel Programming
    Dakota State University

    Kiera Conway

    Note: Simulate client at http://localhost:8080
------------------------------------------------------------------------ */

//todo: add to readme
// url of used website
//
// ~/.gdbinit
//set detach-on-fork off
//set follow-fork-mode child
//
// must run as root
// // // // // // //
//todo: if time
//  add daemon as option -d
//  add current time to logfile
//


/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

/** ----------------------------- Constants ----------------------------- **/
int SENT = 0;
int RECV = 1;

/** ----------------------------- Global ----------------------------- **/
int hostPort = 8080;
int maxThreads = 5;
int server_fd;                              //server file descriptor
sem_t mutex;                                //semaphore mutex
sem_t display;                              //semaphore display
sem_t accepting;                            //semaphore accepting
int messages[2];                            //sent, received
int connect_amount;                         //total connections made


int main(int argc, char *argv[]) {

    /*** Function Initialization ***/
    pid_t pid = 0;      //process id
    pid_t sid = 0;      //session id
    FILE *logFile;           //file pointer for log file

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

    //////////////////////////////////////////////
    //// Child Process is now a Daemon Process ///
    //////////////////////////////////////////////

    /*** Update Configuration ***/
    // Change Working Directory to Root
    chdir("/");

    // Create Log file to Forward Error Reporting
    if ((logFile = fopen ("log.txt", "w+")) == NULL) {
        fprintf(stdout,
                "Log Error: Unable to create log file\n<%s>\n\n"
                "Terminating Program ...\n\n",
                strerror(errno));		            //print error message

        exit(-1);
    }

    // Eliminate Visible Error Reporting Methods
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /*** Call Main Daemon Function ***/
    for (int i = 0; i < 10; i++){
        fprintf(logFile, "testing file output\n");		//print error message
    }

    fclose(logFile); //close logfile
    exit(0);
    //return (0);         //program end
}
