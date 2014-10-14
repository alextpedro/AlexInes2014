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