/*
 * DebounceKeypad.h
 *
 *  Created on: Feb 20, 2025
 *      Author: nathaniel
 */

#ifndef INC_KEYPAD_UTIL_H_
#define INC_KEYPAD_UTIL_H_

void Keypad_util_debounceKeypad(int *debounceCount, int DEBOUNCE_MAX, int i, int j, char key_map[4][4], char *inputChar, int *inputSuccess);

void Keypad_util_enterPasscode(int width, int *passwordIndex, char inputChar, char CODE[6], char inputCode[6], char clear, int *inputSuccess);

#endif /* INC_KEYPAD_UTIL_H_ */
