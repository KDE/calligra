#ifndef OPS_H
#define OPS_H

/* this defines the different mutation and crossover operators that we use.
 * basically we have two different kind of chromosomes: lists that can be
 * reordered (the schedule) and lists of numbers that have an upper bound 
 * each (the modes and alternatives). 
 * */

#include "structs.h"

/* combine two schedules with one "breaking point" */
void sched_crossover(struct rcps_solver *solver, struct rcps_problem *problem, 
	int *father, int *mother, int *son, int *daughter);

/* combine two schedules with two "breaking points" */
void sched_crossover2(struct rcps_solver *solver, struct rcps_problem *problem, 
	int *father, int *mother, int *son, int *daughter);

/* swap each entry in the schedule with the one after it with probability
 * p/10000 if this creates a valid schedule */
void sched_mutation(struct rcps_solver *solver, struct rcps_problem *problem, 
	int *schedule, int p);

/* combine two integer arrays with one "breaking point" */
void crossover(int *father, int *mother, 
		int *son, int *daughter, int size);

/* same, with two "breaking points" */
void crossover2(int *father, int *mother, 
		int *son, int *daughter, int size);

/* change each entry in the array to a rabdom value with probability p/10000 
 * */
void mutation(int *data, int *data_max, int size, int p);

#endif /* OPS_H */
