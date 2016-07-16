/*
 * KeyboardInterface.h
 *
 *  Created on: Jul 16, 2016
 *      Author: Nikita Wootten
 */

#ifndef KEYBOARDINTERFACE_H_
#define KEYBOARDINTERFACE_H_

int init_keyboard();
int sendKey(int key, int value);
int sendSync();
int destroy_keyboard();
int typeKey(int key);

#endif /* KEYBOARDINTERFACE_H_ */
