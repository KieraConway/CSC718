/**
 * \file diningphilos.c
 * \brief Problem of dining philosophers using monitor-concept with pthreads-
 * mutex and pthreads-condvars
 * \author repat, <repat@repat.de>
 *
 * \note All comments for doxygen
*/

#include "diningphilos.h"

/**
 * \brief Main function that starts threads and listens for keyboard
 * input
 * \return 0 if programm exits normally
 */
int
main(void)
{
    pthread_t philoThreadIDs[NUMPHILO];
    int i;
    int err;
    int res[NUMPHILO];

//init stuff
    for(i = 0; i < NUMPHILO; i++) {
        philoStates[i] = THINK;
        stickStates[i] = FREE;
        err = pthread_cond_init(&cond[i], NULL);
        assert(!err);
        sem_init(&semaphore[i], 0, 0);
    }
    pthread_mutex_init(&mutex, NULL);

// start threads
    /*** for each philosopher ***/
    for(i = 0; i < NUMPHILO; i++) {

        /*** create a thread that does philo(), save return value in tmp[i] ***/
        res[i] =
            pthread_create(&philoThreadIDs[i], NULL, philo, &tmp[i]);
        tmp[i] = i;

        if(res[i] != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

// keyboard input
    while(TRUE) {
        fgets(keyinput, KEYKOMBO, stdin);

        if(keyinput[0] == 'q' || keyinput[0] == 'Q') {

        printf("Quitting...\n");
	    
	    printf("Unblocking all philos\n");
	    for(i = 0; i < NUMPHILO; i++) {
	      listen[i] = 'q';
	      sem_post(&semaphore[keyinput[0]-ASCII]);
	    }
        for(i = 0; i < NUMPHILO; i++) {
        	printf("Thread %d joined\n", i);
			pthread_cond_signal(&cond[i]);
            pthread_join(philoThreadIDs[i], NULL);
            }
            
        printf("Destroying the rest...\n");
        for(i = 0; i < NUMPHILO; i++) {
			pthread_cond_destroy(&cond[i]);
            sem_destroy(&semaphore[i]);
	    }
        pthread_mutex_destroy(&mutex);
        
	    printf("Exiting...\n");
        pthread_exit(NULL);
        exit(EXIT_FAILURE);    // programm stops here
        }
        /***
         *  If enter
         *  0-4b: block
         *  0-4u: unblock
         *  0-4p: proceed
         ***/
        else if(keyinput[0] >= '0' && keyinput[0] < NUMPHILO + ASCII) {
            if(keyinput[1] == 'b') {
                printf("Blocking Philo Nr. %d\n", keyinput[0] - ASCII);
                listen[keyinput[0] - ASCII] = keyinput[1];
            }
            else if(keyinput[1] == 'u') {
                printf("Unblocking Philo Nr. %d\n", keyinput[0] - ASCII);
                listen[keyinput[0] - ASCII] = 'u';
                sem_post(&semaphore[keyinput[0] - ASCII]);
            }
            else if(keyinput[1] == 'p') {
                printf("Proceeding Philo Nr. %d\n", keyinput[0] - ASCII);
                listen[keyinput[0] - ASCII] = 'p';
            }
        }
    }

    return 0;
}       // main

/**
 * \brief The main philosopher function
 * \param pID philosoher ID from thread creation
 * \return nothing
 */
void *
philo(void *pID)
{

    int *philoID = pID;
    int running = 1;            // init

    printf("philospher %d just awoke\n", *philoID);

    while(running) {

        /***
         * Check if thread is blocked,
         * if input_b enter sem_wait
        ***/
        checkForB(*philoID);

        /***
         * When unblocked,
         * Enter thinking loop for THINK_LOOP time
         * > loop breaks early if input_p
        ***/
        think(*philoID);

        /***
         * Check if thread is blocked,
         * if input_b enter sem_wait
        ***/
        checkForB(*philoID);

        /***
         * Locks pThread, calls disp_philo_states
         * ** Error Checks
         * ** displays philosophers states
         * ** T stands for THINK, H for HUNGRY and E for EAT
         * ** ** calls convertStates to do so
         * Sets philo to Hungry (hes trying to get sticks)
         * if either L or R stick is IN_USE,
         * enter conditional wait until both sticks available
         * sets philo to eating
         * sets sticks to IN_USE
         * ***/
        get_sticks(*philoID);

        /***  ***/
        eat(*philoID);

        /***
         * Check if thread is blocked,
         * if input_b enter sem_wait
        ***/
        checkForB(*philoID);

        /***
         * Mutex is locked
         * Current philo states checked for errors, converted, and displayed
         * Stick states are updated from 'IN_USE' to 'FREE'
         * Philosopher state is set to THINK
         * Signals are sent out to notify any conditional waits that the sticks are now available
         * Mutex is unlocked
        ***/
        put_sticks(*philoID);

        /***  ***/
        if(listen[*philoID] == 'q' || listen[*philoID] == 'Q') {
            running = 0;
        }
    }
    return NULL;
}



/**
 * \brief Simulates thinking by going through an empty loop
 * \param philoID philosoher ID from thread creation
 * \return nothing
 */
void
think(int philoID)
{
    int i;

    for(i = 0; i < THINK_LOOP; i++) {

        /***
         * listen for block, unblock, *proceed*
         ***/
        if(listen[philoID] == 'p') {
            /*** if 'proceed', set to 'd' ***/
            listen[philoID] = 'd';
            break;
        }
    }
}

/**
 * \brief Simulates eating by going through an empty loop
 * \param philoID philosoher ID from thread creation
 * \return nothing
 */
void
eat(int philoID)
{
    int i;
    for(i = 0; i < EAT_LOOP; i++) {
        if(listen[philoID] == 'p') {
            listen[philoID] = 'd';
            break;
        }
    }
}

/**
 * \brief Checks if Thread is blocked
 * \param philoID philosoher ID from thread creation
 * \return nothing
 */
void
checkForB(int philoID)
{
    if(listen[philoID] == 'b') {
        /***
         * decrements S if it’s greater than zero (there are available resources to allocate).
         * If S is already zero (there are no available resources to allocate),
         * the calling process is put to sleep.
        ***/
        ////////KC CODE
/*        printf("enter sem wait [%i]\n", philoID);

        int semValue;
        sem_getvalue(&semaphore[philoID], &semValue);
        printf("after getVal [%i]\n", semValue);*/
        ///////end kc code

        sem_wait(&semaphore[philoID]);

        ////////KC CODE
/*        printf("return sem wait (%i)", philoID);
        sem_getvalue(&semaphore[philoID], &semValue);
        printf("after2 getVal [%i]\n", semValue);*/
        ///////end kc code
    }
}
