#ifdef _MSC_VER
#define inline __inline
#endif

#include <stdlib.h>
#include <assert.h>

#include "decode.h"
#include "lib.h"
#include "librcps.h"

#include <stdio.h>

#define UNSCHEDULED	-1

// XXX this sizing might make sense on 32-bit platforms, but not on 64 bit?
struct edge {
	int time;
	int amount;
	struct edge *next;
};

struct decoding_state {
	int *res_load;
	struct edge **res_edges;
};

/* XXX we already search the lists to find the place where to add the edge, it's
 * stupid to search the lists again. addedge should also get an edge from which
 * to search from */
static inline void addedge(struct rcps_resource *r, const int time, 
		const int duration, const int amount, struct decoding_state *state) {
	struct edge *cedge;
	struct edge *nedge;
	struct edge *last = NULL;
	cedge = state->res_edges[r->index];
	while ((cedge != NULL) && (cedge->time < time)) {
		last = cedge;
		cedge = cedge->next;
	}
	if ((cedge != NULL) && (cedge->time == time)) {
		cedge->amount += amount;
		assert(cedge->amount >= 0);
		last = cedge;
		cedge = cedge->next;
	}
	else {
		// XXX perhaps do not malloc, but take from list. might be faster and
		// cache-aligned
		nedge = (struct edge*)malloc(sizeof(struct edge));
		nedge->time = time; 
		nedge->next = cedge;
		last->next = nedge;
		nedge->amount = last->amount + amount; 
		assert(nedge->amount >= 0);
		last = nedge;
	}
	
	while ((cedge != NULL) && (cedge->time < (time + duration))) {
		cedge->amount += amount;
		last = cedge;
		cedge = cedge->next;
	}
	if (cedge == NULL) {
		// XXX perhaps do not malloc, but take from list. might be faster and
		// cache-aligned
		nedge = (struct edge*)malloc(sizeof(struct edge));
		nedge->time = time + duration; 
		nedge->amount = 0; 
		nedge->next = NULL;
		last->next = nedge;
	}

	// set the load to the maximum
	if (state->res_load[r->index] < amount) {
		state->res_load[r->index] = amount;
	}

	assert(state->res_edges[r->index]->time == 0);
}

/* find the first time >= start where the job can be scheduled without 
 * exceeding any resource limitations 
 * */
int postpone(struct rcps_solver *solver, struct rcps_problem *problem, 
		struct rcps_genome *genome, struct rcps_job *job, int start, 
		struct decoding_state *state, struct rcps_phenotype *pheno) {
	int i;
	int max_load;
	int ltime;
	int max_ltime;
	int duration = 0;
	struct rcps_request *crequest;
	struct rcps_resource *cresource;
	struct rcps_mode *cmode;
	struct edge *cedge;
	struct rcps_alternative *calternative;
	assert(start >= 0);

	max_ltime = start;
	cmode = job->modes[
		job->genome_position != -1 
		? genome->modes[job->genome_position]
		: 0
	];
	duration = cmode->duration;
	if (solver->duration_callback) {
		duration = solver->duration_callback(DURATION_FORWARD, start, 
			cmode->duration, cmode->cb_arg);
		//printf("postpone: %s direction=%d, start=%d = duration=%d\n", job->name, DURATION_FORWARD, start, duration);
	}

	for (i = 0; i < cmode->request_count; i++) {
		int pos = 0;
		if (cmode->requests[i]->genome_position != -1) {
			pos = genome->alternatives[cmode->requests[i]->genome_position];
		}
	}

	/* find the first possible start time */
	for (i = 0; i < cmode->request_count; i++) {
		ltime = start;
		calternative = cmode->requests[i]->alternatives[
			cmode->requests[i]->genome_position != -1
			? genome->alternatives[cmode->requests[i]->genome_position]
			: 0
		];
		crequest = cmode->requests[i];
		cresource = crequest->alternatives[
			crequest->genome_position != -1
			? genome->alternatives[crequest->genome_position]
			: 0
		]->resource;
		/* ignore requests which go to a non-renewable resource */
		if (calternative->resource->type == RCPS_RESOURCE_NONRENEWABLE) {
			break;
		}
		/* try at start time */
		/* get the maximum resource usage during the interval */
		max_load = 0;
		cedge = state->res_edges[calternative->resource->index];
		while ((cedge != NULL) && (cedge->time < (start + duration))) {
			if (cedge->time <= start) {
				max_load = cedge->amount;
			}
			else if (cedge->time < (start + duration)) {
				max_load = kpt_max(max_load, cedge->amount);
			}
			cedge = cedge->next;
		}
		if ((max_load > (calternative->resource->avail
					- calternative->amount)) 
					// for resource requests which are greater than the amount
					// available
					&& (max_load != 0)) {
			cedge = state->res_edges[calternative->resource->index];
			start = start + 1;
			while (cedge != NULL) {
				if ((cedge->time >= start) && (cedge->amount < max_load)) {
					start = cedge->time;
					break;
				}
				cedge = cedge->next;
			}
			// return postpone(problem, genome, job, start + 1);
			return postpone(solver, problem, genome, job, start, state, pheno);
		}
	}
	/* ok, do it at that time */
	pheno->job_start[job->index] = start;
	pheno->job_duration[job->index] = duration;
	/* add the edges */
	for (i = 0; i < cmode->request_count; i++) {
		crequest = cmode->requests[i];
		cresource = crequest->alternatives[
			crequest->genome_position != -1
			? genome->alternatives[crequest->genome_position]
			: 0
		]->resource;
		if (cresource->type == RCPS_RESOURCE_NONRENEWABLE) {
			state->res_load[cresource->index] += crequest->alternatives[
				crequest->genome_position != -1
				? genome->alternatives[crequest->genome_position]
				: 0
			]->amount;	
		}
		else if (cresource->type == RCPS_RESOURCE_RENEWABLE) {
			addedge(cresource, pheno->job_start[job->index], duration,
					crequest->alternatives[
						crequest->genome_position != -1
						? genome->alternatives[crequest->genome_position]
						: 0
					]->amount, state);
		}
		else {
			/* this should never happen */
			assert(1 == 0);
		}
	}
	return start;
}


struct rcps_phenotype *decode(struct rcps_solver *solver, 
		struct rcps_problem *problem, struct rcps_genome *genome) {
	int i, j;
	int s;
	int duration;
	int cmi;
	struct rcps_job *cjob;
	struct rcps_job *pjob;
	struct rcps_phenotype *pheno;
	struct decoding_state state;
	struct edge *cedge, *tedge;

	/* init the state */
	state.res_load = (int*)malloc(sizeof(int) * problem->resource_count);
	state.res_edges = (struct edge**)malloc(
		sizeof(struct edge*) * problem->resource_count);
	for (i = 0; i < problem->resource_count; i++) {
		state.res_load[i] = 0;
		// XXX perhaps do not malloc, but take from list. might be faster and
		// cache-aligned
		state.res_edges[i] = (struct edge*)malloc(sizeof(struct edge));
		state.res_edges[i]->time = 0;
		state.res_edges[i]->amount = 0;
		state.res_edges[i]->next = NULL;
	}

	pheno = (struct rcps_phenotype*)malloc(sizeof(struct rcps_phenotype));
	pheno->overuse_count = 0;
	pheno->overuse_amount = 0;
	pheno->job_start = (int*)malloc(sizeof(int) * problem->job_count);
	for (i = 0; i < problem->job_count; i++) {
		pheno->job_start[i] = UNSCHEDULED;
	}
	pheno->job_duration = (int*)malloc(sizeof(int) * problem->job_count);
	for (i = 0; i < problem->job_count; i++) {
		pheno->job_duration[i] = 0;
	}

	/* now take every job in turn and schedule it */
	for (i = 0; i < problem->job_count; i++) {
		cjob = problem->jobs[genome->schedule[i]];
		/* find the first possible start time through the predeccessors */
		s = cjob->earliest_start;
		for (j = 0; j < cjob->predeccessor_count; j++) {
			int rel_type = 0;
			int result = 0;
			pjob = cjob->predeccessors[j];
						rel_type = cjob->predeccessor_types[j];
			// get the start time of the pjob, use genome_position here
			result = pheno->job_start[pjob->index];
			assert(result != UNSCHEDULED);
			if (rel_type == SUCCESSOR_FINISH_START) {
				cmi = pjob->genome_position != -1
						? genome->modes[pjob->genome_position]
						: 0;
				duration = pjob->modes[cmi]->duration;
				if (solver->duration_callback) {
					duration = solver->duration_callback(
						DURATION_FORWARD,
						pheno->job_start[pjob->index],
						duration, pjob->modes[cmi]->cb_arg);
					//printf("decode: %s direction=%d, start=%d = duration=%d\n", pjob->name, DURATION_FORWARD, pheno->job_start[pjob->index], duration);
				}
				s = kpt_max(s, pheno->job_start[pjob->index] + duration);
			}
			else if (rel_type == SUCCESSOR_START_START) {
				s = kpt_max(s, pheno->job_start[pjob->index]);
			}
			else if (rel_type == SUCCESSOR_FINISH_FINISH) {
				int d2;
				cmi = pjob->genome_position != -1
						? genome->modes[pjob->genome_position]
						: 0;
				duration = pjob->modes[cmi]->duration;
				if (solver->duration_callback) {
					duration = solver->duration_callback(
						DURATION_FORWARD,
						pheno->job_start[pjob->index],
						duration, pjob->modes[cmi]->cb_arg);
				}
				cmi = cjob->genome_position != -1
						? genome->modes[cjob->genome_position]
						: 0;
				d2 = cjob->modes[cmi]->duration;
				if (solver->duration_callback) {
					//printf( "duration callback backward: %s\n", cjob->name );
					d2 = solver->duration_callback(
						DURATION_BACKWARD,
						pheno->job_start[pjob->index] + duration,
						d2, cjob->modes[cmi]->cb_arg);
				}
				s = kpt_max(s, pheno->job_start[pjob->index]
					+ duration
					- d2);
			}
		}
		/* now we need to postpone it until all requests can be satisfied */
		postpone(solver, problem, genome, cjob, s, &state, pheno);
	}
	// check for overloaded nonrenewable resources
	for (i = 0; i < problem->resource_count; i++) {
		if (state.res_load[i] > problem->resources[i]->avail) {
			pheno->overuse_count++;
			pheno->overuse_amount += state.res_load[i];
		}
	}
	// free the internal state
	free(state.res_load);
	for (i = 0; i < problem->resource_count; i++) {
		cedge = state.res_edges[i];
		while (cedge) {
			tedge = cedge;
			cedge = cedge->next;
			free(tedge);
		}
	}
	free(state.res_edges);
	
	return pheno;
}

