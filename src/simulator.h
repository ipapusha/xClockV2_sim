#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include <pthread.h>

// simulator updates these global variables
pthread_mutex_t global_time_secs_mutex;
uint16_t global_time_secs;

pthread_mutex_t global_intensity_mutex;
uint8_t global_hour_intensity[12];
uint8_t global_minute_intensity[12];

// interface to multithreaded simulator
void start_sim(void);	// start simulator threads
void wait_sim(void);	// block until all threads are done

#endif
