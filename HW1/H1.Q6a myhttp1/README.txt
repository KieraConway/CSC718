Project:    myhttpd1.cpp
File:       README.txt
Student:    Kiera Conway

Compile:

    Program compiled using Makefile - commands are copied below

        > make
            g++ -g -Wall -o myhttpd1 myhttpd1.cpp

        > make clean
            rm -f myhttpd1


Usage:

    myhttpd1 -h -c max_connections -p port_number -f file_name

        -h: display usage summary

        -p: change port number
            Default: 8080
            Example: myhttpd1 -p 8080
			

    Note: 
		-	All Arguments are Optional

Contents:

	makefile:			Automates compilation and linking process

    myhttpd1.cpp:       Contains functions that implement server
	
	README.txt:			General information regarding project

	exampleOutput.txt:  Example output of program


Notes:
	
    -   Usage details above - all arguments are optional
		
	-	Client can be simulated using http://localhost:<PortNumber>/
	    Example: http://localhost:8080/

	-   Running time varies as it continues in a listening state until 
		exited manually
		
	-   Example output can be found in file log.txt.

	        > Thread X Waiting for a Connection...
	        > Thread X Received Connection from Y
	        > Received Message Data
	            > Bytes Received
                > String Received
            > Sent Message Data
                > Bytes Sent

	-	Code was given to me as an example. My changes are from lines 
	142-146 and notated clearly


Overview of Functions:
    main()                  Program Entry
                                (1)  Parses Program Arguments
                                (2) Handles Socket Initialization
                                (3) Handles Socket Creation, Options, Binding,
                                    and Listening
                                (3) Creates Threads
                                (4) Joins Threads
                                (5) Releases Semaphores

    Usage()                 Displays program usage information. Access using -h