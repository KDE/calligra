#ifdef _WIN32
# include <malloc.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
// XXX remove these two, needed for debugging only
#include <stdio.h>
#include <errno.h>

#include "librcps.h"
#include "../config.h"
#include "structs.h"
#include "decode.h"
#include "fitness.h"
#include "initial.h"
#include "repair.h"
#include "lib.h"
#include "ops.h"

#define DEFAULT_POP_SIZE	600
#define DEFAULT_MUT_MODE	500
#define DEFAULT_MUT_SCHED   500
#define DEFAULT_MUT_ALT	 500

char *rcps_version() {
	return VERSION;
}

struct rcps_problem* rcps_problem_new() {
	struct rcps_problem *ret;
	ret = (struct rcps_problem*)malloc(sizeof(struct rcps_problem));
	if (ret) {
		memset(ret, 0, sizeof(struct rcps_problem));
	}
	return ret;
}

void rcps_problem_free(struct rcps_problem *p) {
	int i;
	for (i = 0; i < p->resource_count; i++) {
		rcps_resource_free(p->resources[i]);
	}
	free(p->resources);
	for (i = 0; i < p->job_count; i++) {
		rcps_job_free(p->jobs[i]);
	}
	free(p->jobs);
	free(p);
}

void rcps_problem_setfitness_mode(struct rcps_problem *p, int mode) {
	p->fitness_mode = mode;
}

void rcps_problem_set_weight_callback(struct rcps_problem *p,
			int (*weight_callback)(int time, int duration, struct rcps_fitness *nominal_weight, void *arg, void *fitness_arg)) {
	p->weight_callback = weight_callback;
}

void rcps_problem_set_fitness_callback(struct rcps_problem *p,
			void* (*fitness_callback_init)(void *arg),
			void *init_arg,
			int (*fitness_callback_result)(struct rcps_fitness *fitness, void *arg)) {
	p->fitness_callback_init = fitness_callback_init;
	p->fitness_init_arg = init_arg;
	p->fitness_callback_result = fitness_callback_result;
}

struct rcps_resource* rcps_resource_new() {
	struct rcps_resource *ret;
	ret = (struct rcps_resource*)malloc(sizeof(struct rcps_resource));
	if (ret) {
		memset(ret, 0, sizeof(struct rcps_resource));
	}
	return ret;
}

void rcps_resource_free(struct rcps_resource *r) {
	free(r->name);
	free(r);
}

char* rcps_resource_getname(struct rcps_resource *r) {
	return r->name;
}

void rcps_resource_setname(struct rcps_resource *r, const char *name) {
	r->name = (char*)malloc((strlen(name)+1)*sizeof(char));
	strcpy(r->name, name);
}

int rcps_resource_gettype(struct rcps_resource *r) {
	return r->type;
}

void rcps_resource_settype(struct rcps_resource *r, int type) {
	// XXX assert valid values in these functions	
	r->type = type;
}

int rcps_resource_getavail(struct rcps_resource *r) {
	return r->avail;
}

void rcps_resource_setavail(struct rcps_resource *r, int avail) {
	r->avail = avail;
}

void rcps_resource_add(struct rcps_problem *p, struct rcps_resource *r) {
	p->resource_count++;
	p->resources = (struct rcps_resource**)
		realloc(p->resources, p->resource_count * 
			sizeof(struct rcps_resource*));
	p->resources[p->resource_count-1] = r;
}

int rcps_resource_count(struct rcps_problem *p) {
	return p->resource_count;
}

struct rcps_resource* rcps_resource_get(struct rcps_problem *p, int r) {
	return p->resources[r];
}

struct rcps_resource* rcps_resource_getbyname(struct rcps_problem *p, 
	char *name) {
	int i;
	for (i = 0; i < p->resource_count; i++) {
		struct rcps_resource *cr = p->resources[i];
		if (strcmp(cr->name, name) == 0) {
			return cr;
		}
	}
	return NULL;
}

struct rcps_resource* rcps_resource_remove(struct rcps_problem *p, int r) {
	struct rcps_resource *ret = p->resources[r];
	memmove(p->resources[r], p->resources[r+1], 
		(p->resource_count-r-1)*sizeof(struct rcps_problem*));
	p->resource_count--;
	return ret;
}

struct rcps_job* rcps_job_new() {
	struct rcps_job *ret = (struct rcps_job*)malloc(
		sizeof(struct rcps_job));
	if (ret) {
		memset(ret, 0, sizeof(struct rcps_job));
		ret->res_start = RCPS_UNDEF;
		ret->res_mode = RCPS_UNDEF;
	}
	return ret;
}

void rcps_job_free(struct rcps_job *j) {
	free(j->name);
	free(j->successors);
	free(j->successor_types);
	free(j->predeccessors);
	free(j->modes);
	free(j);
}

char* rcps_job_getname(struct rcps_job *j) {
	return j->name;
}

void rcps_job_setname(struct rcps_job *j, const char *name) {
	j->name = (char*)malloc((strlen(name)+1)*sizeof(char));
	strcpy(j->name, name);
}

void rcps_job_setearliest_start(struct rcps_job *j, int time) {
	j->earliest_start = time;
}

int rcps_job_getearliest_start(struct rcps_job *j) {
	return j->earliest_start;
}


void rcps_job_setweight(struct rcps_job *j, int weight) {
	j->weight = weight;
}

/* add a job struct to a problem */
void rcps_job_add(struct rcps_problem *p, struct rcps_job *j);

void rcps_job_add(struct rcps_problem *p, struct rcps_job *j) {
	p->job_count++;
	p->jobs = (struct rcps_job**)
		realloc(p->jobs, p->job_count*sizeof(struct rcps_job*));
	p->jobs[p->job_count-1] = j;
}

int rcps_job_count(struct rcps_problem *p) {
	return p->job_count;
}

struct rcps_job* rcps_job_get(struct rcps_problem *p, int j) {
	return p->jobs[j];
}

struct rcps_job* rcps_job_getbyname(struct rcps_problem *p, char *name) {
	int i;
	for (i = 0; i < p->job_count; i++) {
		struct rcps_job *cj = p->jobs[i];
		if (strcmp(cj->name, name) == 0) {
			return cj;
		}
	}
	return NULL;
}

struct rcps_job* rcps_job_remove(struct rcps_problem *p, int j) {
	struct rcps_job *ret = p->jobs[j];
	memmove(p->jobs[j], p->jobs[j+1], 
		(p->job_count-j-1)*sizeof(struct rcps_problem*));
	p->job_count--;
	return ret;
}

void rcps_job_successor_add(struct rcps_job *j, struct rcps_job *s, int type) {
	assert(type >= SUCCESSOR_FINISH_START);
	assert(type <= SUCCESSOR_FINISH_FINISH);
	j->successor_count++;
	j->successors = (struct rcps_job**)
		realloc(j->successors, j->successor_count *
			sizeof(struct rcps_job*));
	j->successor_types = (int*)
		realloc(j->successor_types, j->successor_count *
			sizeof(int));
	j->successors[j->successor_count-1] = s;
	j->successor_types[j->successor_count-1] = type;
}

void rcps_job_predeccessor_add(struct rcps_job *j, struct rcps_job *p, int type) {
	j->predeccessor_count++;
	j->predeccessors = (struct rcps_job**)
		realloc(j->predeccessors, j->predeccessor_count *
			sizeof(struct rcps_job*));
	j->predeccessor_types = (int*)
		realloc(j->predeccessor_types, j->predeccessor_count *
			sizeof(int));
	j->predeccessors[j->predeccessor_count-1] = p;
	j->predeccessor_types[j->predeccessor_count-1] = type;
}

int rcps_job_successor_count(struct rcps_job *j) {
	return j->successor_count;
}

struct rcps_job* rcps_job_successor_get(struct rcps_job *j, int s) {
	return j->successors[s];
}

int rcps_job_successor_type_get(struct rcps_job *j, int s) {
	return j->successor_types[s];
}

struct rcps_job* rcps_job_successor_remove(struct rcps_job *j, int s) {
	struct rcps_job *ret = j->successors[s];
	memmove(&j->successors[s], &j->successors[s+1], 
		(j->successor_count-s-1)*sizeof(struct rcps_job*));
	memmove(&j->successor_types[s], &j->successor_types[s+1], 
		(j->successor_count-s-1)*sizeof(struct rcps_job*));
	j->successor_count--;
	return ret;
}

int rcps_job_getstart_res(struct rcps_job *j) {
	return j->res_start;
}

int rcps_job_getmode_res(struct rcps_job *j) {
	return j->res_mode;
}

struct rcps_mode* rcps_mode_new() {
	struct rcps_mode *ret = (struct rcps_mode*)malloc(
		sizeof(struct rcps_mode));
	if (ret) {
		memset(ret, 0, sizeof(struct rcps_mode));
	}
	return ret;
}

void rcps_mode_free(struct rcps_mode *m) {
	int i;
	for (i = 0; i < m->request_count; i++) {
		rcps_request_free(m->requests[i]);
	}
	free(m->requests);
	free(m);
}

int rcps_mode_getduration(struct rcps_mode *m) {
	return m->duration;
}

void rcps_mode_setduration(struct rcps_mode *m, int d) {
	m->duration = d;
}

void rcps_mode_set_cbarg(struct rcps_mode *m, void *arg) {
	m->cb_arg = arg;
}

void* rcps_mode_get_cbarg(struct rcps_mode *m) {
	return m->cb_arg;
}

void rcps_mode_set_weight_cbarg(struct rcps_mode *m, void *arg) {
	m->weight_cb_arg = arg;
}

void *rcps_mode_get_weight_cbarg(struct rcps_mode *m) {
	return m->weight_cb_arg;
}

void rcps_mode_add(struct rcps_job *j, struct rcps_mode *m) {
	j->mode_count++;
	j->modes = (struct rcps_mode**)
		realloc(j->modes, j->mode_count *
			sizeof(struct rcps_mode*));
	j->modes[j->mode_count-1] = m;
}

int rcps_mode_count(struct rcps_job *j) {
	return j->mode_count;
}

struct rcps_mode* rcps_mode_get(struct rcps_job *j, int m) {
	return j->modes[m];
}

struct rcps_mode* rcps_mode_remove(struct rcps_job *j, int m) {
	struct rcps_mode *ret = j->modes[m];
	memmove(j->modes[m], j->modes[m+1], 
		(j->mode_count-m-1)*sizeof(struct rcps_mode*));
	j->mode_count--;
	return ret;
}

struct rcps_request* rcps_request_new() {
	struct rcps_request *ret = (struct rcps_request*)malloc(
		sizeof(struct rcps_request));
	if (ret) {
		memset(ret, 0, sizeof(struct rcps_request));
		ret->res_alternative = RCPS_UNDEF;
	}
	return ret;
}

void rcps_request_free(struct rcps_request *r) {
	free(r);
}

void rcps_request_add(struct rcps_mode *m, struct rcps_request *r) {
	m->request_count++;
	m->requests = (struct rcps_request**)
		realloc(m->requests, m->request_count *
			sizeof(struct rcps_request*));
	m->requests[m->request_count-1] = r;
}

int rcps_request_count(struct rcps_mode *m) {
	return m->request_count;
}

struct rcps_request* rcps_request_get(struct rcps_mode *m, int r) {
	return m->requests[r];
}

struct rcps_request* rcps_request_remove(struct rcps_mode *m, int r) {
	struct rcps_request *ret = m->requests[r];
	memmove(m->requests[r], m->requests[r+1], 
		(m->request_count-r-1)*sizeof(struct rcps_request*));
	m->request_count--;
	return ret;
}

int rcps_request_getalternative_res(struct rcps_request *r) {
	if (r->res_alternative < 0)
		return 0;
	return r->res_alternative;
}

struct rcps_alternative* rcps_alternative_new() {
	struct rcps_alternative *ret = (struct rcps_alternative*)malloc(
		sizeof(struct rcps_alternative));
	if (ret) {
		memset(ret, 0, sizeof(struct rcps_alternative));
	}
	return ret;
}

void rcps_alternative_free(struct rcps_alternative *a) {
	free(a);
}

int rcps_alternative_getamount(struct rcps_alternative *a) {
	return a->amount;
}

void rcps_alternative_setamount(struct rcps_alternative *a, int m) {
	a->amount = m;
}

struct rcps_resource* rcps_alternative_getresource(
	struct rcps_alternative *a) {
	return a->resource;
}

void rcps_alternative_setresource(struct rcps_alternative *a, 
		struct rcps_resource *r) {
	a->resource = r;
}

void rcps_alternative_add(struct rcps_request *r, struct rcps_alternative *a) {
	r->alternative_count++;
	r->alternatives = (struct rcps_alternative**)
		realloc(r->alternatives, r->alternative_count *
			sizeof(struct rcps_alternative*));
	r->alternatives[r->alternative_count-1] = a;
}

int rcps_alternative_count(struct rcps_request *r) {
	return r->alternative_count;
}

struct rcps_alternative* rcps_alternative_get(struct rcps_request *r, int a) {
	return r->alternatives[a];
}

struct rcps_alternative* rcps_alternative_remove(struct rcps_request *r, 
	int a) {
	struct rcps_alternative *ret = r->alternatives[a];
	memmove(r->alternatives[a], r->alternatives[a+1], 
		(r->alternative_count-a-1)*sizeof(struct rcps_alternative*));
	r->alternative_count--;
	return ret;
}

int rcps_solver_getparam(struct rcps_solver *s, int param) {
	switch (param) {
		case SOLVER_PARAM_POPSIZE:
			return s->pop_size;
			break;
		case SOLVER_PARAM_MUTSCHED:
			return s->mut_sched;
			break;
		case SOLVER_PARAM_MUTMODE:
			return s->mut_mode;
			break;
		case SOLVER_PARAM_MUTALT:
			return s->mut_alt;
			break;
		case SOLVER_PARAM_JOBS:
			return s->jobs;
			break;
		default:
			return -1;
	}
}

void rcps_solver_setparam(struct rcps_solver *s, int param, int value) {
	switch (param) {
		case SOLVER_PARAM_POPSIZE:
			s->pop_size = value;
			break;
		case SOLVER_PARAM_MUTSCHED:
			s->mut_sched = value;
			break;
		case SOLVER_PARAM_MUTMODE:
			s->mut_mode = value;
			break;
		case SOLVER_PARAM_MUTALT:
			s->mut_alt = value;
			break;
		case SOLVER_PARAM_JOBS:
#ifdef HAVE_PTHREAD	
			s->jobs = value;
#else
			// XXX report that this is not supported
#endif
			break;
			// XXX return error by default
	}
}

struct rcps_solver* rcps_solver_new() {
	struct rcps_solver *ret = (struct rcps_solver*)malloc(
		sizeof(struct rcps_solver));
	if (ret) {
		memset(ret, 0, sizeof(struct rcps_solver));
	}

	ret->pop_size = DEFAULT_POP_SIZE;
	ret->mut_sched = DEFAULT_MUT_SCHED;
	ret->mut_mode = DEFAULT_MUT_MODE;
	ret->mut_alt = DEFAULT_MUT_ALT;
	ret->jobs = 1; // XXX autodetect on some platforms?
	ret->reproductions = RCPS_UNDEF;
#ifdef HAVE_PTHREAD	
	// XXX look at error code, perhaps use an if to not use the lock if there is
	// only one thread
	//pthread_init();
	assert(pthread_mutex_init(&ret->lock, NULL) == 0);
#endif
	return ret;
}

void rcps_solver_free(struct rcps_solver *s) {
#ifdef HAVE_PTHREAD	
	// XXX look at error code
	assert(pthread_mutex_destroy(&s->lock) == 0);
#endif
	free(s);
}

static char *check_ok = "Ok";
static char *check_start_job_missing = "Start job missing";
static char *check_multiple_end_jobs = "Multiple end jobs";
static char *check_end_job_missing = "End job missing";
static char *check_circular_dependency = "Circular dependency";
static char *check_unknown_code = "Unknown error code";

char *rcps_error(int code) {
	char *r = check_unknown_code;
	if (code == RCPS_CHECK_OK) {
		r = check_ok;
	} else if (code & RCPS_CHECK_START_JOB_MISSING) {
		r = check_start_job_missing;
	} else if (code & RCPS_CHECK_MULTIPLE_END_JOBS ) {
		r = check_multiple_end_jobs;
	} else if (code & RCPS_CHECK_END_JOB_MISSING ) {
		r = check_end_job_missing;
	} else if (code & RCPS_CHECK_CIRCULAR_DEPENDENCY ) {
		r = check_circular_dependency;
	}
	return r;
}

int job_compare(const void *a, const void *b) {
	return (char *)a - (char *)b;
}

int check_circulardependencies(struct rcps_job *job, struct slist *visited) {
	int result = RCPS_CHECK_OK;
	int i;
	struct slist_node *n;
	//printf("check_circulardependencies: %s visited: %i\n", job->name, slist_count(visited));
	if ( slist_find(visited, job)) {
		result = RCPS_CHECK_CIRCULAR_DEPENDENCY;
		//printf("check_circulardependencies: %s already visited\n", job->name);
	} else {
		n = slist_node_new(job);
		slist_add(visited, n);
		for (i = 0; i < job->successor_count; ++i) {
			result = check_circulardependencies( job->successors[ i ], visited );
			if ( result != RCPS_CHECK_OK ) {
				break;
			}
		}
		// remove this job from the visited to avoid false positive
		slist_unlink(visited, n);
		slist_node_free(n, NULL);
	}
	return result;
}
/* all jobs must have successors except the end job, and all jobs must have predeccessors except the start job */
int check_dependencies(struct rcps_problem *p) {
	int result = RCPS_CHECK_OK;
	int end_count = 0;
	int i, k;
	struct rcps_job *start_job;
	struct slist *visited;
	struct slist *has_predecessor = slist_new(job_compare);

	for (i = 0; i < p->job_count; ++i) {
		struct rcps_job *j = p->jobs[ i ];
		//printf("check_dependencies: job %s successors: %i\n", j->name, j->successor_count);
		if (j->successor_count == 0) {
			++end_count;
		} else {
			for (k = 0; k < j->successor_count; ++k) {
				//printf("check_dependencies: job %s successor[%i] = %s\n", j->name, k, j->successors[k]->name);
				slist_add(has_predecessor, slist_node_new(j->successors[k]));
			}
		}
	}
	if (end_count > 1) {
		result += RCPS_CHECK_MULTIPLE_END_JOBS;
	} else if (end_count == 0) {
		result += RCPS_CHECK_END_JOB_MISSING;
	}
	if (result == RCPS_CHECK_OK) {
		start_job = 0;
		for (i = 0; i < p->job_count; ++i) {
			if (!slist_find(has_predecessor, p->jobs[i])) {
				start_job = p->jobs[i];
			}
		}
		if (start_job) {
			/* All other jobs should be successors of the start job */
			//printf("check_dependencies: check circular\n");
			visited = slist_new(job_compare);
			result += check_circulardependencies(start_job, visited);
			slist_free(visited, NULL);
		} else {
			result += RCPS_CHECK_START_JOB_MISSING;
		}

	}
	slist_free(has_predecessor, NULL);
	//printf("check_dependencies: result=%i\n", result);
	return result;
}

int rcps_check(struct rcps_problem *p) {
	/* XXX check for structural problems */
	int result = RCPS_CHECK_OK;
	result = check_dependencies( p );
	printf("rcps_check: result='%s'\n", rcps_error(result));
	return result;
}

int rcps_fitness_cmp(const struct rcps_fitness *a, const struct rcps_fitness *b) {
	return a->group == b->group ? a->weight - b->weight : a->group - b->group;
}

int individual_cmp(const void *a, const void *b) {
	return rcps_fitness_cmp(&(((struct rcps_individual*)a)->fitness), &(((struct rcps_individual*)b)->fitness));
}

void add_individual(struct rcps_individual *ind, struct rcps_population *pop) {
	struct slist_node *n;
	struct rcps_individual *i;

	n = slist_node_new(ind);
	slist_add(pop->individuals, n);

	while (slist_count(pop->individuals) > pop->size) {
		n = slist_last(pop->individuals);
		if (!n) {
			// XXX what the fuck is that?
			fprintf(stderr, "uhu, no one there?\n");
		}
		slist_unlink(pop->individuals, n);
		i = (struct rcps_individual*)slist_node_getdata(n);
		slist_node_free(n, NULL);
		free(i->genome.schedule);
		free(i->genome.modes);
		free(i->genome.alternatives);
		free(i);
	}
}

struct rcps_population *new_population(struct rcps_solver *s, 
		struct rcps_problem *problem) {
	struct rcps_population *pop;
	struct rcps_individual *ind;
	int i;
	int lcount = 0;
	struct rcps_fitness best_fitness;
	best_fitness.group = FITNESS_MAX_GROUP;
	best_fitness.weight = 0;

	pop = (struct rcps_population*)malloc(sizeof(struct rcps_population));
	pop->individuals = slist_new(individual_cmp);
	pop->size = s->pop_size;
	for (i = 0; i < s->pop_size; i++) {
		ind = (struct rcps_individual*)malloc(sizeof(struct rcps_individual));
		initial(problem, &ind->genome);
		if (repair(problem, &ind->genome)) {
			struct rcps_phenotype *pheno = decode(s, problem, 
				&ind->genome);
			ind->fitness = fitness(problem, &ind->genome, pheno);
			if (rcps_fitness_cmp(&(ind->fitness), &best_fitness) < 0) {
				best_fitness = ind->fitness;
			}
			add_individual(ind, pop);
		}
		else {
			// XXX somehow track this and abort with an error if we never add a
			// valid individual
			free(ind->genome.schedule);
			free(ind->genome.modes);
			free(ind->genome.alternatives);
			free(ind);
			i--;
		}
		if (s->progress_callback) {
			if (i >= (lcount + s->cb_steps)) {
				if (s->progress_callback(0, best_fitness, s->cb_arg)) {
					return pop;
				}
				lcount = i;
			}
		}
	}
	return pop;
}


int run_alg(struct rcps_solver *s, struct rcps_problem *p) { 
	/* run the algorithm */
	int end = 0;
	int count = 0;
	int tcount = 0;
	int lcount = 0;
	struct rcps_fitness last_fitness;
	int last_overuse = 1;
	int breakoff_count = 0;
	int desperate = 0;

	last_fitness.group = FITNESS_MAX_GROUP;
	last_fitness.weight = 0;

	fflush(stderr);

	// make this configurable
	breakoff_count = 100000 / s->jobs;

#ifdef HAVE_PTHREAD	
	// XXX look at error code
	pthread_mutex_lock(&s->lock);
#endif
	do {
		// breed
		int i,j;
		int son_overuse, daughter_overuse, best_overuse;
		struct rcps_individual *father;
		struct rcps_individual *mother;
		struct rcps_individual *son;
		struct rcps_individual *daughter;
		struct rcps_phenotype *pheno;
		struct rcps_fitness f1, f2;
		f1.group = FITNESS_MAX_GROUP;
		f1.weight = 0;
		f2 = f1;

		son = (struct rcps_individual*)malloc(sizeof(struct rcps_individual));
		son->genome.schedule = (int*)malloc(sizeof(int) * p->job_count);
		son->genome.modes = (int*)malloc(p->genome_modes * sizeof(int));
		son->genome.alternatives = (int*)malloc(p->genome_alternatives * sizeof(int));
		daughter = (struct rcps_individual*)malloc(sizeof(struct rcps_individual));
		daughter->genome.schedule = (int*)malloc(sizeof(int) * p->job_count);
		daughter->genome.modes = (int*)malloc(p->genome_modes * sizeof(int));
		daughter->genome.alternatives = (int*)malloc(p->genome_alternatives * sizeof(int));
		// select father and mother
		// XXX we want a configurable bias towards better individuals here
		i = irand(s->pop_size);
		j = 1 + irand(s->pop_size - 1);
		j = (i + j) % s->pop_size;
		father = (struct rcps_individual*)slist_node_getdata(
			slist_at(s->population->individuals, i));
		mother = (struct rcps_individual*)slist_node_getdata(
			slist_at(s->population->individuals, j));
		// crossover
		sched_crossover2(s, p, 
			father->genome.schedule, mother->genome.schedule, 
			son->genome.schedule, daughter->genome.schedule);
		crossover2(father->genome.modes, mother->genome.modes, 
			son->genome.modes, daughter->genome.modes, p->genome_modes);
		crossover2(father->genome.alternatives, mother->genome.alternatives, 
			son->genome.alternatives, daughter->genome.alternatives, 
			p->genome_alternatives);
#ifdef HAVE_PTHREAD	
	// XXX look at error code
		pthread_mutex_unlock(&s->lock);
#endif

		// mutate
		sched_mutation(s, p, son->genome.schedule, s->mut_sched);
		sched_mutation(s, p, daughter->genome.schedule, s->mut_sched);
		mutation(son->genome.modes, p->modes_max, 
			p->genome_modes, s->mut_mode);
		mutation(daughter->genome.modes, p->modes_max, 
			p->genome_modes, s->mut_mode);
		mutation(son->genome.alternatives, p->alternatives_max, 
			p->genome_alternatives, s->mut_alt);
		mutation(daughter->genome.alternatives, p->alternatives_max, 
			p->genome_alternatives, s->mut_mode);

		// add to population
		pheno = decode(s, p, &son->genome);
		son->fitness = fitness(p, &son->genome, pheno);
		son_overuse = pheno->overuse_count;
		pheno = decode(s, p, &daughter->genome);
		daughter->fitness = fitness(p, &daughter->genome, pheno);
		daughter_overuse = pheno->overuse_count;

#ifdef HAVE_PTHREAD	
	// XXX look at error code
		pthread_mutex_lock(&s->lock);
#endif
		add_individual(son, s->population);
		add_individual(daughter, s->population);
		// check if we have a better individual, if yes reset count
		f1 = ((struct rcps_individual*)slist_node_getdata(slist_first(
			s->population->individuals)))->fitness;
		f2 = ((struct rcps_individual*)slist_node_getdata(slist_last(
			s->population->individuals)))->fitness;
		// get the best overuse count
		best_overuse = son_overuse < daughter_overuse ?
			son_overuse : daughter_overuse;
		// check if we want to stop
		if (rcps_fitness_cmp(&f1, &last_fitness) < 0) {
			last_fitness = f1;
			last_overuse = best_overuse;
			count = 0;
		}
		count++;
		tcount++;
		if (count >= breakoff_count) {
			if ((last_overuse > 0) && (!desperate)) {
				// we are going into desperate mode
				desperate = 1;
				breakoff_count *= 10;
				// XXX threading problem: do not do this because is affects
				// other threads too! intead us desperate accordingly above
/*				s->population->size *= 5;
				s->mut_sched += 1000;
				s->mut_mode += 1000;
				s->mut_alt += 1000;*/
			}
			else {
				end = 1;
			}
		}
		// XXX if we use multiple threads, communicate to the others as well!
		if (s->progress_callback) {
			if (tcount >= (lcount + s->cb_steps)) {
				end |= s->progress_callback(tcount, last_fitness, s->cb_arg);
				lcount = tcount;
			}
		}
	} while (!end);
#ifdef HAVE_PTHREAD	
	// XXX look at error code
	pthread_mutex_unlock(&s->lock);
#endif
	return tcount;
}

#ifdef HAVE_PTHREAD	
struct thread_arg {
	struct rcps_solver *s;
	struct rcps_problem *p;
};

void *threadfunc(void *a) {
	struct thread_arg *args = (struct thread_arg*)a;
	int tcount = run_alg(args->s, args->p);
	// XXX can be moved to run_alg
	assert(pthread_mutex_lock(&args->s->lock) == 0);
	args->s->reproductions += tcount;
	assert(pthread_mutex_unlock(&args->s->lock) == 0);
	return NULL;
}
#endif

void rcps_solver_set_progress_callback(struct rcps_solver *s, 
	int steps, void *arg,
	int (*progress_callback)(int generations, struct rcps_fitness fitness, void *arg)) {
	s->progress_callback = progress_callback;
	s->cb_arg = arg;
	s->cb_steps = steps;
}

void rcps_solver_set_duration_callback(struct rcps_solver *s,
		int (*duration_callback)(int direction, int starttime, 
		int nominal_duration, void *arg)) {
	s->duration_callback = duration_callback;
}

void rcps_solver_solve(struct rcps_solver *s, struct rcps_problem *p) {
	int i, j, k;
	struct rcps_genome *genome;
	struct rcps_phenotype *pheno;
	/* init the random number generator */
	srand(time(NULL));
	/* fix the indices */
	for (i = 0; i < p->job_count; i++) {
		p->jobs[i]->index = i;
	}
	for (i = 0; i < p->resource_count; i++) {
		p->resources[i]->index = i;
	}
	/* fix the predeccessors */
	for (i = 0; i < p->job_count; i++) {
		free(p->jobs[i]->predeccessors);
		p->jobs[i]->predeccessor_count = 0;
	}
	for (i = 0; i < p->job_count; i++) {
		for (j = 0; j < p->jobs[i]->successor_count; j++) {
			rcps_job_predeccessor_add(p->jobs[i]->successors[j], 
				p->jobs[i], p->jobs[i]->successor_types[j]);
		}
	}
	/* do other precalculations */
	/* modes and alternatives */
	p->genome_modes = 0;
	p->genome_alternatives = 0;
	for (i = 0; i < p->job_count; i++) {
		struct rcps_job *job = p->jobs[i];
		// do the modes
		if (job->mode_count > 1) {
			p->genome_modes++;
			p->modes_max = (int*) realloc(p->modes_max, p->genome_modes*sizeof(int));
			p->modes_max[p->genome_modes-1] = job->mode_count;
			job->genome_position = p->genome_modes-1;
		}
		else {
			job->genome_position = -1;
		}
		job->res_start = -1;
		job->res_mode = -1;
		for (j = 0; j < job->mode_count; j++) {
			struct rcps_mode *mode = job->modes[j];
			for (k = 0; k < mode->request_count; k++) {
				struct rcps_request *request = mode->requests[k];
				request->res_alternative = -1;
				// do the alternatives
				if (request->alternative_count > 1) {
					p->genome_alternatives++;
					p->alternatives_max = (int*) realloc(p->alternatives_max,
						p->genome_alternatives*sizeof(int));
					p->alternatives_max[p->genome_alternatives-1] = 
						request->alternative_count;
					request->genome_position = p->genome_alternatives-1;
				}
				else {
					request->genome_position = -1;
				}
			}
		}
	}
	/* hash of predecessor relations */
	free(s->predecessor_hash);
	s->predecessor_hash = (char*)malloc(sizeof(char)*p->job_count*p->job_count);
	for (i = 0; i < p->job_count*p->job_count; i++) {
		s->predecessor_hash[i] = -1;
	}

	/* initialize the population */
	s->population = new_population(s, p);

	/* here we run the algorithm */
#ifdef HAVE_PTHREAD	
	if (s->jobs <= 1) {
		s->reproductions = run_alg(s, p);
	}
	else {
		s->reproductions = 0;
		pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * s->jobs);
		struct thread_arg args;
		args.s = s;
		args.p = p;
		int i;
		for (i = 0; i < s->jobs; i++) {
			int result = pthread_create(&threads[i], NULL, threadfunc, &args);
			assert(result == 0);
		}

		// XXX join them all!
		for (i = 0; i < s->jobs; i++) {
			int result = pthread_join(threads[i], NULL);
			assert(result == 0);
		}
	}
#else
	s->reproductions = run_alg(s, p);
#endif

// 	struct rcps_fitness fit = ((struct rcps_individual*)slist_node_getdata(slist_first(
// 		s->population->individuals)))->fitness;
// 	printf("cycles: \t%i\nfitness:\t[%d, %d]\n", tcount, fit.group, fit.weight);

	// transfer the results to the problem structure
	genome = &((struct rcps_individual*)slist_node_getdata(
	slist_first(s->population->individuals)))->genome;
	// XXX we could save us this decoding step if we would keep the best ones
	// from before, not really worth it
	
	// save a warning if the schedule is not valid
	pheno = decode(s, p, genome);
	if (pheno->overuse_count) {
		s->warnings = 1;
	}
	else {
		s->warnings = 0;
	}

	for (i = 0; i < p->job_count; i++) {
		struct rcps_job *job;
		struct rcps_mode *mode;
		job = p->jobs[i];
		job->res_start = pheno->job_start[job->index];
		job->res_mode = 
			job->genome_position == -1 
				? 0 
				: genome->modes[job->genome_position];

		mode = job->modes[job->res_mode];
		for (j = 0; j < mode->request_count; j++) {
			struct rcps_request *request;
			request = mode->requests[j];
			request->res_alternative =
				request->genome_position == -1 
					? 0 
					: genome->alternatives[request->genome_position];
		}
	}
}

int rcps_solver_getreps(struct rcps_solver *s) {
	return s->reproductions;
}

int rcps_solver_getwarnings(struct rcps_solver *s) {
	return s->warnings;
}
