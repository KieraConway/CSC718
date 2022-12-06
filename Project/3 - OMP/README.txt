Project:    proj_omp.c
File:       README.txt
Authors:    Kiera   Conway

Compile:

    Program compiled using Makefile - commands are copied below

        > make
            gcc -03 -fopenmp proj_omp.c -o proj_omp -lm

        > make clean
            rm proj_omp


Usage:

	proj_omp -h -c -v -t <1-10> -f <file> -s <string>

        -h:            Display Usage summary    Example: proj_omp -h

        -c:            Set Case Sensitivity     Default: False        |   Example: proj_omp -c
        -v:            Set Verbose Mode         Default: False        |   Example: proj_omp -v

        -t <1-10>:     Set Number of Threads    Default: 5            |   Example: proj_omp -t 3
        -f <file>:     Specify File             Default: testFile.txt |   Example: proj_omp -f myFile.txt
        -s <string>:   Specify Search Term      Default: life         |   Example: proj_omp -s search_term

    Note: 
		-	All Arguments are Optional (Defaults are used if no arguments are set)


Contents:

	makefile:			Automates compilation and linking process

    proj_omp.c:			Contains functions that implement an omp file parser

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
                            (2) Set Defaults
                            (3) Parse Program Arguments
							(4)	Split Input File
									FileSplitter();
									
							---- Begin Parallelization ----
                            (5) Fork Threads
								-	Obtain pInputFile as Temp File
								-	Rewind Temp File
								-	Parse Temp File
								-	Close and Remove File
							---- End Of Parallel Region ----
							
                            (6) Display Final Data
                            (7) Calculate and Print Runtime

	FileSplitter()		Reads in file and splits into n files
                            (1) Open Input File
                            (2) Parse and Split Input File
                            (3) Close Input File
	
	ProcessArgs()		Process user arguments
	
	TrimLeft()			Removes characters from the beginning of a string
	
	TrimRight()			Removes characters from the end of a string
	
	ChangeToLower()		Changes input string to lower case

    Usage()             Displays program usage information. Access using -h

