#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include <pthread.h>

// simulator updates these global variables
typedef uint32_t gtime_t;
pthread_mutex_t global_time_millis_mutex;
gtime_t global_time_millis;

pthread_mutex_t global_intensity_mutex;
uint8_t global_outer_intensity[12];
uint8_t global_inner_intensity[12];

// interface to multithreaded simulator
void start_sim(void);	// start simulator threads
void wait_sim(void);	// block until all threads are done

#endif
