Project:    Final Project
File:       README.txt
Authors:    Kiera   Conway

Compile:

	Specific program compilation commands are available in their corresponding 
	folders. However, each file can be compiled/ removed using provided Makefiles.

        > make

        > make clean
            


Usage:

	In depth review of specific program commands can be accessed in their 
	corresponding folders or by using the `-h` flag:
		
		`program_name.c -h`
		
	An overview of program commands are below:
	
	
		> proj_seq -h -c -v -f <file> -s <string>
		
		> proj_sem -h -c -v -t <1-10> -f <file> -s <string>

		> proj_omp -h -c -v -t <1-10> -f <file> -s <string>


    Note: 
		-	All Arguments are Optional (Defaults are used if no arguments are set)


Contents:

	FOLDERS -
	
		1 - Sequential
			A Sequential Program for finding occurrences of a specified search term 
			in a large file
			
		2 - Semaphore
			A Multithreaded Program for finding occurrences of a specified search term 
			in a large file
			
		3 - OMP
			An openMP Program for finding occurrences of a specified search term in a 
			large file

	FILES -
		Conway, Kiera - HW3 Questions [Final]:		
			Document which provides an overview of the project including file contents, 
			purpose, benchmarking, review, and program process, compilation, and 
			execution.
			Attached in both a .pdf and .docx for compatibility
	
		outputFile.md:
		outputFile.txt:
			Example output of each program
			Attached in both a .md and .txt for compatibility


		README.md:
			[Current File] 
			General information regarding project


Notes:
    -   General usage details above, specific usage details in corresponding
		README files - all arguments are optional

	-   Running time depends on max thread connections.  Runtime is
	    printed at end of each program.
		
	-	testFile.txt is a randomly generated AI text file, the information 
		within is nonfactual and mostly nonsensical.
		
	-	Both 'outputFile's contain the same information. They are formatted differently 
		to help digest the information.