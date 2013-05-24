#ifndef DECODE_H
#define DECODE_H

#include "structs.h"

/* decode a genome. in the moment this stores its findings in
 * the problem structure */
struct rcps_phenotype *decode(struct rcps_solver *solver,
	struct rcps_problem *problem, struct rcps_genome *genome);

#endif /* DECODE_H */
