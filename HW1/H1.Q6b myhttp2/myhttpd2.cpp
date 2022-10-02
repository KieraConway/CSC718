/* ------------------------------------------------------------------------
    myhttpd2.cpp

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

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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

/** ----------------------------- Prototypes ---------------------------- **/
void ThreadHandler(int * thread_data);
int AcceptConnection(int thread_number);
int Communicator(int socket_fd);
void usage();
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
    /* Socket Variables */
    int *opt;								//socket option value
    struct sockaddr_in server_addr;			//server address

    /* Thread Variables */
    pthread_t threadIDs[maxThreads];		//thread IDS
    int thread[maxThreads][2];				//thread number, max connections
    int max_connections = 3;				//maximum number of connections for each thread

    /* Socket Init */
    opt = (int *) malloc(sizeof(int));		//allocate storage
    *opt = 1;

    /* Thread Init */
    for (int i = 0; i < maxThreads; i++) {
        if ((sem_init(&mutex, 0, i)) < 0) {
            fprintf(stderr,
                    "Semaphore Error: Unable to create semaphore mutex\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }
        if ((sem_init(&display, 0, i)) < 0) {
            fprintf(stderr,
                    "Semaphore Error: Unable to create semaphore display\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }
        if ((sem_init(&accepting, 0, i)) < 0) {
            fprintf(stderr,
                    "Semaphore Error: Unable to create semaphore accepting\n<%s>\n",
                    strerror(errno));		//print error message
            exit(-1);
        }
    }

    /*** Process Arguments ***/
    int input = 0;
    input = getopt( argc, argv,"hp:c:" );

    while (input != -1)
    {
        switch (input)
        {

            case 'p':
                hostPort = atoi(optarg);
                break;

            case 'c':
                maxThreads = atoi(optarg);
                break;

            case 'h':

            default:
                usage();
                exit(0);
        }
        input = getopt( argc, argv, "hp:c:");
    }

    /*** Create Socket ***/
    //  socket(int domain, int type, int protocol)
    //
    //  domain:     IPv4 Internet protocols
    //  type:       sequenced, reliable, two-way, connection-based byte streams
    //  protocol:   unspecified
    //
    //  Success: returns non-negative number, socket id
    // // //

    if ((server_fd = socket(AF_INET,
                            SOCK_STREAM,
                            0)) == -1) {							// attempt endpoint creation

        fprintf(stderr,
                "Socket Error: Unable to create socket\n<%s>\n",
                strerror(errno));                               	//print error message
        exit(-1);
    }

    /*** Set Socket Options ***/
    //  setsockopt(int socket, int level, int option_name,
    //             const void *option_value, socklen_t option_len);
    //
    //  socket: 		returned from socket()
    //  level:  		Set to socket level
    //  option_name:    allow reuse of local addresses, Keeps connections active with
    //                  periodic messages
    //  *option_value:  location to set the option specified by the option_name
    //  				option_len
    //
    //  Success: returns 0
    // // //

    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                    (char *) opt, sizeof(int)) == -1) ||
        (setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE,
                    (char *) opt, sizeof(int)) == -1)) {			//attempt option setting

        fprintf(stderr,
                "Option Error: Unable to set socket options\n<%s>\n",
                strerror(errno));                               	//print error message
        exit(-1);
    }
    free(opt);  //deallocate storage

    /*** Set Address Structure ***/
    memset(&(server_addr.sin_zero), 0, 0);		//clear structure
    server_addr.sin_family = AF_INET;			//match socket() domain
    server_addr.sin_addr.s_addr = INADDR_ANY;	//bind to any local address
    server_addr.sin_port = htons(hostPort);    //convert and set listening port

    /*** Bind Address to Socket ***/
    //  bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    //
    //  sockfd: 		returned from socket()
    //  addr:  			binding address
    //  addrlen:  		binding address length
    //
    //  Success: returns 0
    // // //

    if (bind(server_fd,
             (sockaddr *) &server_addr,
             sizeof(server_addr)) == -1) {							//attempt binding

        fprintf(stderr,
                "Bind Error: Unable to bind address to socket.\n"
                "Ensure no other devices are listening on port %d.\n"
                "<%s>\n",
                hostPort, strerror(errno));                    	//print error message
        exit(-1);

    }

    /*** Listen for Incoming Connection Requests ***/
    //  listen(int sockfd, int backlog);
    //
    //  sockfd: 		returned from socket()
    //  backlog:  		maximum pending requests
    //
    //  Success: returns 0
    // // //

    if (listen(server_fd, 10) == -1) {                          	//attempt listening

        fprintf(stderr,
                "Listen Error: Unable to set passive socket\n<%s>\n",
                strerror(errno));                               	//print error message
        exit(-1);
    }

    //print listening information
    printf("> myhttpd2 server listening on port %d\n", hostPort);

    /*** Start Threads ***/
    for (int i = 0; i < maxThreads; i++) {
        thread[i][0] = i;					//save thread number
        thread[i][1] = max_connections;		//save max connections

        if (pthread_create(&threadIDs[i], NULL,
                           (void *(*)(void *)) &ThreadHandler,
                           &thread[i]) != 0) {                      //attempt thread creation

            fprintf(stderr,
                    "Thread Error: Unable to create thread\n<%s>\n",
                    strerror(errno));                               //print error message
            exit(-1);
        }
    }

    /*** Join Threads ***/
    for (int i = 0; i < maxThreads; i++) {
        pthread_join(threadIDs[i], NULL);
    }

    /*** Release Semaphores ***/
    sem_destroy(&mutex);
    sem_destroy(&display);
    sem_destroy(&accepting);

    /*** Display Final Data ***/

    printf("\n\n--------------------------\n"
           "> Total Messages: %d\n"
           "> > Sent: %d\n"
           "> > Receieved: %d\n\n",
           messages[SENT] + messages[RECV], messages[SENT], messages[RECV]);
    printf("> Total Connections: %d\n"
           "--------------------------\n\n",
           connect_amount);

    /*** Program End ***/
    return 0;

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

        /*** Display Waiting Message ***/
        sem_wait(&display);                                     	//access display semaphore
        printf("\n> Thread %d Waiting for a Connection...\n", thread);
        sem_post(&display);                                     	//release display semaphore

        //
        // Simulate Client Here: http://localhost:8080/
        //

        /*** Accept Connection ***/
        int socket_fd;
        if((socket_fd = AcceptConnection(thread)) != -1){


            /*** Receive Connection ***/
            Communicator(socket_fd);

            /*** Update Counters ***/
            // Update Connection Counter
            count++;

            // Update Global Counters
            sem_wait(&mutex);			//access mutex semaphore
            connect_amount += 1;
            sem_post(&mutex);			//release mutex semaphore
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
    //
    //  sockfd: 		returned from socket()
    //	addr:			pointer to sockaddr structure
    //  addrlen:  		addr structure (bytes)
    //
    //  Success: socket file descriptor
    // // //

    sem_wait(&accepting);							//access accepting semaphore

    if((socket_fd = accept((int) server_fd,
                           (sockaddr*)&socket_addr,
                           &addr_size)) != -1){		//attempt connection accept

        sem_post(&accepting);						//release accepting semaphore


        /*** Display Received Update ***/
        sem_wait(&display);							//access display semaphore

        printf("> Thread %d Received connection from %s\n",
               thread_number, inet_ntoa(socket_addr.sin_addr));

        sem_post(&display);							//release display semaphore

        /*** Success ***/
        return socket_fd;
    }
    else {

        // Connection Failed
        /*** Display Received Message ***/
        sem_wait(&display);							//access display semaphore
        fprintf(stderr,
                "Connection Error: Unable to accept connection request\n<%s>\n",
                strerror(errno));					//print error message
        sem_post(&display);							//release display semaphore

        return(-1);
    }


}

/* ------------------------------------------------------------------------
  Name -            Communicator
  Purpose -         Communicates across connection accepted
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
                     buffer_len, 0)) == -1){		//attempt receive

        /*** Display Received Error ***/
        sem_wait(&display);							//access display semaphore
        fprintf(stderr,
                "Receive Error: Unable to receive data\n<%s>\n",
                strerror(errno));					//print error message
        sem_post(&display);							//release display semaphore

        return(-1);
    }

    /*** Display Received Data ***/
    sem_wait(&display);								//access display semaphore
    printf("============================\n"
           "Bytes Received: %d\n"
           "String Received:\n\n"
           "%s\n"
           "============================\n",
           bytes, buffer);												//display received data
    strcpy(buffer,
           "HTTP/1.1 200 OK\n"
           "Server: demo\n"
           "Content-Length: 37\n"
           "Connection: close\n"
           "Content-Type: html\n\n"
           "<html><body>Welcome to my first page!</body></html>");      //update buffer
    sem_post(&display);								//release display semaphore

    /*** Update Global Counter ***/
    sem_wait(&mutex);			//access mutex semaphore
    messages[RECV]++;			//update messages recv
    sem_post(&mutex);			//release mutex semaphore

    /*** Transmit Message ***/
    if ((bytes = send(socket_fd, buffer,
                      strlen(buffer), 0)) == -1){				//attempt send

        /*** Display Send Error ***/
        sem_wait(&display);                                     //access display semaphore
        fprintf(stderr,
                "Send Error: Unable to transmit data\n<%s>\n",
                strerror(errno));								//print error message
        sem_post(&display);                                     //release display semaphore

        return(-1);
    }
    /*** Display Sent Data ***/
    sem_wait(&display);							//access display semaphore
    printf("============================\n"
           "Bytes Sent: %d\n"
           "String Sent:\n\n"
           "%s\n"
           "============================\n",
           bytes, buffer);						//display sent data
    sem_post(&display);							//release display semaphore

    /*** Update Global Counter ***/
    sem_wait(&mutex);							//access mutex semaphore
    messages[SENT]++;							//update messages sent
    sem_post(&mutex);							//release mutex semaphore

    close(socket_fd);							//close socket
    return 0;
}

void usage(){
    printf("\nmyhttpd2, a multithreaded webserver\n"
           "ver 2.0, 2022\n"
           "Usage: myhttpd2 -h -c max_connections -p port_number\n"
           "	-h: display usage summary\n"
           "	-c: change max thread connections. Default = 3 | example: -c 5\n"
           "	-p: change port number. Default = 8080 | example: -p 8080 \n\n");
}