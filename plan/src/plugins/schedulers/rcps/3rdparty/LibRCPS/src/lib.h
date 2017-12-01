#ifndef LIB_H
#define LIB_H

#include <stdlib.h>

/* return the bigger of the two arguments */
static /*inline*/ int kpt_max(const int a, const int b) {
        return a > b ? a : b;
}

/* return the smaller of the two arguments */
static /*inline*/ int kpt_min(const int a, const int b) {
	return a < b ? a : b;
}

/* return a random number between 0 and max (both inclusive)*/
static /*inline*/ int irand(const int max) {
	return (int) (1.0*max*rand()/(RAND_MAX+1.0));
}

#endif /* LIB_H */
