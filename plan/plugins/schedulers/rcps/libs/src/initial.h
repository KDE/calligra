#ifndef INITIAL_H
#define INITIAL_H

#include "structs.h"

/* XXX we need other ways to create initial solution, not only random ones.
 * ideas would be heuristic ones, or user-defined */

/* create an random solution */
void initial(struct rcps_problem *p, struct rcps_genome *g);

#endif /* INITIAL_H */
