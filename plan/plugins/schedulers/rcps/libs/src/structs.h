#ifndef STRUCTS_H
#define STRUCTS_H

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#include "librcps.h"
#include "slist.h"

// XXX perhaps we need an index in most sstructs so that resources[i]->index==i
// this would be cool in repair

struct rcps_problem {
	struct rcps_resource **resources;
	int resource_count;
	struct rcps_job **jobs;
	int job_count;
	/* pre-calculated values */
	int genome_modes;
	int *modes_max;
	int genome_alternatives;
	int *alternatives_max;
	/* fitness calculation mode */
	int fitness_mode;
	/* weight callback */
	int (*weight_callback)(int starttime, int duration, struct rcps_fitness *nominal_weight, void *arg, void *fitness_arg);
	/* fitness callback, set up a user defined fitness structure and return a pointer to it */
	void *(*fitness_callback_init)(void *fitness_init_arg);
    /* the argument returned by fitness_callback_init */
    void *fitness_init_arg;
    /* fitness callback
     * fill in the result of the fitness calculation into fitness.
     * arg is the argument returned by fitness_callback_init  */
    int (*fitness_callback_result)(struct rcps_fitness *fitness, void *arg);
};

struct rcps_resource {
	char *name;
	int type;
	int avail;
	/* pre-calculated values */
	int index;
};

struct rcps_job {
	char *name;
	struct rcps_job **successors;
	int *successor_types;
	int successor_count;
	struct rcps_job **predeccessors;
	int *predeccessor_types;
	int predeccessor_count;
	struct rcps_mode **modes;
	int mode_count;
	/* result */
	int res_start;
	int res_mode;
	/* pre-calculated values */
	int genome_position;
	int index;
	/* fitness calculations */
	int weight;
	/* time constraints etc. */
	int earliest_start;
};

struct rcps_mode {
	int duration;
    /* duration callback argument */
	void *cb_arg;
	struct rcps_request **requests;
	int request_count;
    /* weight callback argument */
    void *weight_cb_arg;

};

struct rcps_request {
	struct rcps_alternative **alternatives;
	int alternative_count;
	/* result */
	int res_alternative;
	/* pre-calculated values */
	int genome_position;
};

struct rcps_alternative {
	int amount;
	struct rcps_resource *resource;
};

/* please note that the number of modes and alternatives here is not 
 * necessarily the same as in the problem definition, because we do not put
 * alternatives or modes with only one possibility into the genome */
struct rcps_genome {
	int *schedule;
	int *modes;
	int *alternatives;
};

struct rcps_phenotype {
	int overuse_count;
	int overuse_amount;
	int *job_start;
    int *job_duration;
};

struct rcps_individual {
    struct rcps_fitness fitness;
	struct rcps_genome genome;
};

struct rcps_population {
	int size;
	struct slist *individuals;
};

struct rcps_solver {
	// this contains a job_count * job_count array
	char *predecessor_hash;
	// population size and mutation rates
	int pop_size;
	int mut_sched;
	int mut_mode;
	int mut_alt;
	// the number of parallel jobs
	int jobs;
	// save the number of reproductions needed by the solver here
	int reproductions;
	// set to != 0 if the computed schedule is not valid
	int warnings;
	// the actual population;
	struct rcps_population *population;
	// progress callback and data
	int (*progress_callback)(int generations, struct rcps_fitness fitness, void *arg);
	void *cb_arg;
	int cb_steps;
	// duration callback
	int (*duration_callback)(int direction, int starttime, 
		int nominal_duration, void *arg);
	// the lock for multithreading
#ifdef HAVE_PTHREAD
	pthread_mutex_t lock;
#endif
};

#endif /* STRUCTS_H */
