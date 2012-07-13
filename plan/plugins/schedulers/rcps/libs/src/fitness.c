#include <assert.h>
#ifdef _WIN32
# include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>

#include "fitness.h"
#include "librcps.h"

struct rcps_fitness fitness(struct rcps_problem *problem, struct rcps_genome *genome,
	struct rcps_phenotype *pheno) {

	int max = 0;
	int i, j;
	void *fitness_cb_arg = 0;
	struct rcps_fitness result;
	result.group = FITNESS_MAX_GROUP;
	result.weight = 0;

	/*printf("Fitness: ");*/
	switch (problem->fitness_mode) {
		case FITNESS_WEIGHT: {
			if ( problem->fitness_callback_init ) {
				fitness_cb_arg = problem->fitness_callback_init( problem->fitness_init_arg );
			}
			for (i = 0; i < problem->job_count; i++) {
				struct rcps_job *job = problem->jobs[genome->schedule[i]];
				int job_start = pheno->job_start[job->index];
				int dur = pheno->job_duration[job->index];
				int job_end = job_start + dur;
				int modenum = job->genome_position >= 0 ? genome->modes[job->genome_position] : 0;
				struct rcps_fitness weight;
				weight.group = 0;
				weight.weight = job->weight;
				if (problem->weight_callback) {
					problem->weight_callback(job_start, dur, &weight, job->modes[modenum]->weight_cb_arg, fitness_cb_arg);
				} else {
					weight.weight *= job_end;
				}
				if ( weight.group == result.group ) {
					result.weight += weight.weight;
				} else if ( weight.group < result.group ) {
					result.group = weight.group;
					result.weight = weight.weight;
				}
				/*printf("%s (%d)", job->name, weight); if ( (i + 1) < problem->job_count ) { printf(" -> "); }*/
			}
			if ( problem->fitness_callback_result ) {
				problem->fitness_callback_result(&result, fitness_cb_arg);
			}
			break;
		}
		default: {
			for (i = 0; i < problem->job_count; i++) {
				int curr = pheno->job_start[problem->jobs[i]->index];
				int modenum = problem->jobs[i]->genome_position >= 0 ?
					genome->modes[problem->jobs[i]->genome_position] : 0;
				curr += problem->jobs[i]->modes[modenum]->duration;
				if (curr > max) {
					max = curr;
				}
			}
			result.weight = max;
			// check if we have overused nonrenewable resources
			if (pheno->overuse_count) {
				// get an upper bound on the project fitness
				int bound = 0;
				for (i = 0; i < problem->job_count; i++) {
					int max_duration = 0;
					for (j = 0; j < problem->jobs[i]->mode_count; j++) {
						struct rcps_mode *cmode = problem->jobs[i]->modes[j];
						if (cmode->duration > max_duration) {
							max_duration = cmode->duration;
						}
					}
					bound += max_duration;
				}
				// and add the bound to the fitness as a penalty for overusing resources
				// XXX the overuse multiplier should be configurable?
				result.weight += pheno->overuse_count * bound + pheno->overuse_amount * 3;
			}
			break;
		}
	}
// 	printf(" : total weight = [%d, %d]\n", result.group, result.weight);
	return result;
}
