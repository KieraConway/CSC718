Project:    HW2
File:       README.txt
Authors:    Kiera Conway

Compile:

    mpicc HW2-#.c -o HW2-#


Usage:

	mpirun -np process_amount -machinefile machinefile.dsu ./ConwayQ4
	
		-np: specify number of processes
		
Contents:


	HW2-1:
		Sequential Program 
		
		Finds the number of times that two consecutive
		odd integers are both prime numbers from 2 to 1,000,000

	HW2-2:
		Parallel Program using openMPI

		Finds the number of times that two consecutive
		odd integers are both prime numbers from 2 to 1,000,000

	HW2-3:
		Parallel Program using openMPI

		Finds the largest gap between a pair of consecutive
		prime numbers for all integers from 2 to 1,000,000
		
	HW2-4:
		Parallel Program using openMPI

		computer π using the rectangle rule with 1,000,000 intervals.

	machinefile.dsu:	Contains list of machines
	
	Conway, Kiera - HW2 Questions [Final]:		
		Document which contains screenshots for all programs listed above
		Saved as docx and pdf to ensure compatibility
	
	README.txt: 								
		General information regarding project
Notes:
    -   This README covers all programs and files in the folder, please refer 
		for compilation and usage details, as well as other notes regarding 
		the assignments
		
	-   Usage details above
		
		-	Usage details for HW2-1 may also be accessed using "-h" argument
			after program name
				Example: HW2-1 -h

	-	Must be compiled and executed inside the IA lab 

	-   Running time varies greatly on the IA lab machines
	
	-	After code is added to machine AND compiled, type the following 
		into the command line to update the other machines (all machines 
		must contain the file to execute MPI). You only need to do the 
		three machines you are not currently on.
			

		scp /path/to/file/HW2-# mpiuser@Lincoln:/path/to/file/
		scp /path/to/file/HW2-# mpiuser@Washington:/path/to/file/
		scp /path/to/file/HW2-# mpiuser@Jefferson:/path/to/file/
		scp /path/to/file/HW2-# mpiuser@Roosevelt:/path/to/file/
		
		- Note: there is an optional bash script included to streamline this 
				process. However, the file paths and names must be changed prior 
				to execution