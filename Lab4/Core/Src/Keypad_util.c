/*
 * DebounceKaypad.c
 *
 *  Created on: Feb 20, 2025
 *      Author: nathaniel
 */
#include "stdio.h"
#include "main.h"
#include "string.h"

void Keypad_util_debounceKeypad(int *debounceCount, int DEBOUNCE_MAX, int i, int j, char key_map[4][4], char *inputChar, int *inputSuccess){

	static int row;
	static int col;
	static int tempRow;
	static int tempCol;

	tempRow = i;
	tempCol = j;

	(*debounceCount) ++;

	if ((*debounceCount >= DEBOUNCE_MAX) && (row == tempRow) && (col == tempCol)){
		*inputChar = key_map[row][col];
		printf("%c\n", *inputChar);
		*debounceCount = 0;
		*inputSuccess = 1;
	 }
	else{
		*inputSuccess = 0;
	}
	row = tempRow;
	col = tempCol;
	tempRow = 0;
	tempCol = 0;

};

void Keypad_util_enterPasscode(int width, int *passwordIndex, char inputChar, char CODE[6], char inputCode[6], char clear, int *inputSuccess){

	if ((*passwordIndex <= width) && (*inputSuccess == 1)){
		if (inputChar == '*'){
			strcpy(inputCode, "00000");
			(*passwordIndex) = 0;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		}
		else if ((*passwordIndex == 5)){
			(*passwordIndex) = 0;
		}
		else{
			inputCode[*passwordIndex] = inputChar;
			(*passwordIndex)++;
		}
	}
	*inputSuccess = 0;

};
