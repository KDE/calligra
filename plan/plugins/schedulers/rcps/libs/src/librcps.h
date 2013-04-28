#ifndef LIBRCPS_H
#define LIBRCPS_H

#ifdef __cplusplus
extern "C" {
#endif

/* XXX general introduction, refer people to manual */

struct rcps_problem;
struct rcps_resource;
struct rcps_job;
struct rcps_mode;
struct rcps_request;
struct rcps_alternative;
struct rcps_solver;

#define RCPS_UNDEF			-1

#define RCPS_RESOURCE_RENEWABLE		0
#define RCPS_RESOURCE_NONRENEWABLE	1

#define RCPS_CHECK_OK					0
#define RCPS_CHECK_START_JOB_MISSING	1
#define RCPS_CHECK_MULTIPLE_END_JOBS	2
#define RCPS_CHECK_END_JOB_MISSING		4
#define RCPS_CHECK_CIRCULAR_DEPENDENCY	8


/* defines for the rcps_solver_getparam and _setparam methods */
#define SOLVER_PARAM_POPSIZE 	0 /* population size, default is 600 */
#define SOLVER_PARAM_MUTSCHED	1 /* mutation probability for the scheduling 
    								 chromosome, in 1/10000. default is 500 */
#define SOLVER_PARAM_MUTMODE 	2 /* same, for the modes chromosome */
#define SOLVER_PARAM_MUTALT  	3 /* same, for the alternatives chromosome */
#define SOLVER_PARAM_JOBS		4 /* the number of parallel jobs to run */

/* different types of successors */
#define SUCCESSOR_FINISH_START	0
#define SUCCESSOR_START_START	1
#define SUCCESSOR_FINISH_FINISH	2

/* different types of fitness calculations */
#define FITNESS_DEFAULT		0 	/* uses the jobs end time to minimize problem duration */
#define FITNESS_WEIGHT		1	/* uses the weight assigned to the jobs to minimize problem duration */


/* return the library version as a string (static, don't free it) */
char *rcps_version();

/* return an error string corresponding to code */
char *rcps_error(int code);

/* create and return a new problem structure */
struct rcps_problem* rcps_problem_new();

/* clear and free a problem structure. this will also clear and free all
 * associated structures */
void rcps_problem_free(struct rcps_problem *p);

/* set fitness calculation mode */
void rcps_problem_setfitness_mode(struct rcps_problem *problem, int mode);

/* holds the result of the fitness calculation for a problem or job */
#define FITNESS_MAX_GROUP	9999	/* the maximum fitness group allowed */
struct rcps_fitness {
    int group;	/* the group this weight belongs to ( less is better) */
    int weight;	/* the weight (less is better) */
};
/* compare two fitness values */
int rcps_fitness_cmp(const struct rcps_fitness *a, const struct rcps_fitness *b);

/* register a callback that gets called for each job every time we want to calculate the project fitness.
 * arguments are the jobs start time, duration, an argument that can be set per job mode and an argument that
 * can be set for the problem.
 * should return the weight of this job mode.
 * the weight does not need to be constant, it may change dependent on eg start time, duration or cost.
 * the arg is set using rcps_job_set_weight_cbarg.
 */
void rcps_problem_set_weight_callback(struct rcps_problem *p,
    int (*weight_callback)(int time, int duration, struct rcps_fitness *nominal_weight, void *arg, void *fitness_arg));

/* register callbacks that gets called every time we want to calculate the project fitness. */
void rcps_problem_set_fitness_callback(struct rcps_problem *p,
                void* (*fitness_callback_init)(void *arg),
                void *init_arg,
                int (*fitness_callback_result)(struct rcps_fitness *fitness, void *arg));

/* create a new resource structure */
struct rcps_resource* rcps_resource_new();

/* clear and free a resource structure */
void rcps_resource_free(struct rcps_resource *r);

/* return the name of this resource, the name is not used internally */
char* rcps_resource_getname(struct rcps_resource *r);

/* set the name of this resource, the string is copied so you can use locals or
 * free it afterwards */
void rcps_resource_setname(struct rcps_resource *r, const char *name);

/* get the type of this resource, possible values are
 * RCPS_RESOURCE_RENEWABLE and RCPS_RESOURCE_NONRENEWABLE */
int rcps_resource_gettype(struct rcps_resource *r);

/* set the resource type, for possible values see ..._gettype() */
void rcps_resource_settype(struct rcps_resource *r, int type);

/* get the available amount of this resource */
int rcps_resource_getavail(struct rcps_resource *r);

/* set the available amount of this resource */
void rcps_resource_setavail(struct rcps_resource *r, int avail);

/* add a resource to a problem structure */
void rcps_resource_add(struct rcps_problem *p, struct rcps_resource *r);

/* return the number of resource structures in this problem */
int rcps_resource_count(struct rcps_problem *p);

/* get a resource from the problem */
struct rcps_resource* rcps_resource_get(struct rcps_problem *p, int r);

/* get a resource from the problem, by name */
struct rcps_resource* rcps_resource_getbyname(struct rcps_problem *p, 
	char *name);

/* remove (but don't free) a resource from the problem struct */
struct rcps_resource* rcps_resource_remove(struct rcps_problem *p, int r);

/* create a new job structure */
struct rcps_job* rcps_job_new();

/* clear and free a job structure, this also frees all associated structures */
void rcps_job_free(struct rcps_job *j);

/* return the name of this job, not used internally, don't free */
char* rcps_job_getname(struct rcps_job *j);

/* set the name, string is copied */
void rcps_job_setname(struct rcps_job *j, const char *name);

/* set earliest start time of this job */
void rcps_job_setearliest_start(struct rcps_job *j, int time);

/* get earliest start time of this job */
int rcps_job_getearliest_start(struct rcps_job *j);

/* set weight of this job to be used in fitness calculations */
void rcps_job_setweight(struct rcps_job *j, int weight);

/* add a job struct to a problem */
void rcps_job_add(struct rcps_problem *p, struct rcps_job *j);

/* return the number of jobs in this problem struct */
int rcps_job_count(struct rcps_problem *p);

/* get a job from the problem structure */
struct rcps_job* rcps_job_get(struct rcps_problem *p, int j);

/* get a job from the problem structure, by name */
struct rcps_job* rcps_job_getbyname(struct rcps_problem *p, char *name);

/* remove (but don't free) a job structure from the problem */
struct rcps_job* rcps_job_remove(struct rcps_problem *p, int j);

/* make the job "s" the successor of job "j", use one of the types defined above */
void rcps_job_successor_add(struct rcps_job *j, struct rcps_job *s, int type);

/* return the number of successors for this job */
int rcps_job_successor_count(struct rcps_job *j);

/* get successor from this job */
struct rcps_job* rcps_job_successor_get(struct rcps_job *j, int s);

/* get successor type from this job */
int rcps_job_successor_type_get(struct rcps_job *j, int s);

/* remove the sucessor from this job. please note that it might be references 
 * in other locations and should not be freed just like that */
struct rcps_job* rcps_job_successor_remove(struct rcps_job *j, int s);

/* after the problem has beed solved, get the start time of the job  */
int rcps_job_getstart_res(struct rcps_job *j);

/* after the problem has been solved, get the mode selected for execution */
int rcps_job_getmode_res(struct rcps_job *j);

/* create a new mode struct */
struct rcps_mode* rcps_mode_new();

/* free and clear a mode struct, will also clear associated requests */
void rcps_mode_free(struct rcps_mode *m);

/* get the duration of this mode */
int rcps_mode_getduration(struct rcps_mode *m);

/* set the duration of this mode */
void rcps_mode_setduration(struct rcps_mode *m, int d);

/* set the argument for the duration callback */
void rcps_mode_set_cbarg(struct rcps_mode *m, void *arg);

/* retrieve the argument for the duration callback */
void* rcps_mode_get_cbarg(struct rcps_mode *m);

/* set the argument for the weight callback */
void rcps_mode_set_weight_cbarg(struct rcps_mode *m, void *arg);

/* retrieve the argument for the weight callback */
void* rcps_mode_get_weight_cbarg(struct rcps_mode *m);

/* add a mode to a job */
void rcps_mode_add(struct rcps_job *j, struct rcps_mode *m);

/* count the modes on a job */
int rcps_mode_count(struct rcps_job *j);

/* get a mode from a job */
struct rcps_mode* rcps_mode_get(struct rcps_job *j, int m);

/* remove, but don't free, a mode from a job */
struct rcps_mode* rcps_mode_remove(struct rcps_job *j, int m);

/* create a new resource request structure */
struct rcps_request* rcps_request_new();

/* free a resource request structure */
void rcps_request_free(struct rcps_request *r);

/* add a resource request to a mode struct */
void rcps_request_add(struct rcps_mode *m, struct rcps_request *r);

/* count the resource requests on this mode */
int rcps_request_count(struct rcps_mode *m);

/* get a resource request from the mode */
struct rcps_request* rcps_request_get(struct rcps_mode *m, int r);

/* remove, but don't free a resource request froma mode */
struct rcps_request* rcps_request_remove(struct rcps_mode *m, int r);

/* after solving the problem, get the alternative selected */
int rcps_request_getalternative_res(struct rcps_request *r);

/* create a new resource alternative structure */
struct rcps_alternative* rcps_alternative_new();

/* clear and free a new resource alternative struct */
void rcps_alternative_free(struct rcps_alternative *a);

/* get the amount of the resource requested */
int rcps_alternative_getamount(struct rcps_alternative *a);

/* set the amount of the resource requested */
void rcps_alternative_setamount(struct rcps_alternative *a, int m);

/* get the resource that is requested by this alternative */
struct rcps_resource* rcps_alternative_getresource(struct rcps_alternative *a);

/* set (overwrites) the resource used by this alterantive */
void rcps_alternative_setresource(struct rcps_alternative *a, 
		struct rcps_resource *r);

/* add an alternative to a resource request */
void rcps_alternative_add(struct rcps_request *r, struct rcps_alternative *a);

/* count the alternatives of a request */
int rcps_alternative_count(struct rcps_request *r);

/* get an alterantive from a request */
struct rcps_alternative* rcps_alternative_get(struct rcps_request *r, int a);

/* remove, but don't free, an alterantive from a request */
struct rcps_alternative* rcps_alternative_remove(struct rcps_request *r, 
	int a);

/* check a problem for structural problems, returns one of the RCPS_CHECK_...
 * constants */
int rcps_check(struct rcps_problem *p);

/* create a new solver structure */
struct rcps_solver* rcps_solver_new();

/* free and clear a solver */
void rcps_solver_free(struct rcps_solver *s);

/* get and set solver parameters. for a list and possible values see the
 * SOLVER_PARAM_... defines. please don't change these parameters unless you
 * really know what you are doing */
int rcps_solver_getparam(struct rcps_solver *s, int param);
void rcps_solver_setparam(struct rcps_solver *s, int param, int value);

/* register a callback that gets called every 'steps' cycles. it can be used to 
 * report the progress to the user, and to abort the scheduling (return != 0). 
 * 'arg' is passed through to the callback
 */
void rcps_solver_set_progress_callback(struct rcps_solver *s, 
	int steps, void *arg,
	int (*progress_callback)(int generations, struct rcps_fitness fitness, void *arg));

/* register a callback that gets called every time we want to determine the 
 * duration of a mode at a given time. arguments are the 'nominal'
 * duration of the mode, and an argument that can be set per mode. should
 * return the actual duration of this mode. direction determines whether
 * you calculate the duration based on the start time (FORWARD) or on the
 * end time (BACKWARD)
 */
#define DURATION_FORWARD	0
#define DURATION_BACKWARD	1
void rcps_solver_set_duration_callback(struct rcps_solver *s, 
	int (*duration_callback)(int direction, int time, int nominal_duration, 
		void *arg));

/* solve a problem */
void rcps_solver_solve(struct rcps_solver *s, struct rcps_problem *p);

/* after solving a problem, get the number of reproduction cycles that were
 * needed. this is only useful if you want to compare the performance of
 * different settings, and not really meant for normal use */
int rcps_solver_getreps(struct rcps_solver *s);

/* not all schedules can be computed to a valid result, so after solving you can
 * use this function to query the solver for warnings. if this returns non-zero,
 * the solution is not valid!
 * */
int rcps_solver_getwarnings(struct rcps_solver *s);

#ifdef __cplusplus
}
#endif

#endif /* LIBRCPS_H */
