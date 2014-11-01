/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#include <signal.h>
#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "memory.h"

int running = 1;

static void signal_handler(int);

void install_signal_handler(void) {
	struct sigaction config;

	catch_signal();
	config.sa_handler = signal_handler;
	sigemptyset(&config.sa_mask);
	config.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &config, NULL) != 0) {
		printf("sigaction() failed");
	}
}

void signal_handler(int signal) {
	(void) signal;
	running = 0;
	
}

void catch_signal () {
	if (running == 0) {
		printf("Operation interrupted by user @<todo date and time>\n");
	}
}

float compression_ratio(float sizeOfCompressed, float sizeOfDecompressed) {
	float ratio = 0;

	if (sizeOfDecompressed == 0) {
		return ratio;
	}

	ratio = ((1 - (sizeOfCompressed / sizeOfDecompressed) ) * 100);
	return ratio;
}

