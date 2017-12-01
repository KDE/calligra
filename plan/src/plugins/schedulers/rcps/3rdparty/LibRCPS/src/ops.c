#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ops.h"
#include "lib.h"

/* XXX in both these crossover ops, the son and the daughter parts could be
 * folded together by using twice as much memory for the "done" array, or by
 * using multiple bits of each entry in it */
void sched_crossover(struct rcps_solver *solver, struct rcps_problem *problem, 
		int *father, int *mother, int *son, int *daughter) {
	char *done;
	int i, j, q;

	q = rand() % problem->job_count;
	/* prepare the hash list */
	/* XXX put this array in each threads local struct so you don't need to
	 * realloc it each time */
	done = (char*)malloc(problem->job_count * sizeof(char));
	memset(done, 0, problem->job_count * sizeof(char));
	/* do the son */
	for (i = 0; i < q; i++) {
		son[i] = father[i];
		done[father[i]] = 1;
	}
	j = 0;
	for (i = 0; i < problem->job_count; i++) {
		if (!done[mother[i]]) {
			son[q + j] = mother[i];
			j++;
		}
	}
	/* do the daughter */
	memset(done, 0, problem->job_count * sizeof(char));
	for (i = 0; i < q; i++) {
		daughter[i] = mother[i];
		done[mother[i]] = 1;
	}
	j = 0;
	for (i = 0; i < problem->job_count; i++) {
		if (!done[father[i]]) {
			daughter[q + j] = father[i];
			j++;
		}
	}

	free(done);
}

void sched_crossover2(struct rcps_solver *solver, struct rcps_problem *problem, 
		int *father, int *mother, int *son, int *daughter) {
	char *done;
	int i, j, q1, q2;

	q1 = irand(problem->job_count);
	q2 = irand(problem->job_count-1);
	if (q2 >= q1) {
		q2++;
	}
	if (q1 > q2) {
		q1 ^= q2;
		q2 ^= q1;
		q1 ^= q2;
	}
	assert(q1 < q2);
	assert(q2 < problem->job_count);
	assert(0 <= q1);
	/* prepare the hash list */
	/* XXX put this array in each threads local struct so you don't need to
	 * realloc it each time */
	done = (char*)malloc(problem->job_count * sizeof(char));
	memset(done, 0, problem->job_count * sizeof(char));
	/* do the son */
	for (i = 0; i < q1; i++) {
		son[i] = father[i];
		done[father[i]] = 1;
	}
	for (i = q1; i < q2; i++) {
		j = 0;
		while (done[mother[j]]) {
			j++;
		}
		if (j < problem->job_count) {
			son[i] = mother[j];
			done[mother[j]] = 1;
		}
	}
	for (; i < problem->job_count; i++) {
		j = 0;
		while (done[father[j]]) {
			j++;
		}
		if (j < problem->job_count) {
			son[i] = father[j];
			done[father[j]] = 1;
		}
	}
	/* do the daughter */
	memset(done, 0, problem->job_count * sizeof(char));
	for (i = 0; i < q1; i++) {
		daughter[i] = mother[i];
		done[mother[i]] = 1;
	}
	for (i = q1; i < q2; i++) {
		j = 0;
		while (done[father[j]]) {
			j++;
		}
		if (j < problem->job_count) {
			daughter[i] = father[j];
			done[father[j]] = 1;
		}
	}
	for (; i < problem->job_count; i++) {
		j = 0;
		while (done[mother[j]]) {
			j++;
		}
		if (j < problem->job_count) {
			daughter[i] = mother[j];
			done[mother[j]] = 1;
		}
	}

	free(done);
}

int _before(struct rcps_solver *solver, struct rcps_problem *problem, 
		struct rcps_job *a, struct rcps_job *b) {
	int i;

	if (a == b) {
		return 1;
	}
	for (i = 0; i < b->predeccessor_count; i++) {
		if (_before(solver, problem, a, b->predeccessors[i])) {
			return 1;
		}
	}
	return 0;
}

int before(struct rcps_solver *solver, struct rcps_problem *problem, 
		int a,	int b) {
	int r;
	if (solver->predecessor_hash[a+b*problem->job_count] == -1) {
		r = _before(solver, problem, problem->jobs[a], problem->jobs[b]);
		solver->predecessor_hash[a+b*problem->job_count] = r;
		return r;
	}
	else {
		return solver->predecessor_hash[a+b*problem->job_count];
	}
}

void sched_mutation(struct rcps_solver *solver, struct rcps_problem *problem, 
	int *schedule, int p) {
	int i;
	int t;
	
	for (i = 0; i < problem->job_count-1; i++) {
		if (irand(10000) < p) {
			if (!before(solver, problem, schedule[i], 
					schedule[i+1])) {
				t = schedule[i];
				schedule[i] = schedule[i+1];
				schedule[i+1] = t;
			}
		}
	}
}

void crossover(int *father, int *mother, int *son, int *daughter, int size) {
	int i, j;
	i = irand(size);
	for (j = 0; j < i; j++) {
		son[j] = father[j];
		daughter[j] = mother[j];
	}
	for (; j < size; j++) {
		son[j] = mother[j];
		daughter[j] = father[j];
	}
}

void crossover2(int *father, int *mother, int *son, int *daughter, int size) {
	int i, q, j;
	i = irand(size);
	q = irand(size-1);
	if (q >= i) {
		q++;
	}
	if (i > q) {
		i ^= q;
		q ^= i;
		i ^= q;
	}
	for (j = 0; j < i; j++) {
		son[j] = father[j];
		daughter[j] = mother[j];
	}
	for (; j < q; j++) {
		son[j] = mother[j];
		daughter[j] = father[j];
	}
	for (; j < size; j++) {
		son[j] = father[j];
		daughter[j] = mother[j];
	}
}

void mutation(int *data, int *data_max, int size, int p) {
	int i;
	for (i = 0; i < size; i++) {
		if ((irand(10000) < p) && (data_max[i] != 0)) {
			data[i] = irand(data_max[i]);
			assert(data[i] < data_max[i]);
		}
	}
}

