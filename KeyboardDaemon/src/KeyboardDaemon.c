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
#include "Daemonize.h"

int strcmp(char *, char *);
void killDaemon();
void usleep(int);

int main(int argc, char *argv[]) {
	if(argc > 2 || !strcmp(argv[1], "-h")) {
		printf("Raspberry Pi Keyboard Matrix\n");
		printf("usage:\n");
		printf("(no arguments) - do not start as a daemon\n");
		printf("-d - start as daemon (current running daemon will be killed\n");
		printf("-k - kill running daemon");
	} else if(!strcmp(argv[1], "-d")) {
		startDaemon();
	} else if(!strcmp(argv[1], "-k")) {
		killDaemon();
	} else {
		printf("Starting in terminal\n");
	}

	initKeyboard();
	usleep(100000);
//	for(int i = 0; i < 4; i++) {
//		typeKey(KEY_T);
//		typeKey(KEY_E);
//		typeKey(KEY_S);
//		typeKey(KEY_T);
//	}
	while(1) {
		update();
		usleep(100);
	}
	destroyKeyboard();
	unlockPid();
	exit(EXIT_SUCCESS);
}
