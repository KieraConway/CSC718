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
#include <unistd.h>
//#include <resolv.h>
//#include <sys/socket.h>



/** ------------------------------ Globals --------------------------------- **/
int host_port = 8080;

/** ----------------------------- Functions ----------------------------- **/
int AcceptConnection(int server_fd);    //TODO: MOVE FUNCTION HERE
int ReceiveConnection(int socket_fd);


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

int main(int argc, char *argv[]){

    /*** Function Initialization ***/
    int server_fd;                                  //server file descriptors
    int * opt;                                      //socket option value
    struct sockaddr_in server_addr;                 //server address

    opt = (int*)malloc(sizeof(int));           //allocate storage
    *opt = 1;

    /*** Process Arguments ***/
    //todo: insert argc/v handling

    /*** Create Socket ***/
    //  socket(int domain, int type, int protocol)
    //  domain:     IPv4 Internet protocols
    //  type:       sequenced, reliable, two-way, connection-based byte streams
    //  protocol:   unspecified
    //
    //  Success: returns non-negative number, socket id
    // // //

    if ((server_fd = socket(AF_INET,
                            SOCK_STREAM,
                            0)) == -1){                         // attempt endpoint creation

        fprintf(stderr,
                "Socket Error: Unable to create socket\n<%s>\n",
                strerror(errno));                               //print error message
        exit(-1);
    }

    /*** Set Socket Options ***/
    //  setsockopt(int socket, int level, int option_name,
    //             const void *option_value, socklen_t option_len);
    //
    //  socket: returned from socket()
    //  level:  Set to socket level
    //  option_name:    allow reuse of local addresses, Keeps connections active with
    //                  periodic messages
    //  *option_value:  location to set the option specified by the option_name
    //  option_len
    //
    //  Success: returns 0
    // // //

    if ((setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR,
                    (char*)opt, sizeof(int)) == -1 ) ||
        (setsockopt(server_fd, SOL_SOCKET,SO_KEEPALIVE,
                    (char*)opt, sizeof(int)) == -1 ) ){     //attempt option setting

        fprintf(stderr,
                "Option Error: Unable to set socket options\n<%s>\n",
                strerror(errno));                               //print error message
        exit(-1);
    }
    free(opt);  //deallocate storage

    /*** Set Address Structure ***/
    memset(&(server_addr.sin_zero), 0, 0);      //clear structure
    server_addr.sin_family = AF_INET;                   //match socket() domain
    server_addr.sin_addr.s_addr = INADDR_ANY;           //bind to any local address
    server_addr.sin_port = htons(host_port);    //convert and set listening port

    /*** Bind Address to Socket ***/
    if (bind(server_fd,
             (sockaddr*)&server_addr,
             sizeof(server_addr)) == -1 ){                          //attempt binding

        fprintf(stderr,
                "Bind Error: Unable to bind address to socket.\n"
                "Ensure no other devices are listening on port %d.\n"
                "<%s>\n",
                host_port, strerror(errno));                    //print error message
        exit(-1);

    }

    /*** Listen for Incoming Connection Requests ***/
    if (listen(server_fd, 10) == -1 ){                          //attempt listening

        fprintf(stderr,
                "Listen Error: Unable to set passive socket\n<%s>\n",
                strerror(errno));                               //print error message
        exit(-1);
    }

    //print listening information
    printf("> myhttpd2 server listening on port %d\n", host_port);

    AcceptConnection(server_fd);
}

//todo: change to void
int AcceptConnection(int server_fd){

    /*** Function Initialization ***/
    int socket_fd;                      //socket file descriptor
    struct sockaddr_in socket_addr;     //socket address
    socklen_t addr_size = sizeof(sockaddr_in);      //address size

    while (true){
        //todo: insert thread id below
        printf("> Waiting for a Connection...\n");

        //
        // Simulate Client Here: http://localhost:8080/
        //

        /*** Accept Connection ***/
        if((socket_fd = accept(server_fd,
                               (sockaddr*)&socket_addr,
                               &addr_size)) != -1){         //attempt connection accept

            //todo: insert thread id below
            printf("> Received connection from %s\n", inet_ntoa(socket_addr.sin_addr));

            ReceiveConnection(socket_fd);

        }
        else {
            // Connection Failed
            fprintf(stderr,
                    "Connection Error: Unable to accept connection request\n<%s>\n",
                    strerror(errno));                               //print error message
            exit(-1);
        }
    }
}

int ReceiveConnection(int socket_fd){

    /*** Function Initialization ***/
    char buffer[1024];
    int buffer_len = 1024;
    int bytes;              //bytes returned from recv() and send()

    memset(buffer, 0, buffer_len);      //clear structure

    /*** Receive Data ***/
    if((bytes = recv(socket_fd, buffer,
                     buffer_len, 0)) == -1){                    //attempt receive

        fprintf(stderr,
                "Receive Error: Unable to receive data\n<%s>\n",
                strerror(errno));                               //print error message
        return(-1);
    }

    printf("============================\n"
           "Bytes Received: %d\n"
           "String Received:\n\n"
           "%s\n"
           "============================\n",
           bytes, buffer);                                              //display received data
    strcpy(buffer,
           "HTTP/1.1 200 OK\n"
           "Server: demo\n"
           "Content-Length: 37\n"
           "Connection: close\n"
           "Content-Type: html\n\n"
           "<html><body>Welcome to my first page!</body></html>");      //update buffer

    /*** Transmit Message ***/
    if ((bytes = send(socket_fd, buffer,
                      strlen(buffer), 0)) == -1){               //attempt send

        fprintf(stderr,
                "Send Error: Unable to transmit data\n<%s>\n",
                strerror(errno));                                   //print error message
        return(-1);
    }
    printf("============================\n"
           "Bytes Sent: %d\n"
           "String Sent:\n\n"
           "%s\n"
           "============================\n",
           bytes, buffer);                                              //display sendt data


    close(socket_fd);
    return 0;

}
