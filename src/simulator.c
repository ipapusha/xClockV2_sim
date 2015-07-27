#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h> /* usleep, requires _BSD_SOURCE */
#include <sys/time.h>

#include "simulator.h"


/*
 * =============================================================================
 * register worker thread callbacks here
 * =============================================================================
 */
void *counter_sim(void *args);
void *console_printer(void *args);
void *determine_intensities(void *args);
void *(*thread_fun[3])(void *) = { 
	counter_sim, console_printer, determine_intensities 
};

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

gtime_t sample_cur_time(void) {
	struct timeval tv;
	struct timezone tz;
	
	gettimeofday(&tv, &tz);

	// UTC
	//return (tv.tv_sec % (12*60*60))*1000 + (tv.tv_usec/1000L);
	
	// in local timezone
	return ((tv.tv_sec - (tz.tz_minuteswest - 60*tz.tz_dsttime)*60) % (12*60*60))*1000 + (tv.tv_usec/1000L);
}

void *counter_sim(void *args) {
	gtime_t cur_time_millis;
	
	while (1) {
		// determine time
		cur_time_millis = sample_cur_time();

		// update time global variable
		pthread_mutex_lock(&global_time_millis_mutex);
		global_time_millis = cur_time_millis;
		pthread_mutex_unlock(&global_time_millis_mutex);

		// sleep for a while
		usleep(8000);
	}

	return NULL;
}

void *console_printer(void *args) {
	gtime_t cur_time_millis;
	uint8_t hours, mins, secs;
	uint16_t millis;
	while (1) {
		// sample the time
		pthread_mutex_lock(&global_time_millis_mutex);
		cur_time_millis = global_time_millis;
		pthread_mutex_unlock(&global_time_millis_mutex);
		
		// print results
		millis = cur_time_millis % 1000;
		secs   = (cur_time_millis / 1000) % 60;
		mins   = (cur_time_millis / (60*1000)) % 60;
		hours  = (cur_time_millis / (60*60*1000)) % 12;
		
		printf("G|%d|, HMS|%02d:%02d:%02d.%03d| \n", cur_time_millis, hours, mins, secs, millis);
		
		usleep(1000000L);
	}

	return NULL;
}

// called roughly every 1/125 second
void *determine_intensities(void *args) {
	gtime_t cur_time_millis;
	uint8_t hours, mins, secs;
	uint16_t millis;

	uint8_t i;
	uint8_t lidx, ridx;

	while (1) {
		pthread_mutex_lock(&global_time_millis_mutex);
		pthread_mutex_lock(&global_intensity_mutex);
		
		// sample the time
		cur_time_millis = global_time_millis;
		
		// determine time from the number of seconds since 12:00:00
		millis = cur_time_millis % 1000;
		secs   = (cur_time_millis / 1000) % 60;
		mins   = (cur_time_millis / (60*1000)) % 60;
		hours  = (cur_time_millis / (60*60*1000)) % 12;
		
		// update intensities
		for (i = 0; i < 12; ++i) {
			global_outer_intensity[i] = 0;
			global_inner_intensity[i] = 0;
		}
		
		// seconds have different brightnesses
		lidx = secs / 5;
		ridx = (lidx + 1) % 12;

		float frac = ((secs % 5)*1000.0 + millis)/5000.0;

		global_inner_intensity[lidx] = (uint8_t)(UINT8_MAX * (1-frac));
		global_inner_intensity[ridx] = (uint8_t)(UINT8_MAX * (frac));

		// hours and minutes are always solid
		global_outer_intensity[hours] = 255;
		global_inner_intensity[mins/5] = 255;

		// sleep for a while
		pthread_mutex_unlock(&global_time_millis_mutex);
		pthread_mutex_unlock(&global_intensity_mutex);
		
		usleep(8000L);
	}

	return NULL;
}

