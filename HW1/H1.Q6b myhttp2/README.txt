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
        -c: change max thread connections.
            Default: 3
            Example: myhttpd2 -c 5
        -p: change port number.
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

	-   Running time depends on consistency of client request. A filled backlog will return an average run time of

Overview of Functions:
	Tables.c:
		Functions for Processes:

			ProcessInit() 	 	This functions handles two things:
									(1) Initializing a UsrProcTable slot to -1
									(2) Removing a process from the UsrProcTable.

			AddToProcTable()	Adds a new process to the UsrProcTable and updates
								the status to newStatus

			GetProcIndex		Returns the process position inside UsrProcTable
								based on the passed in PID

		Functions for Semaphores:

			SemaphoreInit()		This functions handles two things:
									(1) Initializing a SemTable slot to -1
									(2) Removing a semaphore from the SemTable.

			AddToSemTable()		Adds a new semaphore to the SemTable and updates
								the status to newStatus

			GetSemIndex			Returns the semaphore position inside SemTable
								based on the passed in SID

	Lists.c:
		Functions for Processes:
			ListIsFull()        - 	Checks if a process or sem list is full.
			ListIsEmpty()       - 	Checks if a process or sem list is empty.
			InitializeList()    - 	Initializes the process list.
			AddProcessLL()      - 	Adds a process to the process list.
			RemoveProcessLL()   - 	Removed the head process from the process list.