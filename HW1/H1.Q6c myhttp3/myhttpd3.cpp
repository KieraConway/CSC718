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
int maxConnections = 3;				        //maximum number of connections for each thread
int server_fd;                              //server file descriptor
FILE *logFile;                              //file pointer for log file
sem_t mutex;                                //semaphore mutex
sem_t logging;                              //semaphore logging
sem_t accepting;                            //semaphore accepting
int messages[2];                            //sent, received
int connect_amount;                         //total connections made

/** ----------------------------- Prototypes ---------------------------- **/
void DaemonMain();
void ThreadHandler(int * thread_data);
int AcceptConnection(int thread_number);
int Communicator(int socket_fd);
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

    /*** Function Initialization ***/
    clock_t start = clock();    	//program start time
    pid_t pid = 0;              	//process id
    pid_t sid = 0;              	//session id
    char fileName[20];          	//holds log file name
    bool file_has_name = false; 	//keeps track if file has a name

    memset(fileName, 0, 20);

    /*** Process Arguments ***/
    int input = 0;
    input = getopt( argc, argv,"hp:c:f:" );

    while (input != -1)
    {
        switch (input)
        {
            /* Change Port */
            case 'p':
                hostPort = atoi(optarg);
                break;

                /* Change Connection Amount */
            case 'c':
                maxConnections = atoi(optarg);
                break;

                /* Change Log File Name */
            case 'f':
                strcpy(fileName, optarg);
                file_has_name = true;
                break;

                /* Access Help Menu */
            case 'h':

            default:
                Usage();
                exit(0);
        }
        input = getopt( argc,
                        argv,
                        "hp:c:f:");       		//check for remaining args
    }
    if(!file_has_name){
        strcpy(fileName, "log.txt");
    }

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
                strerror(errno));				//print error message
        exit(-1);
    }

    /*** Terminate Parent Process ***/
    if(pid > 0){
        fprintf(stdout,
                "Fork Successful: Child Process [%d] Created\n"
                "Terminating Parent Process ...\n\n",
                pid);
        exit(0);                        		//terminate parent
    }

    /*** Run Child in New Session ***/
    umask(0);               			//clear inherited permissions
    if((sid = setsid()) < 0){   		//set child as leader to new session/group
        fprintf(stderr,
                "Session Error: Unable to create new session\n<%s>\n\n"
                "Terminating Program ...\n\n",
                strerror(errno));		//print error message
        exit(-1);
    }

    //////////////////////////////////////////////
    //// Child Process is now a Daemon Process ///
    //////////////////////////////////////////////

    /*** Update Configurations ***/
    /* Change Working Directory to Root */
    chdir("/");

    /* Create Log file to Forward Error Reporting */
    if ((logFile = fopen (fileName, "w+")) == NULL) {
        fprintf(stdout,
                "Log Error: Unable to create Log file\n<%s>\n\n"
                "Terminating Program ...\n\n",
                strerror(errno));				//print error message

        exit(-1);
    }

    /* Eliminate Visible Error Reporting Methods */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /*** Call Main Daemon Function ***/
    DaemonMain();

    /*** Program Termination ***/
    /* Log Final Data */
    fprintf(logFile,
            "\n\n--------------------------\n"
            "  Final Connection Data\n"
            "- - - - - - - - - - - - - \n"
            "> Total Messages: %d\n"
            "  > Sent: %d\n"
            "  > Received: %d\n\n",
            messages[SENT] + messages[RECV], messages[SENT], messages[RECV]);
    fprintf(logFile,
            "> Total Connections: %d\n"
            "--------------------------\n\n",
            connect_amount);

    /* Calculate Runtime */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    fprintf(logFile,"<Program Runtime: %.4fs>\n\n", time_spent);

    /* Close Log File */
    fclose(logFile);

    /* Terminate */
    return (0);
}


/* ------------------------------------------------------------------------
  Name -            DaemonMain
  Purpose -         Handles daemon's main processes
  Parameters -      none
  Returns -         none, exits on failure
  Side Effects -    Initializes socket and thread variables, initializes
                    semaphores, handles socket creation, options, binding,
                    and listening, creates threads, joins threads, and
                    releases semaphores.
  ----------------------------------------------------------------------- */
void DaemonMain(){
    /*** Function Initialization ***/
    /* Socket Variables */
    int *opt;								//socket option value
    struct sockaddr_in server_addr;			//server address

    /* Thread Variables */
    pthread_t threadIDs[maxThreads];		//thread IDS
    int thread[maxThreads][2];				//thread number, max connections

    /* Socket Init */
    opt = (int *) malloc(sizeof(int));		//allocate storage
    *opt = 1;

    /* Thread Init */
    for (int i = 0; i < maxThreads; i++) {

        // Mutex Semaphore
        if ((sem_init(&mutex, 0, i)) < 0) {

            fprintf(logFile,
                    "Semaphore Error: Unable to create semaphore mutex\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }

        // Updating Log Semaphore
        if ((sem_init(&logging, 0, i)) < 0) {
            fprintf(logFile,
                    "Semaphore Error: Unable to create semaphore logging\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }

        // Accepting Connections Semaphore
        if ((sem_init(&accepting, 0, i)) < 0) {
            fprintf(logFile,
                    "Semaphore Error: Unable to create semaphore accepting\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }
    }

    /*** Create Socket ***/
    //  socket(int domain, int type, int protocol)
    //  Success: returns non-negative number, socket id
    // // //

    if ((server_fd = socket(AF_INET,
                            SOCK_STREAM,
                            0)) == -1) {							// attempt endpoint creation

        fprintf(logFile,
                "Socket Error: Unable to create socket\n<%s>\n",
                strerror(errno));                               	//print error message
        exit(-1);
    }


    /*** Set Socket Options ***/
    //  setsockopt(int socket, int level, int option_name,
    //             const void *option_value, socklen_t option_len);
    //  Success: returns 0
    // // //

    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                    (char *) opt, sizeof(int)) == -1) ||
        (setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE,
                    (char *) opt, sizeof(int)) == -1)) {			//attempt option setting

        fprintf(logFile,
                "Option Error: Unable to set socket options\n<%s>\n",
                strerror(errno));                               	//print error message
        exit(-1);
    }
    free(opt);  //deallocate storage

    /*** Set Address Structure ***/
    memset(&(server_addr.sin_zero), 0, 0);		//clear structure
    server_addr.sin_family = AF_INET;			//match socket() domain
    server_addr.sin_addr.s_addr = INADDR_ANY;	//bind to any local address
    server_addr.sin_port = htons(hostPort);		//convert and set listening port

    /*** Bind Address to Socket ***/
    //  bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    //  Success: returns 0
    // // //

    if (bind(server_fd,
             (sockaddr *) &server_addr,
             sizeof(server_addr)) == -1) {							//attempt binding

        fprintf(logFile,
                "Bind Error: Unable to bind address to socket.\n"
                "Ensure no other devices are listening on port %d.\n"
                "<%s>\n",
                hostPort, strerror(errno));                    		//print error message
        exit(-1);
    }

    /*** Listen for Incoming Connection Requests ***/
    //  listen(int sockfd, int backlog);
    //  Success: returns 0
    // // //

    if (listen(server_fd, 10) == -1) {                          	//attempt listening

        fprintf(logFile,
                "Listen Error: Unable to set passive socket\n<%s>\n",
                strerror(errno));                               	//print error message
        exit(-1);
    }

    //log listening information
    fprintf(logFile,
            "> myhttpd2 server listening on port %d\n",
            hostPort);

    /*** Start Threads ***/
    for (int i = 0; i < maxThreads; i++) {
        thread[i][0] = i;					//save thread number
        thread[i][1] = maxConnections;		//save max connections

        if (pthread_create(&threadIDs[i], NULL,
                           (void *(*)(void *)) &ThreadHandler,
                           &thread[i]) != 0) {                      //attempt thread creation

            // Log Error Message
            sem_wait(&logging);										//access logging semaphore
            fprintf(logFile,
                    "Thread Error: Unable to create thread\n<%s>\n",
                    strerror(errno));                               //print error message
            sem_post(&logging);										//release logging semaphore

            exit(-1);
        }
    }

    /*** Join Threads ***/
    for (int i = 0; i < maxThreads; i++) {
        pthread_join(threadIDs[i], NULL);
    }

    /*** Release Semaphores ***/
    sem_destroy(&mutex);
    sem_destroy(&logging);
    sem_destroy(&accepting);

    return;
}


/* ------------------------------------------------------------------------
  Name -            ThreadHandler
  Purpose -         Handles each thread
  Parameters -      thread_data:    contains following thread information:
                            [0]:    Thread Number
                            [1]:    Max Connections
  Returns -         none, exits on failure
  Side Effects -    calls AcceptConnection(), Communicator(), and
                    initiates/signals semaphores
  ----------------------------------------------------------------------- */
void ThreadHandler(int * thread_data){
    /*** Function Initialization ***/
    /* Thread Variables */
    int thread = thread_data[0];			//copy thread number
    int max_connections = thread_data[1];	//copy max connections

    int count = 0;							//number of connections

    /*** Run Thread for n Connections ***/
    while (count < max_connections){
        /*** Log Waiting Message ***/
        sem_wait(&logging);                                     	//access logging semaphore
        fprintf(logFile,"\n> Thread %d Waiting for a Connection...\n", thread);
        sem_post(&logging);                                     	//release logging semaphore

        //
        // Simulate Client Here: http://localhost:8080/
        //

        /*** Accept Connection ***/
        int socket_fd;
        if((socket_fd = AcceptConnection(thread)) != -1){

            /*** Communicate over Connection ***/
            Communicator(socket_fd);

            /*** Update Counters ***/
            // Update Connection Counter
            count++;

            // Update Global Counters
            sem_wait(&mutex);				//access mutex semaphore
            connect_amount += 1;
            sem_post(&mutex);				//release mutex semaphore
        }

        else{
            exit(-1);
        }
    }
}

/* ------------------------------------------------------------------------
  Name -            AcceptConnection
  Purpose -         Accepts Connection from listen() backlog
  Parameters -      thread_number: thread number
  Returns -         >0: Success, socket file descriptor
                    -1: Connection Error
  Side Effects -    Initiates/signals semaphores, removes first incoming
                    connection request from listen() backlog
  ----------------------------------------------------------------------- */
int AcceptConnection(int thread_number){

    /* Socket Variables */
    int socket_fd;                      		//socket file descriptor
    struct sockaddr_in socket_addr;     		//socket address
    socklen_t addr_size = sizeof(sockaddr_in);	//address size

    /*** Accept Connection ***/
    //  int accept(int sockfd, struct sockaddr *restrict addr,
    //				socklen_t *restrict addrlen);
    //  Success: socket file descriptor
    // // //

    sem_wait(&accepting);							//access accepting semaphore

    if((socket_fd = accept((int) server_fd,
                           (sockaddr*)&socket_addr,
                           &addr_size)) != -1){		//attempt connection accept

        sem_post(&accepting);						//release accepting semaphore


        /*** Log Received Update ***/
        sem_wait(&logging);							//access logging semaphore
        fprintf(logFile,
                "\n> Thread %d Received Connection from %s\n\n",
                thread_number, inet_ntoa(socket_addr.sin_addr));
        sem_post(&logging);							//release logging semaphore

        /*** Success ***/
        return socket_fd;
    }
    else {

        // Connection Failed
        /*** Log Received Message ***/
        sem_wait(&logging);							//access logging semaphore
        fprintf(logFile,
                "Connection Error: Unable to accept connection request\n<%s>\n",
                strerror(errno));					//print error message
        sem_post(&logging);							//release logging semaphore

        return(-1);
    }
}


/* ------------------------------------------------------------------------
  Name -            Communicator
  Purpose -         Communicates across accepted connections
                    by AcceptConnection()
  Parameters -      socket_fd: socket file descriptor
  Returns -          0: Success
                    -1: Connection Error
  Side Effects -    Initiates/signals semaphores, receives/sends messages
                    across connection, updates global variables
  ----------------------------------------------------------------------- */
int Communicator(int socket_fd){
    /*** Function Initialization ***/
    char buffer[1024];
    int buffer_len = 1024;
    int bytes;              			//bytes returned from recv() and send()

    memset(buffer, 0, buffer_len);      //clear buffer structure

    /*** Receive Data ***/
    if((bytes = recv(socket_fd, buffer,
                     buffer_len, 0)) == -1){			//attempt receive

        /*** Log Received Error ***/
        sem_wait(&logging);								//access logging semaphore
        fprintf(logFile,
                "Receive Error: Unable to receive data\n<%s>\n",
                strerror(errno));						//print error message
        sem_post(&logging);								//release logging semaphore

        return(-1);
    }

    /*** Log Received Data ***/
    sem_wait(&logging);							//access logging semaphore
    fprintf(logFile,
            "============================\n"
            "Bytes Received: %d\n"
            "String Received:\n\n"
            "%s\n"
            "============================\n",
            bytes, buffer);						//log received data
    strcpy(buffer,
           "HTTP/1.1 200 OK\n"
           "Server: demo\n"
           "Content-Length: 37\n"
           "Connection: close\n"
           "Content-Type: html\n\n"
           "<html><body>Welcome to my first page!</body></html>");      //update buffer
    sem_post(&logging);							//release logging semaphore

    /*** Update Global Counter ***/
    sem_wait(&mutex);			//access mutex semaphore
    messages[RECV]++;			//update messages recv
    sem_post(&mutex);			//release mutex semaphore

    /*** Transmit Message ***/
    if ((bytes = send(socket_fd, buffer,
                      strlen(buffer), 0)) == -1){				//attempt send

        /*** Log Send Error ***/
        sem_wait(&logging);                                     //access logging semaphore
        fprintf(logFile,
                "Send Error: Unable to transmit data\n<%s>\n",
                strerror(errno));								//print error message
        sem_post(&logging);                                     //release logging semaphore

        return(-1);
    }
    /*** Log Sent Data ***/
    sem_wait(&logging);							//access logging semaphore
    fprintf(logFile,
            "============================\n"
            "Bytes Sent: %d\n"
            "String Sent:\n\n"
            "%s\n"
            "============================\n",
            bytes, buffer);						//logging sent data
    sem_post(&logging);							//release logging semaphore

    /*** Update Global Counter ***/
    sem_wait(&mutex);							//access mutex semaphore
    messages[SENT]++;							//update messages sent
    sem_post(&mutex);							//release mutex semaphore

    close(socket_fd);							//close socket
    return 0;
}

void Usage(){
    printf("\nmyhttpd2, a multithreaded webserver\n"
           "ver 2.0, 2022\n"
           "Usage: myhttpd2 -h -c max_connections -p port_number\n"
           "	-h: Display Usage summary\n"
           "	-c: change max thread connections   Default: 3        |   example: -c 3 \n"
           "	-f: change log file name            Default: log.txt  |   example: -f fileName.txt \n"
           "	-p: change port number              Default: 8080     |   example: -p 8080 \n\n");
}
