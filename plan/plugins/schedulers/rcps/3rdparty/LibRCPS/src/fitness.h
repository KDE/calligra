#ifndef FITNESS_H
#define FITNESS_H

#include "structs.h"

/* calculate the fitness of the current solution, some of the 
 * data is stored in problem! */
struct rcps_fitness fitness(struct rcps_problem *problem, struct rcps_genome *genome,
	struct rcps_phenotype *pheno);

#endif /* FITNESS_H */
