/*
 ============================================================================
 Name        : KeyboardDaemon.c
 Author      : Nikita Wootten
 Version     : 0.01
 Copyright   : Your copyright notice
 Description : Poll input from raspberry pi matrix and type corresponding key
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <linux/uinput.h>
#include "KeyboardInterface.h"

int main(void) {
	// Daemonize the process
	pid_t pid;
	pid = fork();

	if(pid < 0) {
		// Fork was a failure
		printf("Fork failure! Exiting.");
		exit(EXIT_FAILURE);
	}

	if(pid > 0) {
		// Fork was a success
		printf("Fork successful! PID: %x\n", pid);
		exit(EXIT_SUCCESS);
	}

	// Child process starts here
	init_keyboard();
	usleep(100000);
	for(int i = 0; i < 4; i++) {
		typeKey(KEY_T);
		typeKey(KEY_E);
		typeKey(KEY_S);
		typeKey(KEY_T);
	}
	destroy_keyboard();
	exit(EXIT_SUCCESS);
}
