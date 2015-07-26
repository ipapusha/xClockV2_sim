#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

volatile uint16_t global_time_secs;
pthread_mutex_t global_time_secs_mutex;

void *inc_sec_function(void *args)
{
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

void *time_printer_function(void *args)
{
	while (1) {
		pthread_mutex_lock(&global_time_secs_mutex);
		printf("%d\n", global_time_secs);
		pthread_mutex_unlock(&global_time_secs_mutex);
		usleep(1000000L);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t inc_sec_thread;
	pthread_t time_printer_thread;
	
	// create thread to increment global variable
	if (pthread_create(&inc_sec_thread, NULL, inc_sec_function, NULL) != 0) {
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&time_printer_thread, NULL, time_printer_function, NULL) != 0) {
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	}

	// join threads
	pthread_join(inc_sec_thread, NULL);
	return EXIT_SUCCESS;
}
