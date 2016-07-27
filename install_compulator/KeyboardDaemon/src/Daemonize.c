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
int kill(long, int);

// Check for existance of pid file, pid
long checkDaemon(void) {
	char pid_str[10];

	int pid_fd = open(PID_LOCATION, O_RDONLY, 0600);
	if(pid_fd < 0) {
		printf("error reading pid file\n");
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
		printf("error: unable to delete pid file\n");
	}
}

void killDaemon(void) {
	long pid = checkDaemon();
	if(pid == 0) {
		return;
	} else {
		if(kill(pid, SIGTERM) < 0) {
			printf("error: could not kill daemon, continuing\n");
			return;
		}
		printf("killed daemon, continuing\n");
		unlockPid();
	}
}

void startDaemon(void) {
	//check for existence of daemon
	if(checkDaemon() != 0) {
		printf("Another instance is already running! Killing older instance.\n");
		killDaemon();
	}

	// Daemonize the process
	pid_t pid;
	pid = fork();

	if(pid < 0) {
		// Fork was a failure
		printf("Fork failure! Exiting.\n");
		exit(EXIT_FAILURE);
	}

	if(pid > 0) {
		// Fork was a success
		printf("Fork successful! PID: %d\n", pid);
		exit(EXIT_SUCCESS);
	}

	// Write pid file
	int pid_fd = open(PID_LOCATION, O_RDWR|O_CREAT, 0600);
	if(pid_fd < 0) {
		printf("Error opening pid file, exiting (NOT DAEMON)\n");
		exit(EXIT_FAILURE);
	}
	char pid_str[10];
	sprintf(pid_str, "%d\n", getpid());

	if(write(pid_fd, pid_str, strlen(pid_str)) < 0) {
		printf("Error writing pid file, exiting (NOT DAEMON)\n");
	}
	close(pid_fd);

	/* Everything after this line is in the child process! */
}
