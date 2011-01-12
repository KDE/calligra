#include <assert.h>
#include <malloc.h>
#include <stdio.h>

#include "fitness.h"
#include "librcps.h"

int fitness(struct rcps_problem *problem, struct rcps_genome *genome,
	struct rcps_phenotype *pheno) {
	int result = 0.0;
    int max = 0;
	int i, j;

//    printf("Fitness: ");
	switch (problem->fitness_mode) {
	case FITNESS_WEIGHT:
		for (i = 0; i < problem->job_count; i++) {
            struct rcps_job *job = problem->jobs[genome->schedule[i]];
            int job_start = pheno->job_start[job->index];
            int dur = pheno->job_duration[job->index];
            int job_end = job_start + dur;
			int modenum = job->genome_position >= 0 ?
					genome->modes[job->genome_position] : 0;
            int weight = problem->weight_callback ?
                    problem->weight_callback(job_start, dur, job->weight, job->modes[modenum]->weight_cb_arg) :
                    job->weight * ( job_start * dur );

			result += weight;
/*            printf("%s (%d)", job->name, weight);
            if ( (i + 1) < problem->job_count ) {
                printf(" -> ");
            }*/
        }
		break;
	default:
		for (i = 0; i < problem->job_count; i++) {
			int curr = pheno->job_start[problem->jobs[i]->index];
			int modenum = problem->jobs[i]->genome_position >= 0 ?
				genome->modes[problem->jobs[i]->genome_position] : 0;
			curr += problem->jobs[i]->modes[modenum]->duration;
			if (curr > max) {
				max = curr;
			}
		}
		result = max;
		break;
	}
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
		result += pheno->overuse_count * bound + pheno->overuse_amount * 3;
	}
//     printf(" : total weight = %f\n", result);
	return result;
}
