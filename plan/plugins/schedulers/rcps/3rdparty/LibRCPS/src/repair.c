#ifdef _WIN32
# include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <assert.h>

#include "repair.h"

int repair(struct rcps_problem *problem, struct rcps_genome *genome) {
	int i, j, k;
	struct rcps_mode *cmode;
	struct rcps_alternative *calternative;
	struct rcps_alternative *palternative;
	int icalternative;
	int ipalternative;
	// XXX perhasp only allocate this once in solver
	int *load;

	load = (int*)malloc(sizeof(int)*problem->resource_count);
		
	for (i = 0; i < problem->job_count; i++) {
		if (problem->jobs[i]->genome_position == -1) {
			continue;
		}
		cmode = problem->jobs[i]->modes[genome->modes[
			problem->jobs[i]->genome_position]];
		/* check this job */
		for (j = 0; j < problem->resource_count; j++) {
			load[j] = 0;
		}
		for (j = 0; j < cmode->request_count; j++) {
			// XXX genome_index stuff
			if (cmode->requests[j]->genome_position == -1) {
				icalternative = 0;
			}
			else {
				icalternative =	genome->alternatives[
					cmode->requests[j]->genome_position];
			}
			calternative = cmode->requests[j]->alternatives[icalternative];
			load[icalternative] += calternative->amount;
			if (load[calternative->resource->index] > 
					calternative->resource->avail) {
				/* this one needs repairs */
				for (k = 0; k < cmode->requests[j]->alternative_count; k++) {
					if (cmode->requests[j]->alternatives[k] == calternative) {
						continue;
					}
					palternative = cmode->requests[j]->alternatives[k];
					ipalternative = k;
					load[icalternative] -= calternative->amount;
					if ((load[ipalternative] + palternative->amount) 
							<= palternative->resource->avail) {
						load[ipalternative] += palternative->amount;
						genome->alternatives[j]	= k;
						break;
					}
					load[icalternative] += calternative->amount;
				}
				if (k == cmode->requests[j]->alternative_count) {
					free(load);
					return 0;
				}
			}
		}
	}
	free(load);
	return 1;
}
