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
	init_keyboard();
	usleep(100000);
	for(int i = 0; i < 4; i++) {
		typeKey(KEY_T);
		typeKey(KEY_E);
		typeKey(KEY_S);
		typeKey(KEY_T);
	}
	destroy_keyboard();
	return EXIT_SUCCESS;
}
