Project:    myhttpd2.cpp
File:       README.txt
Authors:    Kiera   Conway

Compile:

    Program compiled using Makefile - commands are copied below

        > make
            g++ -g -Wall -o myhttpd2 myhttpd2.cpp -lpthread -lm

        > make clean
            rm -f myhttpd2


Usage:

    myhttpd2 -h -c max_connections -p port_number

        -h: display usage summary
        -c: change max thread connections
            Default: 3
            Example: myhttpd2 -c 5

        -p: change port number
            Default: 8080
            Example: myhttpd2 -p 8080

    Note: All Arguments are Optional


Contents:

	makefile:			Automates compilation and linking process

    myhttpd2.cpp:       Contains functions that implement a multithreaded server

	README.txt:			General information regarding project

	exampleOutput.txt:  Example output of program


Notes:
    -   Usage details above - all arguments are optional

	-	Client can be simulated using http://localhost:<PortNumber>/
	    Example: http://localhost:8080/

	-   Running time depends on frequency of client request and max thread
	    connections. A filled backlog with 5 max connections per thread (the
	    default) will return an average run time of 0.002 seconds. Runtime is
	    printed at end of each program.

	-   Example output can be found in file exampleOutput.txt.
	    Expected program output:
	        > Thread X Waiting for a Connection...
	        > Thread X Received Connection from Y
	        > Received Message Data
	            > Bytes Received
                > String Received
            > Sent Message Data
                > Bytes Sent
                > String Sent
            > Final Stats
                > Total Messages
                    > Total Sent
                    > Total Received
                > Total Connections


Overview of Functions:
    main()                  Program starting point.
                                (1) Handles Socket and Thread Initialization
                                (2) Parses Program Arguments
                                (3) Handles Socket Creation, Options, Binding,
                                    and Listening
                                (4) Creates Threads
                                (5) Joins Threads
                                (6) Releases Semaphores
                                (7) Displays Final Data
                                (8) Calculates Runtime

    ThreadHandler()         Handles Threads
                                (1) Begins Thread Run Loop
                                (2) Accepts Connection by calling AcceptConnection()
                                (3) Communicate over Connection by calling
                                    Communicator()
                                (4) Update Global Counters

    AcceptConnection()      Accepts Connection from listen() backlog and performs
                            error check

    Communicator()          Communicates across accepted connections and displays
                            send/receive message data

    Usage()                 Displays program usage information. Access using -h

