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
//void testGPIO(void);
int initGPIO(void);
void update(void);
void readKeyboard(void);

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf("Starting in terminal mode!");
	} else if(argc > 2 || !strcmp(argv[1], "-h")) {
		printf("Raspberry Pi Keyboard Matrix\n");
		printf("usage:\n");
		printf("(no arguments) - do not start as a daemon\n");
		printf("-d - start as daemon (current running daemon will be killed\n");
		printf("-k - kill running daemon\n");
		printf("-tg - test GPIO\n");
		printf("-tk - test virtual keyboard\n");
		exit(EXIT_SUCCESS);
	} else if(!strcmp(argv[1], "-d") || !strcmp(argv[1], "start")) {
		startDaemon();
	} else if(!strcmp(argv[1], "-k") || !strcmp(argv[1], "stop")) {
		killDaemon();
		exit(EXIT_SUCCESS);
//	} else if(!strcmp(argv[1], "-tg")) {
//		testGPIO();
	} else if(!strcmp(argv[1], "-tk")) {
		// test keyboard?
		initKeyboard();
		int i;
		for(int i = 0; i < 10; i++) {
			typeKey(KEY_T);
			typeKey(KEY_E);
			typeKey(KEY_S);
			typeKey(KEY_T);
		}
		exit(EXIT_SUCCESS);
	} else {
		printf("error: invalid options specified");
	}
	printf("---ACTIVE---\n");
	initKeyboard();
	usleep(50000);
	initGPIO();
	usleep(50000);

	while(1) {
		update();
		usleep(500);
	}
	destroyKeyboard();
	unlockPid();
	exit(EXIT_SUCCESS);
}
