Project:    proj_sem.c
File:       README.txt
Authors:    Kiera   Conway

Compile:

    Program compiled using Makefile - commands are copied below

        > make
            g++ -g -Wall -o proj_sem proj_sem.c -lpthread -lm

        > make clean
            rm -f proj_sem


Usage:

	proj_seq -h -c -v -t <1-10> -f <file> -s <string>

        -h:            Display Usage summary    Example: proj_seq -h

        -c:            Set Case Sensitivity     Default: False        |   Example: proj_seq -c
        -v:            Set Verbose Mode         Default: False        |   Example: proj_seq -v

        -t <1-10>:     Set Number of Threads    Default: 5            |   Example: proj_seq -t 3
        -f <file>:     Specify File             Default: testFile.txt |   Example: proj_seq -f myFile.txt
        -s <string>:   Specify Search Term      Default: life         |   Example: proj_seq -s search_term

    Note: 
		-	All Arguments are Optional (Defaults are used if no arguments are set)


Contents:

	makefile:			Automates compilation and linking process

    proj_sem.c:			Contains functions that implement a multithreaded file parser

	README.txt:			General information regarding project

	testFile.txt:  		Default file parsed


Notes:
    -   Usage details above - all arguments are optional

	-	Client can be simulated using http://localhost:<PortNumber>/
	    Example: http://localhost:8080/

	-   Running time depends on max thread connections.  Runtime is
	    printed at end of each program.

Overview of Functions:
    main()              Program Entry
                            (1) Save Program Start Time
                            (2) Parse Program Arguments
                            (3) Initialize Semaphores
							(4)	Split Input File
									FileSplitter();
                            (5) Creates Threads
									ThreadHandler()
                            (6) Joins Threads
                            (7) Releases Semaphores
                            (8) Displays Final Data
                            (8) Calculates Runtime

    ThreadHandler()		Handles Threads
                            (1) Parse Temp File
                            (2) Close and Remove File
                            (3) Update Global Counters

	FileSplitter()		Reads in file and splits into n files
                            (1) Open Input File
                            (2) Parse and Split Input File
                            (3) Close Input File
	
	ProcessArgs()		Process user arguments
	
	TrimLeft()			Removes characters from the beginning of a string
	
	TrimRight()			Removes characters from the end of a string
	
	ChangeToLower()		Changes input string to lower case

    Usage()             Displays program usage information. Access using -h

