/*
 * ReadMatrix.c
 *
 *  Created on: Jul 16, 2016
 *      Author: Nikita Wootten
 *
 *	Matrix for calculator's buttons is defined as follows:
 *	+---+-------+---------+------+-----------+------------+
 *	| ~ |   A   |    B    |  C   |     D     |     E      |
 *	+---+-------+---------+------+-----------+------------+
 *	| 0 | F1    | F2      | F3   | F4        | F5         |
 *	| 1 | 2ND   | MODE    | DEL  | LEFTARROW | UPARROW    |
 *	| 2 | ALPHA | X,t,O,n | Stat | DOWNARROW | RIGHTARROW |
 *	| 3 | MATH  | APPS    | PRGM | VARS      | CLEAR      |
 *	| 4 | X^-1  | SIN     | COS  | TAN       | ^          |
 *	| 5 | X^2   | ,       | (    | )         | /          |
 *	| 6 | LOG   | 7       | 8    | 9         | X          |
 *	| 7 | LN    | 4       | 5    | 6         | -          |
 *	| 8 | STO   | 1       | 2    | 3         | +          |
 *	| 9 | ON    | 0       | .    | (-)       | ENTER      |
 *	+---+-------+---------+------+-----------+------------+
 *
 *
 *      +-------------------------+----------+------------------------------------------------+
 *      | Key pressed             | Duration | Action                                         |
 *      +-------------------------+----------+------------------------------------------------+
 *      | 2nd + On                | >= 5 sec | Trigger relay to turn off the computer         |
 *      | 2nd + On                | < 5 sec  | Turn off screen and only listen for on button  |
 *      | On                      | Any      | Turn on screen                                 |
 *      | 2nd + MODE + up arrow   | Any      | Increase Screen Brightness                     |
 *      | 2nd + MODE + down arrow | Any      | Decrease Screen Brightness                     |
 *      +-------------------------+----------+------------------------------------------------+
 *
 *
 *	All key and key combinations should be converted as follows:
 *	+---------------------+----------------------------------+
 *	| uinput ID Triggered | Key pressed                      |
 *	+---------------------+----------------------------------+
 *	| Shift               | STAT                             |
 *	| Caps Lock           | ALPHA                            |
 *	| Control             | X,T,0,n                          |
 *	| Alt                 | VARS                             |
 *	| Tab                 | 2nd + ' '                        |
 *	| Escape              | 2nd + CLEAR                      |
 *	| Backspace           | CLEAR                            |
 *	| Enter               | ENTER                            |
 *	| Space               | ' '                              |
 *	| Fn (F1-F20)         | F(n + 5 * (2nd?) + 10 * (MODE?)) |
 *	| Print Screen        | 2nd + RCL                        |
 *	| Insert              | 2nd + DEL                        |
 *	| Home                | 2nd + Left Arrow                 |
 *	| End                 | 2nd + Right Arrow                |
 *	| Page Down           | 2nd + Down Arrow                 |
 *	| Page Up             | 2nd + Up Arrow                   |
 *	| Delete              | DEL                              |
 *	| Num Lock            | 2nd + ALPHA                      |
 *	| Up Arrow            | Up Arrow                         |
 *	| Down Arrow          | Down Arrow                       |
 *	| Left Arrow          | Left Arrow                       |
 *	| Right Arrow         | Right Arrow                      |
 *	| num (0-9)           | MODE + num                       |
 *	| char (a-z)          | char                             |
 *	| char (A-Z)          | Shift + char                     |
 *	| !                   | 2nd + i (imaginary number)       |
 *	| @                   | 2nd + theta                      |
 *	| #                   | 2nd + pi                         |
 *	| $                   | 2nd + S (alphabet)               |
 *	| %                   | 2nd + / (division symbol)        |
 *	| ^                   | 2nd + ^                          |
 *	| &                   | 2nd + +                          |
 *	| *                   | MODE + x (multiplication symbol) |
 *	| (                   | MODE + (                         |
 *	| )                   | MODE + )                         |
 *	| ~                   | 2nd + MODE + -                   |
 *	| `                   | 2nd + MODE + "                   |
 *	| -                   | MODE + -                         |
 *	| _                   | MODE + (-)                       |
 *	| =                   | 2nd + (-)                        |
 *	| +                   | MODE + +                         |
 *	| [                   | 2nd + [                          |
 *	| ]                   | 2nd + ]                          |
 *	| {                   | 2nd + {                          |
 *	| }                   | 2nd + }                          |
 *	| |                   | 2nd + I (alphabet)               |
 *	| \                   | 2nd + MODE + / (division symbol) |
 *	| ;                   | Shift + :                        |
 *	| :                   | :                                |
 *	| ‘                   | Shift + "                        |
 *	| “                   | "                                |
 *	| ,                   | MODE + ,                         |
 *	| .                   | MODE + .                         |
 *	| <                   | 2nd + MODE + (                   |
 *	| >                   | 2nd + MODE + )                   |
 *	| /                   | MODE + / (division symbol)       |
 *	| ?                   | ?                                |
 *	+---------------------+----------------------------------+
 *
 *	TODO: Add some fancy config options, for now just use my key setup
 */

#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "KeyboardInterface.h"

//command definitions
#define SET_SCREEN_OFF 10
#define SET_SCREEN_ON 11
#define GET_SCREEN_STATE 12

#define SET_SCREEN_BRIGHTNESS 15
#define GET_SCREEN_BRIGHTNESS 16

#define GET_CPU_TEMP 30

#define TURN_OFF_POWER 40

#define GO_TO_SLEEP 101
#define WAKE_UP 102

//button matrix dimensions

#define ROW_COUNT 10
#define COLUMN_COUNT 5

// not an RNG I swear
int rowPins[] = {2, 0, 7, 4, 21, 23, 22, 1, 3, 24};
int colPins[] = {27, 26, 29, 28, 25};

int buttons[ROW_COUNT][COLUMN_COUNT] = {{0}};

int keyMap[4][ROW_COUNT][COLUMN_COUNT] = {
		{
				{KEY_F1,		KEY_F2,			KEY_F3,			KEY_F4,			KEY_F5},
				{-1,			-1,				KEY_DELETE,		KEY_LEFT,		KEY_UP},
				{KEY_CAPSLOCK,	KEY_RIGHTCTRL,	KEY_LEFTSHIFT,	KEY_DOWN,		KEY_RIGHT},
				{KEY_A,			KEY_B,			KEY_C,			KEY_LEFTALT,	KEY_BACKSPACE},
				{KEY_D,			KEY_E,			KEY_F,			KEY_G,			KEY_H},
				{KEY_I,			KEY_J,			KEY_K,			KEY_L,			KEY_M},
				{KEY_N,			KEY_O,			KEY_P,			KEY_Q,			KEY_R},
				{KEY_S,			KEY_T,			KEY_U,			KEY_V,			KEY_W},
				{KEY_X,			KEY_Y,			KEY_Z,			-1,				KEY_APOSTROPHE},
				{-1,			KEY_SPACE,		KEY_SEMICOLON,	-1,				KEY_ENTER}
		},
		{
				{KEY_F1,		KEY_F2,			KEY_F3,			KEY_F4,			KEY_F5},
				{-1,			-1,				KEY_DELETE,		KEY_LEFT,		KEY_UP},
				{KEY_CAPSLOCK,	KEY_RIGHTCTRL,	KEY_LEFTSHIFT,	KEY_DOWN,		KEY_RIGHT},
				{KEY_A,			KEY_B,			KEY_C,			KEY_LEFTALT,	KEY_BACKSPACE},
				{KEY_D,			KEY_E,			KEY_F,			KEY_G,			KEY_H},
				{KEY_I,			KEY_J,			KEY_K,			KEY_L,			KEY_M},
				{KEY_N,			KEY_7,			KEY_8,			KEY_9,			KEY_R},
				{KEY_S,			KEY_4,			KEY_5,			KEY_6,			KEY_MINUS},
				{KEY_X,			KEY_1,			KEY_2,			KEY_3,			KEY_EQUAL},
				{-1,			KEY_0,			KEY_SEMICOLON,	-1,				KEY_ENTER}
		},
		{
				{KEY_F1,		KEY_F2,			KEY_F3,			KEY_F4,			KEY_F5},
				{-1,			-1,				KEY_DELETE,		KEY_LEFT,		KEY_UP},
				{KEY_CAPSLOCK,	KEY_RIGHTCTRL,	KEY_LEFTSHIFT,	KEY_DOWN,		KEY_RIGHT},
				{KEY_A,			KEY_B,			KEY_C,			KEY_LEFTALT,	KEY_BACKSPACE},
				{KEY_D,			KEY_E,			KEY_F,			KEY_G,			KEY_H},
				{KEY_I,			KEY_J,			KEY_K,			KEY_L,			KEY_M},
				{KEY_N,			KEY_7,			KEY_8,			KEY_9,			KEY_R},
				{KEY_S,			KEY_4,			KEY_5,			KEY_6,			KEY_MINUS},
				{KEY_X,			KEY_1,			KEY_2,			KEY_3,			KEY_EQUAL},
				{-1,			KEY_0,			KEY_SEMICOLON,	-1,				KEY_ENTER}
		},
		{
				{KEY_F1,		KEY_F2,			KEY_F3,			KEY_F4,			KEY_F5},
				{-1,			-1,				KEY_DELETE,		KEY_LEFT,		KEY_UP},
				{KEY_CAPSLOCK,	KEY_RIGHTCTRL,	KEY_LEFTSHIFT,	KEY_DOWN,		KEY_RIGHT},
				{KEY_A,			KEY_B,			KEY_C,			KEY_LEFTALT,	KEY_BACKSPACE},
				{KEY_D,			KEY_E,			KEY_F,			KEY_G,			KEY_H},
				{KEY_I,			KEY_J,			KEY_K,			KEY_L,			KEY_M},
				{KEY_N,			KEY_7,			KEY_8,			KEY_9,			KEY_R},
				{KEY_S,			KEY_4,			KEY_5,			KEY_6,			KEY_MINUS},
				{KEY_X,			KEY_1,			KEY_2,			KEY_3,			KEY_EQUAL},
				{-1,			KEY_0,			KEY_SEMICOLON,	-1,				KEY_ENTER}
		}
};

int fd;

int initGPIO(void) {
	//try to open the serial port
	if((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0 ){
		printf("/dev/ttyAMA0 could not be opened.\n");
		return -1;
	}
	if (wiringPiSetup() == -1) {
		printf("WiringPi Failed\n");
		return -1;
	}

	int i;
	for(i = 0; i < ROW_COUNT; i++){
		pinMode(rowPins[i], INPUT);
		pullUpDnControl(rowPins[i], PUD_DOWN) ;
	}
	for(i = 0; i < COLUMN_COUNT; i++){
		pinMode(colPins[i], OUTPUT);
		digitalWrite(colPins[i], LOW);
	}
	return 0;
}

int getModifier(){
	int is2nd = buttons[8][1]; //[y][x]
	int isMod = buttons[8][0];

	return is2nd * 2 + isMod;
}

int screenBrightness = 255;
void update(){
	int x, y, i, modifier, key;
	int state, previousState;
	for(x=0; x<COLUMN_COUNT; x++){
		digitalWrite(colPins[x], HIGH);
		delay(1);
		for(y=0; y<ROW_COUNT; y++){
			i = 10 * x + y;
//			printf("x:%d y:%d i:%d\n", x, y, i);
			state = digitalRead(rowPins[y]);
			previousState = buttons[y][x];
			buttons[y][x] = state;
			if(state && !previousState){
				modifier = getModifier();
				key = keyMap[modifier][9-y][x];
/*				if(modifier == 3 && (key == KEY_UP || key == KEY_DOWN)){
					if(key == KEY_UP){
						screenBrightness++;
					} else {
						screenBrightness--;
					}
					if(screenBrightness > 255){
						screenBrightness = 255;
					}
					if(screenBrightness < 0){
						screenBrightness = 0;
					}
					serialPutchar(fd, SET_SCREEN_BRIGHTNESS);
					serialPutchar(fd, screenBrightness);
				}*/
				printf("press at (%d, %d) mod: %d, key: %d\n", x, y, modifier, key);
				// update the pressed key
				sendKeyByKeycode(key, 1);
				sendSync();
			}
			if(!state && previousState){
				printf("release at (%d, %d)\n", x, y);
				for(modifier=0; modifier<4; modifier++){
					key = keyMap[modifier][9-y][x];
					printf("    mod: %d, key: %d\n", modifier, key);
					// update released key
					sendKeyByKeycode(key, 0);
					sendSync();
				}
			}
//			printf("%x", state);
		}
		digitalWrite(colPins[x], LOW);
//		printf("\n");
	}
//	printf("----------\n");
}

/*int main (int argc, char **argv) {
	initGPIO();
	while(1){
		update();
		delay(5);
	}
}*/




void testGPIO(void) {
	printf("Test Mode - Testing wiringPi\n");
	sleep(1);
	printf("Initializing wiringPi!\n");
	int result = initGPIO();
	if(result == -1) {
		printf("Failure to initialize wiringPi\n");
		exit(EXIT_FAILURE);
	}
	printf("Success! Testing pin initialization - read\n");
	sleep(1);
	pinMode(2, INPUT);
	printf("Testing pin initialization - write\n");
	sleep(1);
	pinMode(27, OUTPUT);
	printf("Testing pin read\n");
	sleep(1);
	printf("Value of pin 2 is %x\n", digitalRead(2));
	printf("Testing pin write\n");
	printf("Writing HIGH to pin 27\n");
	sleep(1);
	digitalWrite(27, HIGH);
	printf("Finished all tests. Exiting\n");
	exit(EXIT_SUCCESS);
}

/*int errorInt = -1;

int* getColumn(int column_number) {
	if(column_number >= COLUMN_COUNT || column_number < 0) {
		printf("error: column out of bounds exception");
		return &errorInt;
	}
	digitalWrite(colPins[column_number], HIGH);
	delay(1);
	static int values[ROW_COUNT];
	int i;
	for(i = 0; i < ROW_COUNT; i++) {
		values[i] = digitalRead(rowPins[i]) - 1;
		if(values[i] == -1) {
			values[i] = 1;
		}
	}
	digitalWrite(colPins[column_number], LOW);
	return values;
}

int* pollKeyboard(void) {
	static int keys[MAXIMUM_PRESSED][2];
	int number_of_keys = 0;
	int i, j;
	for(i = 0; i < COLUMN_COUNT; i++) {
		int* values = getColumn(i);
		for(j = 0; j < ROW_COUNT; j++) {
			if(number_of_keys > MAXIMUM_PRESSED) {
				keys[number_of_keys][0] = -1;
				keys[number_of_keys][1] = -1;
				number_of_keys++;
			} else if(*(values + j) == 1) {
				keys[number_of_keys][0] = i;
				keys[number_of_keys][1] = j;
				number_of_keys++;
			}
		}
	}
	return keys;
}

static int oldKeys[MAXIMUM_PRESSED];

void sendSync(void);
void sendKeyByKeycode(int, int);

void update(void) {
	int *keys = pollKeyboard();
	int isMod = 0;
	int is2nd = 0;


	// Since we can only return an array as a pointer to the first element,
	// New "key-press" on every second pointer increment.

	int i;
	// Determine if modifiers where pressed
	for(i = 0; i < (2 * MAXIMUM_PRESSED); i += 2) {
		if(keys[i][0] != -1 && keys[i][1] != -1) {
			if(keys[i][1] == 1) { // If second row
				if(keys[i][0] == 0) {
					is2nd = 1;
				} else if(keys[i][1] == 1) {
					isMod = 1;
				}
			}
		}
	}

	// Determine what keycodes map to use
	int keyMapModifier;
	if(isMod == 0 && is2nd == 0) {
		keyMapModifier = 0;
	} else if(isMod == 1 && is2nd == 0) {
		keyMapModifier = 1;
	} else if(is2nd == 1 && isMod == 0) {
		keyMapModifier = 2;
	} else {
		keyMapModifier = 3;
	}

	// finalKeys contains "pressed keys" in first half, and "unpressed" values in second.
	int finalKeys[2 * MAXIMUM_PRESSED];

	// match matrix value with key code for pressed keys
	int j;
	for(i = 0; i < (2 * MAXIMUM_PRESSED); i += 2) {
		if(keys[i][1] != -1 && keys[i][0] != -1) {
			finalKeys[i / 2] = keyMap[keyMapModifier][keys[i][1]][keys[i][0]];

			for(j = 0; j < MAXIMUM_PRESSED; j++) {
				if(finalKeys[i / 2] == oldKeys[j]) {
					finalKeys[(i / 2) + MAXIMUM_PRESSED] = oldKeys[j];
				}
				// replace old key at index with new key
				oldKeys[j] = finalKeys[i / 2];
			}
		} else {
			finalKeys[i / 2] = -1;
		}
	}

	// update the keys!
	for(i = 0; i < MAXIMUM_PRESSED; i++) {
		if(finalKeys[i] != -1) {
			sendKeyByKeycode(finalKeys[i], 1);
			sendSync();
		}
	}
	// update released keys
	for(i = MAXIMUM_PRESSED; i < (2 * MAXIMUM_PRESSED); i++) {
		if(finalKeys[i] != -1) {
			sendKeyByKeycode(finalKeys[i], 0);
			sendSync();
		}
	}
}*/

