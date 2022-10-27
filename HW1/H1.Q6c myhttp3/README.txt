Project:    myhttpd3.cpp
File:       README.txt
Authors:    Kiera Conway

Compile:

    Program compiled using Makefile - commands are copied below

        > make
            g++ -g -Wall -o myhttpd3 myhttpd3.cpp -lpthread -lm

        > make clean
            rm -f myhttpd3


Usage:

    myhttpd3 -h -c max_connections -p port_number -f file_name

        -h: display usage summary
        -c: change max thread connections
            Default: 3
            Example: myhttpd3 -c 5

        -p: change port number
            Default: 8080
            Example: myhttpd3 -p 8080
			
		-f: change log file name            
			Default: log.txt
			Example: myhttpd3 -f fileName.txt

    Note: 
		-	Must be ran as Root, 
		-	Must be ran on Linux,
		-	All Arguments are Optional

Contents:

	makefile:			Automates compilation and linking process

    myhttpd3.cpp:       Contains functions that implement a daemon 
						multithreaded server
	
	README.txt:			General information regarding project

	log.txt:  			Example log output of program


Notes:
	-	Program must be ran as Root
		
    -   Usage details above - all arguments are optional
		
	-	Client can be simulated using http://localhost:<PortNumber>/
	    Example: http://localhost:8080/

	-   Running time depends on frequency of client requests and max thread
	    connections. A filled backlog with 5 max connections per thread (the
	    default) will return an average run time of 0.004 seconds. Runtime is
	    displayed at end of log file.

	-   Example output can be found in file log.txt.
		stdout:
			Fork Successful: Child Process [xxxx] Created
			Terminating Parent Process ...
		
	    log file:
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
			> Program Runtime

	-	To follow child process in debugger on linux:
			1)	Create file: 
					~/.gdbinit
					
			2)	Add following to file, each sentence on a separate line:
					set detach-on-fork off
					set follow-fork-mode child
			
			- Program can now be followed via child process branches

	-	The following site was used as a guide to create Daemon code:
			https://www.thegeekstuff.com/2012/02/c-daemon-process/


Overview of Functions:
    main()                  Program Entry
                                (1)  Parses Program Arguments
								(2)  Create Child Process
								(3)  Terminate Parent Process
								(4)  Run Child in New Session
								(5)  Change Working Directory to Root
								(6)  Create Log file to Forward Error Reporting
								(7)  Eliminate Visible Error Reporting Methods
								(8)  Initiate Daemon by calling DaemonMain()
								(9)  Logs Final Data
								(10) Calculates Runtime
								(11) Closes File

    DaemonMain()			Handles Daemon's Main Processes
                                (1) Handles Socket and Thread Initialization
                                (2) Handles Socket Creation, Options, Binding,
                                    and Listening
                                (3) Creates Threads
                                (4) Joins Threads
                                (5) Releases Semaphores

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