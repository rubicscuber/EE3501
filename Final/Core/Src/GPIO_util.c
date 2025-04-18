/*
 * ReadColumns.c
 *
 *  Created on: Feb 17, 2025
 *      Author: nathaniel
 */

#include "main.h"
void GPIO_util_setRow(int currentRow){

	switch (currentRow){
	case 0:
		HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_SET);
		break;

	case 1:
		HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_SET);
		break;

	case 2:
		HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_SET);
		break;

	case 3:
		HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_RESET);
		break;

	}

};

int GPIO_util_readCol(int currentColumn){

	switch (currentColumn){
	case 0:
		return HAL_GPIO_ReadPin(COL1_GPIO_Port, COL1_Pin);
		break;

	case 1:
		return HAL_GPIO_ReadPin(COL2_GPIO_Port, COL2_Pin);
		break;

	case 2:
		return HAL_GPIO_ReadPin(COL3_GPIO_Port, COL3_Pin);
		break;

	case 3:
		return HAL_GPIO_ReadPin(COL4_GPIO_Port, COL4_Pin);
		break;
	}
	return 1;
};







