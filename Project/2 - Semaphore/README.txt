Project:    proj_sem.c
File:       README.txt
Authors:    Kiera   Conway

Compile:

    Program compiled using Makefile - commands are copied below

        > make
            gcc -g -Wall -o proj_sem proj_sem.c -lpthread -lm

        > make clean
            rm -f proj_sem


Usage:

	proj_sem -h -c -v -t <1-10> -f <file> -s <string>

        -h:            Display Usage summary    Example: proj_sem -h

        -c:            Set Case Sensitivity     Default: False        |   Example: proj_sem -c
        -v:            Set Verbose Mode         Default: False        |   Example: proj_sem -v

        -t <1-10>:     Set Number of Threads    Default: 5            |   Example: proj_sem -t 3
        -f <file>:     Specify File             Default: testFile.txt |   Example: proj_sem -f myFile.txt
        -s <string>:   Specify Search Term      Default: life         |   Example: proj_sem -s search_term

    Note: 
		-	All Arguments are Optional (Defaults are used if no arguments are set)


Contents:

	makefile:			Automates compilation and linking process

    proj_sem.c:			Contains functions that implement a multithreaded file parser

	README.txt:			General information regarding project

	testFile.txt:  		Default file parsed


Notes:
    -   Usage details above - all arguments are optional

	-   Running time depends on max thread connections.  Runtime is
	    printed at end of each program.
		
	-	testFile.txt is a randomly generated AI text file, the information 
		within is nonfactual and mostly nonsensical.

Overview of Functions:
    main()              Program Entry
                            (1) Save Program Start Time
                            (2) Parse Program Arguments
                            (3) Initialize Semaphores
							(4)	Split Input File
									FileSplitter();
									
							---- Begin Parallelization ----		
                            (5) Create Threads
									ThreadHandler()
                            (6) Join Threads
							---- End Of Parallel Region ----
							
                            (7) Release Semaphores
                            (8) Display Final Data
                            (9) Calculate and Print Runtime

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

