/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#include <signal.h>
#include <stdio.h>

#include "common.h"
#include "debug.h"

int running = 1;

static void signal_handler (int);

void install_signal_handler (void) {
	struct sigaction config;

	config.sa_handler = signal_handler;
	sigemptyset(&config.sa_mask);
	config.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &config, NULL) != 0) {
		ERROR(ERR_SIG_INSTALL, "sigaction() failed");
	}
}

void signal_handler (int signal) {
	(void) signal;
	running = 0;
}

//TODO - So not finished.
float compression_ratio(int sizeOfCompressed, int sizeOfDecompressed) {
	float ratio = 0;
	ratio = (1 - (sizeOfCompressed / sizeOfDecompressed) ) * 100;
	return ratio;
}