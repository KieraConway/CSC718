Project:    proj_seq.c
File:       README.txt
Authors:    Kiera   Conway

Compile:

    Program compiled using Makefile - commands are copied below

        > make
            gcc -g -Wall -o proj_seq proj_seq.c

        > make clean
            rm -f proj_seq


Usage:

	proj_seq -h -c -v -f <file> -s <string>

		-h:            Display Usage summary    Example: proj_seq -h

		-c:            Set Case Sensitivity     Default: False        |   Example: proj_seq -c
		-v:            Set Verbose Mode         Default: False        |   Example: proj_seq -v

		-f <file>:     Specify File             Default: testFile.txt |   Example: proj_seq -f myFile.txt
		-s <string>:   Specify Search Term      Default: life         |   Example: proj_seq -s search_term

    Note: 
		-	All Arguments are Optional (Defaults are used if no arguments are set)


Contents:

	makefile:			Automates compilation and linking process

    proj_seq.c:			Contains functions that implement a sequential file parser

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
							(4)	Open File
                            (5) Parse File
                            (6) Display Final Data
                            (7) Calculate and Print Runtime
	
	ProcessArgs()		Process user arguments
	
	TrimLeft()			Removes characters from the beginning of a string
	
	TrimRight()			Removes characters from the end of a string
	
	ChangeToLower()		Changes input string to lower case

    Usage()             Displays program usage information. Access using -h

