Project:    Homework 4
File:       README.txt
Authors:    Kiera   Conway

Compile:

	Each folder contains a Makefile for the corresponding program. They can be compiled/ removed using the following commands:

        > make

        > make clean
            


Usage:

	Each program contains unique commands to execute, they are as follows:
	
		student_seq.c
		
			> ./student_seq
		
		student_mpi.c
		
			> ./mpirun -np <nProcesses> -machinefile <machineFile> ./student_mpi
			
			nProcesses:		number of processes; Required
			machineFile: 	possible machines on which to run MPI program; Required
		
		student_omp.c
		
			> ./student_omp -v -t <nThreads>
			
			-v:				verbose
							[Default: false]
			
			nThreads:		number of threads; Optional
							[Default: 5]

Contents:

	FOLDERS -
		mpi
			An MPI program counting 'acceptable' unique student id numbers
		
		omp
			An openMP program counting 'acceptable' unique student id numbers
			
		sequential
			A Sequential Program for counting 'acceptable' unique student id numbers


	FILES -
		Conway, Homework 4 Questions [Final]:		
			Document which contains answers to all homework questions.
			Attached in both a .pdf and .docx for compatibility
	
		outputFile.md:
		outputFile.txt:
			Example output of each program
			Attached in both a .md and .txt for compatibility


		README.txt:
			[Current File] 
			General information regarding project


Notes:
    -   General usage details above, specific usage details in corresponding
		README files - all arguments are optional

	-   Running time depends on max processes or thread connections.  Runtime is
	    printed at end of each program.
				
	-	Both 'outputFile's contain the same information. They are formatted differently 
		to help digest the information.
		
	MPI Program Only:
			-	Machine File is provided in mpi folder (machinefile.dsu)
			-	Must be compiled and executed inside the IA lab 
			-   Running time varies greatly on the IA lab machines
			-	After the program is compiled, the make file should 
				automatically update the other machines (all machines 
				must contain the file to execute MPI).  However, if this fails, 
				you can do it manually using the commands below.
	
					scp /path/to/file/HW2-# mpiuser@Lincoln:/path/to/file/
					scp /path/to/file/HW2-# mpiuser@Washington:/path/to/file/
					scp /path/to/file/HW2-# mpiuser@Jefferson:/path/to/file/
					scp /path/to/file/HW2-# mpiuser@Roosevelt:/path/to/file/
		
				You only need to do the three machines you are not currently on.
				
				There is an optional bash script included to streamline this 
				process. However, the file paths and names must be changed prior 
				to execution