#include <assert.h>
#include <malloc.h>
#include <stdio.h>

#include "fitness.h"
#include "librcps.h"

int fitness(struct rcps_problem *problem, struct rcps_genome *genome, 
	struct rcps_phenotype *pheno) {
	int max = 0;
	int i, j;

	switch (problem->fitness_mode) {
	case FITNESS_WEIGHT:
		for (i = 0; i < problem->job_count; i++) {
			int job_start = pheno->job_start[problem->jobs[i]->index];
			int modenum = problem->jobs[i]->genome_position >= 0 ?
					genome->modes[problem->jobs[i]->genome_position] : 0;
            int dur = problem->jobs[i]->modes[modenum]->duration;
			int job_end = job_start + dur;
            int weight = problem->weight_callback ?
                    problem->weight_callback(job_start, dur, problem->jobs[i]->weight, problem->jobs[i]->modes[modenum]->weight_cb_arg) :
                    problem->jobs[i]->weight;

			max += job_end * weight;
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
		max += pheno->overuse_count * bound + pheno->overuse_amount * 3;
	}
	return max;
}
