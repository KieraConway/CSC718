/**
 * \file monitor.c
 * \brief monitor functions for diningphilos
 * \author repat, repat@repat.de
 *
 * \note All comments for doxygen
*/

#include "diningphilos.h"

//!states of the N philosophers
State philoStates[NUMPHILO];
//!states of the N sticks
Sticks stickStates[NUMPHILO];

//!mutual exclusion with pthreads
pthread_mutex_t mutex;
//!condition variables with pthreads - one for every philosopher
pthread_cond_t cond[NUMPHILO];
//!semaphors with pthreads - one for every philosopher
sem_t semaphore[NUMPHILO];
//!philoIDs
int tmp[NUMPHILO];

//!For keyboard input
char keyinput[KEYKOMBO];
//!For transmitting b, u or p to the philosophers
char listen[NUMPHILO];

/**
 * \brief philosopher tries to get both sticks or waits for sticks to become
 * available(on HIS cond-var), then eats
 * \param philoID philosoher ID from thread creation
 * \return nothing
 */
void
get_sticks(int philoID)
{

    //lock and display states
    pthread_mutex_lock(&mutex);
    disp_philo_states();

    // when he tries to get the sticks he's obviously hungry
    philoStates[philoID] = HUNGRY;

    while(stickStates[LEFT(philoID)] == IN_USE
          || stickStates[RIGHT(philoID)] == IN_USE) {
        //printf("INUSE [%i] (L%i:R%i]\n", philoID);      //KC CODE
        pthread_cond_wait(&cond[philoID], &mutex);
        //printf("no longer [[%i]]\n", philoID);          //KC CODE
    }

    // mark philosopher as eating and stick as in use
    philoStates[philoID] = EAT;
    stickStates[LEFT(philoID)] = IN_USE;
    stickStates[RIGHT(philoID)] = IN_USE;

	//unlock the mutex
    pthread_mutex_unlock(&mutex);
}

/**
 * \brief philosopher puts down the sticks, nudges his fellow philo-buddies and
 * enters thinking phase again
 * \param philoID philosoher ID from thread creation
 * \return nothing
 */
void
put_sticks(int philoID)
{

    // again, lock and display states
    pthread_mutex_lock(&mutex);
    disp_philo_states();

    // lets go of his sticks
    stickStates[LEFT(philoID)] = FREE;
    stickStates[RIGHT(philoID)] = FREE;

    // goes back to thinking
    philoStates[philoID] = THINK;

    // nudges his philosopher buddies next to him
    pthread_cond_signal(&cond[LEFTNEIGHB(philoID)]);
    pthread_cond_signal(&cond[RIGHTNEIGHB(philoID)]);

    pthread_mutex_unlock(&mutex);
}

/**
 * \brief Displays what happens inside the monitor like this
 * OT 1H 2E 3T 4T
 * in which T stands for THINK, H for HUNGRY and E for EAT
 * \return nothing
 */
void
disp_philo_states()
{
    int i;

    // go through them and look if there's an error
    for(i = 0; i < NUMPHILO; i++) {
        if(philoStates[i] == EAT
           && (philoStates[LEFTNEIGHB(i)] == EAT
               || philoStates[RIGHTNEIGHB(i)] == EAT)) {
            printf("OUPS! Something went wrong...\n\n");
            break;
        }
    }

    // display anyway to see what might be wrong or actual states
    for(i = 0; i < NUMPHILO; i++) {
        printf("%d%c ", i, convertStates(philoStates[i]));
    }
    printf("\n");
}

/**
 * \brief Converts the states into their first letter
 * \param philoState state the philosoher is in(THINK, HUNGRY or EAT)
 * \return T/H/E for THINK, HUNGRY or EAT
 */
char
convertStates(State philoState)
{
    if(philoState == EAT) {
        return 'E';
    }
    else if(philoState == THINK) {
        return 'T';
    }
    else {
        return 'H';
    }
    return '-'; // error
}