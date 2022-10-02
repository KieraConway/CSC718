#ifndef __RANDOM_INT_H__
#define __RANDOM_INT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

void init_random_int( int seed );
/*
 * Initializes the random number generator.  If seed is negative then the
 * system clock is used to initialize the generator.  A count is also kept
 * of the number of times this routine has been called.
 */

/*--------------------------------------------------------------------------*/

int random_int( int mean );
/*
 * Computes a random integer from an exponetial distribution with a
 * specified mean.  If this routine is called and the generator has not
 * yet been initialized, it initializes it using the system clock.
 */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __RANDOM_INT_H__ */
