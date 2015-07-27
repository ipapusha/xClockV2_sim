#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h> /* usleep, requires BSD_SOURCE */

#include "simulator.h"

/*
 * =============================================================================
 * register worker thread callbacks here
 * =============================================================================
 */
void *counter_sim(void *args);
void *console_printer(void *args);
void *(*thread_fun[2])(void *) = {counter_sim, console_printer};

/*
 * =============================================================================
 * external interface to simulation file, don't need to change anything here
 * =============================================================================
 */

// count the number of worker threads (at compile time)
// using the array of registered function pointers thread_fun
#define countof(a) (sizeof(a) / sizeof(*(a)))
#define NTHREADS (countof(thread_fun))
static pthread_t sim_threads[NTHREADS];

// start the worker threads
void start_sim(void) {
	// create worker threads
	int i;
	for (i = 0; i < NTHREADS; ++i) {
		if (pthread_create(&sim_threads[i], NULL, (*thread_fun[i]), NULL) != 0) {
			fprintf(stderr, "Error creating thread %d\n", i);
			exit(EXIT_FAILURE);
		}
	}
}

// block until all workers done
void wait_sim(void) {
	int i;
	for (i = 0; i < NTHREADS; ++i) {
		if (sim_threads[i]) {
			pthread_join(sim_threads[i], NULL);
		}
	}
}

/*
 * =============================================================================
 * worker thread definitions
 * =============================================================================
 */

void *counter_sim(void *args) {
	uint16_t cur_time_secs;
	time_t rawtime;
	struct tm *timeinfo;
	
	while (1) {
		// determine time
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		cur_time_secs = timeinfo->tm_sec + 60*timeinfo->tm_min +
			60*60*(timeinfo->tm_hour % 12);

		// update time global variable
		pthread_mutex_lock(&global_time_secs_mutex);
		global_time_secs = cur_time_secs;
		pthread_mutex_unlock(&global_time_secs_mutex);

		// sleep for a while
		usleep(1000);
	}

	return NULL;
}

void *console_printer(void *args) {
	while (1) {
		pthread_mutex_lock(&global_time_secs_mutex);
		printf("%d\n", global_time_secs);
		pthread_mutex_unlock(&global_time_secs_mutex);
		usleep(1000000L);
	}

	return NULL;
}

