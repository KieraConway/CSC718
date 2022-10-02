/*
 * $Id: random_int.c,v 1.2 2000/01/07 16:42:26 senning Exp $
 * 
 * Returns a random integer, exponentially distributed about mean with range
 * of values truncated to lie between 1 and 5 * mean.  The first time this
 * routine is called it initialized the random number generate with 
 * 
 * See http://www.lter.umn.edu/tools/utility/rexpon.d for a justification of
 * the formula used here.
 * 
 * Written by R. Bjork
 * Modified and Extended by J. Senning 2/22/98
 * 
 */

#include <stdlib.h>
#include <math.h>
#include <time.h>

/*
 * Static variable to record the number of times the generator has been
 * initialized.
 */

static int initialized = 0;

/*--------------------------------------------------------------------------*/

void init_random_int( int seed )
/*
 * Initializes the random number generator.  If seed is negative then the
 * system clock is used to initialize the generator.  A count is also kept
 * of the number of times this routine has been called.
 */
{
    if ( seed < 0 ) {
	seed = (int) time( (time_t *) 0 );
    }
    srandom( seed );
    initialized++;
}

/*--------------------------------------------------------------------------*/

int random_int( int mean )
/*
 * Computes a random integer from an exponetial distribution with a
 * specified mean.  If this routine is called and the generator has not
 * yet been initialized, it initializes it using the system clock.
 */
{
    double u;
    int value;
    
    if ( !initialized ) {
	init_random_int( -1 );
    }

    /*
     * Random number from uniform distibution on [0,1)
     */
    u = (double) ( ( random() & RAND_MAX ) / ( 1.0 + RAND_MAX ) );

    /*
     * Find number from exponentially distribution with specified mean
     * and round to an integer.
     */
    value = (int) ( 0.5 - mean * log( u ) );

    if ( value == 0 ) {
	value = 1;
    } else if ( value > 5 * mean ) {
	value = 5 * mean;
    }

    return value;
}

/*--------------------------------------------------------------------------*/
