Project:    HW3
File:       README.txt
Authors:    Kiera Conway

Contents:

	FOLDERS -
	
		source code:
			contains the original p1-p4.c files


	FILES -
		Conway, Kiera - HW3 Questions [Final]:		
			Document which contains answers to all homework questions. Attached in 
			both a .pdf and .docx for compatibility
		
		README.md: 								
			(Current Document) General information regarding project
			
		running.txt:
			Output for all programs p1-p4, openMP and sequential
			
		omp_pX:
		seq_pX:
			Program files in order from 1 to 4 (p1, p2, etc) where omp is the
			openMP version and seq is the sequential version

		Makefile:
			make file available for openMP programs (pX_omp). Usage:
			
				- make specific program executable 
					make program
		
				- make all pX_omp executables
					make
					make all 
			
				- remove all pX_omp executables
					make clean
Usage:

	pX_omp:
	
		gcc -O3 -fopenmp pX_omp.c -o pX_omp		// to compile
		./pX_omp								// to run
		
		or use makefile (instructions above)
	
	pX_seq:
		
		gcc pX_seq.c -o pX_seq					// to compile
		./pX_seq								// to run
	