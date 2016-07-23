/*
 * KeyboardInterface.h
 *
 *  Created on: Jul 16, 2016
 *      Author: Nikita Wootten
 */

#ifndef KEYBOARDINTERFACE_H_
#define KEYBOARDINTERFACE_H_

void initKeyboard();
int sendKeyByKeycode(int key, int value);
int sendSync();
void destroyKeyboard();
void typeKey(int key);

#endif /* KEYBOARDINTERFACE_H_ */
