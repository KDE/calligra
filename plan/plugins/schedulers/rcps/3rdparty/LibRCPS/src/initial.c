#ifdef _WIN32
# include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <stdlib.h>
#include <assert.h>

#include "initial.h"
#include "lib.h"

/* generate a random initial schedule where all predecessor
 * dependencies are fulfilled. we do this by storing a set with all
 * jobs, their dependency count and how many dependecies are already 
 * scheduled in a linked list sorted by the number of unfillfiled 
 * dependencies. we can then in turn choose one of the jobs where that 
 * number is 0, and then update the list
 * */

struct initial_job {
	int job_number;
	int left_deps;
	struct initial_job **successors;
	struct initial_job *next;
	struct initial_job *prev;
};

/* XXX this could be sped up by not regenrating the structure every single 
 * time this is run */
int *random_schedule(struct rcps_problem *problem) {
	int *result;
	int cproblem;
	int i, j, c;
	int zcount;
	struct initial_job *njob;
	struct initial_job *cjob;
	struct initial_job *ojob;
	struct initial_job *job_list;

	struct initial_job *A;
	struct initial_job *B;
	struct initial_job *C;

	/* empty result set */
	result = (int*)malloc(sizeof(int) * problem->job_count);
	cproblem = 0;

	/* init the list */
	job_list = NULL;
	for (i = 0; i < problem->job_count; i++) {
		/* create new job */
		njob = (struct initial_job*)malloc(sizeof(struct initial_job));
		njob->job_number = i;
		njob->left_deps = problem->jobs[i]->predeccessor_count;
		njob->next = NULL;
		njob->prev = NULL;
		if (problem->jobs[i]->successor_count > 0) {
			njob->successors = (struct initial_job**)malloc(sizeof(struct initial_job *) * 
				problem->jobs[i]->successor_count);
		}
		else {
			njob->successors = NULL;
		}
		/* insert into the list */
		if (job_list == NULL) {
			job_list = njob;
		}
		else {
			/* go through the list */
			ojob = NULL;
			cjob = job_list;
			while ((cjob != NULL) && (cjob->left_deps < njob->left_deps)) {
				ojob = cjob;
				cjob = cjob->next;
			}
			/* insert */
			if (cjob == NULL) {
				ojob->next = njob;
				njob->prev = ojob;
			}
			else if (ojob == NULL) {
				njob->next = job_list;
				job_list = njob;
			}
			else {
				njob->next = cjob;
				njob->prev = ojob;
				ojob->next = njob;
				cjob->prev = njob;
			}
		}
	}
	/* fix the successors */
	cjob = job_list;
	while (cjob) {
		for (i = 0; i < problem->jobs[cjob->job_number]->successor_count; i++) {
			for (j = 0; j < problem->job_count; j++) {
				if (problem->jobs[j] == 
						problem->jobs[cjob->job_number]->successors[i]) {
					/* update with successor j */
					ojob = job_list;
					while ((ojob) && (ojob->job_number != j)) {
						ojob = ojob->next;
					}
					cjob->successors[i] = ojob;
					break;
				}
			}
		}
		cjob = cjob->next;
	}
	/* get the zero count */
	cjob = job_list;
	zcount = 0;
	while ((cjob) && (cjob->left_deps == 0)) {
		zcount++;
		cjob = cjob->next;
	}
	/* work your way through the list */
	c = 0;
	while (zcount != 0) {
		/* choose the proper job */
		/* XXX randomly for now, insert heuristics here */
		i = irand(zcount);
		cjob = job_list;
		while (i > 0) {
			cjob = cjob->next;
			i--;
		}
		/* remove from list */
		if (cjob == job_list) {
			job_list = cjob->next;
			if (job_list != NULL) {
				job_list->prev = NULL;
			}
		}
		else {
			if (cjob->next != NULL) {
				cjob->next->prev = cjob->prev;
			}
			if (cjob->prev != NULL) {
				cjob->prev->next = cjob->next;
			}
		}
		zcount--;
		/* add to result */
		result[c] = cjob->job_number;
		c++;
		/* update the list */
		for (i = 0; i < problem->jobs[cjob->job_number]->successor_count; i++) {
			ojob = cjob->successors[i];
			ojob->left_deps--;
			/* resort */
			while ((ojob->prev != NULL) && (ojob->prev->left_deps > ojob->left_deps)) {
				/* shift down */
				C = ojob->next;
				B = ojob->prev;
				A = B->prev;
				if (C) {
					C->prev = B;
				}
				B->prev = ojob;
				B->next = C;
				ojob->next = B;
				ojob->prev = A;
				if (A) {
					A->next = ojob;
				}
				else {
					job_list = ojob;
				}
			}
			/* update zcount */
			if (ojob->left_deps == 0) {
				zcount++;
			}
		}
		/* clean up */
		free(cjob->successors);
		free(cjob);
	}
	return result;
}

void initial(struct rcps_problem *problem, struct rcps_genome *genome) {
	int i;
	/* XXX assert that he structs in the genome are not yet mallocd */
	/* the schedule */
	genome->schedule = random_schedule(problem);
	/* the modes */
	genome->modes = (int*)malloc(problem->genome_modes * sizeof(int));
	for (i = 0; i < problem->genome_modes; i++) {
		if (problem->modes_max[i] == 0) {
			/* XXX this should never happen, assert it and remove later */
			assert(0 == 1);
		}
		else {
			genome->modes[i] = irand(problem->modes_max[i]); 
		}
	}
	/* the alternatives */
	genome->alternatives = (int*)malloc(problem->genome_alternatives * 
			sizeof(int));
	for (i = 0; i < problem->genome_alternatives; i++) {
		if (problem->alternatives_max[i] == 0) {
			/* XXX this should never happen, assert it and remove later */
			assert(0 == 1);
		}
		else {
			genome->alternatives[i] = irand(problem->alternatives_max[i]);
		}
	}
}
