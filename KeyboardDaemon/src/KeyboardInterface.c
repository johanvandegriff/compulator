/*
 * KeyboardInterface.c
 *
 *  Created on: Jul 16, 2016
 *      Author: Nikita Wootten
 *
 *	This file can simulate a virtual keyboard.
 *
 *	Remember:
 *	-BEFORE USE, CALL THE init_keyboard() FUNCTION!
 *	-Be sure to call sendSync() after using sendKey()
 *	-If program outputs "Error opening keyboard device", modprobe uinput!
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

int fd;

// Initialize uinput keyboard
void initKeyboard(void) {
	// Set up uinput device
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if(fd < 0) {
		printf("Error opening keyboard device\n");
		exit(EXIT_FAILURE);
	}

	// Enable key press/release and synchronization events
	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_EVBIT, EV_SYN);

	// Enable keypress events of any key
	int i;
	for(i = 0; i < 256; i++) {
		if(ioctl(fd, UI_SET_KEYBIT, i) < 0) {
			printf("error: associating key %x\n", i);
		}
	}

	// "Vendor" information
	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));

	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "rPi-matrix-keyboard");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;

	// Create the device
	if(write(fd, &uidev, sizeof(uidev)) < 0) {
		printf("Error writing keyboard creation");
		exit(EXIT_FAILURE);
	}

	if(ioctl (fd, UI_DEV_CREATE) < 0) {
		printf("Error creating keyboard");
		exit(EXIT_FAILURE);
	}
}

// Sync keyboard events
int sendSync(void) {
	struct input_event ev;
	ev.type = EV_SYN;
	ev.code = SYN_REPORT;
	ev.value = 0;
	if(write(fd, &ev, sizeof(struct input_event)) < 0) {
		printf("error: writing sync");
		return 1;
	}
	return 0;
}

int sendKeyByKeycode(int key, int value) {
	struct input_event ev;
	//memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_KEY;
	ev.code = key;
	ev.value = value;
	if(write(fd, &ev, sizeof(struct input_event)) < 0) {
		printf("error writing event");
		return 1;
	}
	//sendSync(); /* Keyboard sync events need to be handled elsewhere */
	return 0;
}

int sendKeyByInputEvent(struct input_event ev) {
	if(write(fd, &ev, sizeof(struct input_event)) < 0) {
		printf("error writing event");
		return 1;
	}
	return 0;
}

// Destroy uinput keyboard
void destroyKeyboard() {
	sleep(2);
	if(ioctl(fd, UI_DEV_DESTROY) < 0) {
		printf("error: destroying");
	}
	close(fd);
}

void typeKey(int key) {
	sendKeyByKeycode(key, 1);
	usleep(100000);
	sendKeyByKeycode(key, 0);
	usleep(100000);
}
