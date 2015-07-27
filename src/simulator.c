#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h> /* usleep, requires BSD_SOURCE */
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
	struct tm *tm;

	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);

	return (tm->tm_hour % 12)*60*60*1000 + 
		(tm->tm_min)*60*1000 + (tm->tm_sec)*1000 + (tv.tv_usec % 1000);
}

void *counter_sim(void *args) {
	gtime_t cur_time_millis;
	//time_t rawtime;
	//struct tm *timeinfo;
	//struct timespec ts;
	
	while (1) {
		// determine time
		//clock_gettime(CLOCK_REALTIME, &ts);
		//timeinfo = localtime(&ts.tv_sec);
		//cur_time_millis = timeinfo->tm_sec + 60*timeinfo->tm_min +
		//	60*60*(timeinfo->tm_hour % 12);
		cur_time_millis = sample_cur_time();

		// update time global variable
		pthread_mutex_lock(&global_time_millis_mutex);
		global_time_millis = cur_time_millis;
		pthread_mutex_unlock(&global_time_millis_mutex);

		// sleep for a while
		usleep(1000);
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

// called roughly every millisecond
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
		global_inner_intensity[lidx] = UINT8_MAX/5 * (5 - (secs % 5));
		global_inner_intensity[ridx] = UINT8_MAX/5 * ((secs % 5));

		// hours and minutes are always solid
		global_outer_intensity[hours] = 255;
		global_inner_intensity[mins/5] = 255;

		// sleep for a while
		pthread_mutex_unlock(&global_time_millis_mutex);
		pthread_mutex_unlock(&global_intensity_mutex);
		usleep(1000L);
	}

	return NULL;
}

