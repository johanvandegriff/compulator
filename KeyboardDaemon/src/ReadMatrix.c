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
#include <stdio.h>
#include <stdlib.h>
//#include <wiringPi.h>

#define PIN 0
#define DELAY 2000
#define NUM_PULSES 1
#define PULSE_WIDTH 20

#define COLUMN_COUNT 5
#define ROW_COUNT 10
#define MAXIMUM_PRESSED 5

int rowPins[] = {4,5,6,7,8,9,10,11};
int colPins[] = {12,13,14,15,16,1718};

struct keyPress {
	int column;
	int row;
};

struct keyPress currentlyPressed[MAXIMUM_PRESSED];

int* getColumn(int row_number) {
	static int values[ROW_COUNT];
	// TODO actually poll for keyboard values?
	return values;
}

struct keyPress* pollKeyboard() {
	static struct keyPress keys[MAXIMUM_PRESSED];
	int number_of_keys = 0;
	int i, j;
	for(i = 0; i < ROW_COUNT; i++) {
		int* values = getColumn(i);
		for(j = 0; j < COLUMN_COUNT; j++) {
			if(number_of_keys > MAXIMUM_PRESSED) {
				keys[number_of_keys].column = -1;
				keys[number_of_keys].row = -1;
				number_of_keys++;
			} else if(*(values + j) == 1) {
				keys[number_of_keys].column = i;
				keys[number_of_keys].row = j;
				number_of_keys++;
			}
		}
	}
	return keys;
}

int oldKeys[MAXIMUM_PRESSED];

void sendSync(void);
void sendKeyByKeycode(int, int);

void update(void) {
	int *keys = (int *) pollKeyboard();
	int isMod = 0;
	int is2nd = 0;

	/*
	 * Since we can only return an array as a pointer to the first element,
	 * New "key-press" on every second pointer increment.
	 */
	int i;
	// Determine if modifiers where pressed
	for(i = 0; i < (2 * MAXIMUM_PRESSED); i += 2) {
		if(*(keys + 1) != -1 && *keys != -1) {
			if(*(keys + 1) == 1) { /* If second row */
				if(keys == 0) {
					is2nd = 1;
				} else if(*keys == 1) {
					isMod = 1;
				}
			}
		}
	}

	// Determine what keycodes map to use
	int keyMap[4][ROW_COUNT][COLUMN_COUNT] = {
			{
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
			},
			{
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
			},
			{
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
			},
			{
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
			}
	};
	int keyMapModifier;
	if(isMod == 0 && is2nd == 0) {
		keyMapModifier = 0;
	} else if(isMod == 1) {
		keyMapModifier = 1;
	} else if(is2nd == 1) {
		keyMapModifier = 2;
	} else {
		keyMapModifier = 3;
	}

	// finalKeys contains "pressed keys" in first half, and "unpressed" values in second.
	int finalKeys[2 * MAXIMUM_PRESSED];

	// match matrix value with key code for pressed keys
	int j;
	for(i = 0; i < (2 * MAXIMUM_PRESSED); i += 2) {
		if(*(keys + 1) != -1 && *keys != -1) {
			finalKeys[i / 2] = keyMap[keyMapModifier][*(keys + 1)][*keys];

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
		sendKeyByKeycode(finalKeys[i], 1);
	}
	// update released keys
	for(i = MAXIMUM_PRESSED; i < (2 * MAXIMUM_PRESSED); i++) {
		sendKeyByKeycode(finalKeys[i], 0);
	}
	// synchronize keys
	sendSync();
}
