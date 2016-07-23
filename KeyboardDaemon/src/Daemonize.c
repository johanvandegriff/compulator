/*
 * Daemonize.c
 *
 *  Created on: Jul 16, 2016
 *      Author: Nikita Wootten
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define PID_LOCATION "/tmp/keyboard_daemon.pid"

int fork(void);
int write(int, char *, int);
char getpid();
int close(char);
int read(int, char *, int);

// Check for existance of pid file, pid
long checkDaemon(void) {
	char pid_str[10];

	int pid_fd = open(PID_LOCATION, O_RDONLY, 0600);
	if(pid_fd < 0) {
		printf("Error reading pid file");
		return 0;
	}
	if(read(pid_fd, pid_str, 10) > 0) {
		close(pid_fd);
		return strtol(pid_str, NULL, 0);
	}
	close(pid_fd);
	return 0;
}

// Delete pid file
void unlockPid(void) {
	if(remove(PID_LOCATION) != 0) {
		printf("Error: unable to delete pid file");
	}
}

void killDaemon(void) {
	long pid = checkDaemon();
	if(pid == 0) {
		return;
	} else {
		if(kill(pid, SIGTERM) < 0) {
			printf("Error: could not kill daemon, continuing\n");
			return;
		}
		unlockPid();
	}
}

void startDaemon(void) {
	//check for existence of daemon
	if(checkDaemon() != 0) {
		printf("Another instance is already running! Killing older instance.");
		killDaemon();
	}

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

	// Write pid file
	int pid_fd = open(PID_LOCATION, O_RDWR|O_CREAT, 0600);
	if(pid_fd < 0) {
		printf("Error opening pid file, exiting (NOT DAEMON)");
		exit(EXIT_FAILURE);
	}
	char pid_str[10];
	sprintf(pid_str, "%d\n", getpid());

	if(write(pid_fd, pid_str, strlen(pid_str)) < 0) {
		printf("Error writing pid file, exiting (NOT DAEMON)");
	}
	close(pid_fd);

	/* Everything after this line is in the child process! */
}
