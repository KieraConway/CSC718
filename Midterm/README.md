Project:    Midterm - Question 4
File:       README.txt
Student:    Kiera Conway

Compile:

    mpicc ConwayQ4.c -o ConwayQ4


Usage:

	mpirun -np process_amount -machinefile machinefile.dsu ./ConwayQ4
	
		-np: specify number of processes

Contents:

	machinefile.dsu:	Contains list of machines
	
	README.md:			General information regarding project

	ConwayQ4.c:			Cyclical Allocation Summation MPI parallel program


	CSC718 Midterm - 2022:	Contains questions and answers to midterm - 
							Provided in .docx and .pdf for compatibility

Notes:
	
    -   Usage details above
		
	-	Must be compiled and executed inside the IA lab 

	-   Running time varies greatly on the IA lab machines
	
	-	After code is added to machine AND compiled, type the following 
		into the command line to update the other machines (all machines 
		must contain the file to execute MPI). You only need to do the 
		three machines you are not currently on.
			

		scp /path/to/file/ConwayQ4 mpiuser@Lincoln:/path/to/file/ConwayQ4
		scp /path/to/file/ConwayQ4 mpiuser@Washington:/path/to/file/ConwayQ4
		scp /path/to/file/ConwayQ4 mpiuser@Jefferson:/path/to/file/ConwayQ4
		scp /path/to/file/ConwayQ4 mpiuser@Roosevelt:/path/to/file/ConwayQ4