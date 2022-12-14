Project:    Final
File:       README.txt
Authors:    Kiera   Conway

Compile:

	Each folder contains a Makefile for the corresponding program. They can be compiled/ removed using the following commands:

        > make

        > make clean
            


Usage:

	Each program contains unique commands to execute, they are as follows:
	
		hps.c
			> ./hps n d
				n: 	value of N 			[Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5]
				d: 	numbers of Digits 	[Example: 5 => 0,xxxxx]

		
		hps_mpi.c
		
			> ./mpirun -np <nProcesses> -machinefile <machineFile> ./hps_mpi n d
			
				 nProcesses:	number of processes
				machineFile: 	possible machines on which to run MPI program
						  n: 	value of N 			[Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5]
				          d: 	numbers of Digits 	[Example: 5 => 0,xxxxx]
		hps_omp.c
		
			> ./hps n d t

				n: 	value of N 			[Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5]
				d: 	numbers of Digits 	[Example: 5 => 0,xxxxx]
				t:	number of threads	[Default: 5]

Contents:

	FOLDERS -
		mpi
			An MPI program which calculates harmonic progression sum
		
		omp
			An openMP program which calculates harmonic progression sum
			
		sequential (source_code)
			A Sequential Program which calculates harmonic progression sum


	FILES -
		Conway, Final Questions [Final]:		
			Document which contains answers to all final questions.
			Attached in both a .pdf and .docx for compatibility
	
		outputFile.md:
		outputFile.txt:
			Example output of each program
			Attached in both a .md and .txt for compatibility


		README.txt:
			[Current File] 
			General information regarding project


Notes:
    -   Usage details shown above - all arguments are REQUIRED

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
				
				Note: Ensure the file path exists on each machine. You can create the 
				path manually by ssh-ing into the machine and using the mkdir command.
				See follwoing example:
					> ssh mpiuser@Washington
					> mkdir ~/Documents/Homework1/
