/*
 * $Id: msecond.c,v 1.2 2000/01/07 16:42:08 senning Exp $
 * 
 * Written by J. Senning -- Feburary 1998
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>

int msecond( void )
/*
 * The first time msecond() is called it returns 0.  For all subsequent
 * calls it returns the number of milliseconds that have elapsed since
 * the first call.
 * 
 * The times() system call is used to obtain the number of clock cycles
 * that have elapsed since some point in the past (usually system boot
 * time).  This number is then divide by HZ which is a system-dependent
 * constant from <sys/params.h> that gives the number of clock cycles per
 * second.  Finally, we multiply by 1000 to convert to milliseconds.  The
 * upshot of all this is that msecond() does not return values with
 * millisecond accuracy.  On Linux systems and the SGI O2 the value of HZ
 * is 100 so that the resolution of this function is about 10 milliseconds.
 */
{
    static clock_t start = (clock_t) 0;
    struct tms buf;

    if ( start == (clock_t) 0 )
    {
	start = times( &buf );		/* First time */
	return 0;
    } else {
	return (int) ( ( 1000 / HZ ) * ( times( &buf ) - start ) );
    }
}
