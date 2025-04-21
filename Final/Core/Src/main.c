/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <GPIO_util.h>
#include "lcd4.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern void initialise_monitor_handles(void);

//clock switching variable toggled by the hardware interrupt
volatile int swTime = 0;

//global clock variables
int hour;
int minute;
int second;
char ampm[3] = "PM";


//global char key_map
char key_map [4][4] = {
		  {'1', '2', '3', 'A'},
		  {'4', '5', '6', 'B'},
		  {'7', '8', '9', 'C'},
		  {'*', '0', '#', 'D'}
		  };

//function declarations
void increment_clock ();
char find_key();
void set_time();

//simple clock incrementing function that handles clock-like overflows
void increment_clock(){
	second ++;
	if (second == 60){
		second = 0;
		minute ++;
		if(minute == 60){
			minute = 0;
			hour ++;
			if (hour == 12 && minute ==0){
				//change the string setting
				if (strcmp(ampm, "AM") == 0){
					strcpy(ampm, "PM");
				}
				else{
					strcpy(ampm, "AM");
				}
			}
			if(hour > 12){
				hour = 1;
			}
		}
	}
	return;
}

//Custom GPIO_util.c file contains definitions
char find_key(){
    //this flag will be set once a keypress is registered
    int pressed_flag = 0;
    for (int row = 0; row < 4; row++) {
        GPIO_util_setRow(row);
    	HAL_Delay(100);
        for (int col = 0; col < 4; col++) {
            //Checks if the current pin is high. If so, the button
            //corresponding to the current collumn and row is pressed
        	if (GPIO_util_readCol(col) == 0){
                pressed_flag = 1;
                //loop here until the readCol value goes back to 1
                //ie: the key has been lifted
                while (pressed_flag == 1) {
                    if (GPIO_util_readCol(col) == 1) {
                        pressed_flag = 0;
                    }
                }
                //Return the char associated with the keypress
                return key_map[row][col];
            }
        }
    }
    //Returns null character if no key is pressed when function was called in the superloop;
    return '\0';
}

//This funciton relies heavily on while loops to keep the control flow from leaving
void set_time(){

	int input_flag = 1;
	char currentChar = '\0';
	int input = 0;

	char inputBuffer[30];
	char displayBuffer[30];

	lcdCommand(lcdClear);
	strcpy(inputBuffer, "");
	lcdString("Hour: "); //---------------------------------------------------Hour while loop
	while(input_flag == 1){
		//load a key press into the char
		currentChar = find_key();

		//'#' is the enter key
		if (currentChar == '#') {
		//if the input integer is within range exit the loop
			if (input <= 12 && input >= 1) {
				hour = input;
				input_flag = 0;
			}
		//error checking statement
			else {
				lcdCommand(lcdClear);
				lcdString("ERROR");

				HAL_Delay(500);
				strcpy(inputBuffer, ""); //clear the inputBuffer and int
				input = 0;

				lcdCommand(lcdClear);
				lcdString("Hour: ");

			}
		}
		//any other character is picked up with this statement
		else if (currentChar != '\0') {
			strcpy(displayBuffer, "");
			strcpy(displayBuffer, "Hour: ");//clear then load display buffer

			strncat(inputBuffer, &currentChar, 1);//cat the input string

			strcat(displayBuffer, inputBuffer); //cat onto display Buffer

			lcdCommand(lcdClear);

			lcdString(displayBuffer);
		    input = (input * 10) + (currentChar - '0');
		}
	}

	//reset all variables for next loop
	input_flag = 1;
	currentChar = '\0';
	input = 0;

    lcdCommand(lcdClear);
    strcpy(inputBuffer, "");
    lcdString("Minute: "); //-----------------------------------------------Minute while loop
    while (input_flag) {
		currentChar = find_key();

	    if (currentChar == '#') {
	        if (input <= 59 && input >= 0) {
	        	minute = input;
	            input_flag = 0;
	        }
	        //If input is invalid, prints "ERROR" and resets everything
	        else {
				lcdCommand(lcdClear);
				lcdString("ERROR");

				HAL_Delay(500);
				strcpy(inputBuffer, ""); //clear the inputBuffer and int
				input = 0;

				lcdCommand(lcdClear);
				lcdString("Minute: ");
	        }
	    }
	    else if (currentChar != '\0') {
			strcpy(displayBuffer, "");
			strcpy(displayBuffer, "Minute: ");//clear then load display buffer

			strncat(inputBuffer, &currentChar, 1);//cat the input string

			strcat(displayBuffer, inputBuffer); //cat onto display Buffer

			lcdCommand(lcdClear);

			lcdString(displayBuffer);
		    input = (input * 10) + (currentChar - '0');
		}
	}

	//reset all variables for next loop
	input_flag = 1;
	currentChar = '\0';
	input = 0;


    lcdCommand(lcdClear);
    strcpy(inputBuffer, "");
	lcdString("AM(1) or PM(2):");//--------------------------------------set AM/PM while loop
	while (input_flag) {
		currentChar = find_key();

		if (currentChar == '#') {
			//If input is 1 set AM
			if (input == 1) {
				strcpy(ampm, "AM");
				input_flag = 0;
			}
			//If input is 2 set PM
			else if (input == 2) {
				strcpy(ampm ,"PM");
				input_flag = 0;
			}
			else {
                lcdCommand(lcdClear);
				lcdString("ERROR");

				HAL_Delay(500);
				strcpy(inputBuffer, ""); //clear the inputBuffer and int
				input = 0;

				lcdCommand(lcdClear);
				lcdString("AM(1) or PM(2):");
			}
		}
        //again, any other character is caught with this statement
		else if (currentChar != '\0') {
			strcpy(displayBuffer, "");
			strcpy(displayBuffer, "AM(1) or PM(2):");//clear then load display buffer

			strncat(inputBuffer, &currentChar, 1);//cat the input string

			strcat(displayBuffer, inputBuffer); //cat onto display Buffer

			lcdCommand(lcdClear);

			lcdString(displayBuffer);

			input = currentChar - '0'; //placeholder math for thie input number conversion
		}

	}
    //second is defaulted to 0
	second = 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  initialise_monitor_handles();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim4);
  lcdSetup(GPIOA, LCD_RS_Pin,
  		   GPIOA, LCD_E_Pin,
  		   GPIOA, LCD_DB4_Pin,
  		   GPIOB, LCD_DB5_Pin,
  		   GPIOC, LCD_DB6_Pin,
  		   GPIOC, LCD_DB7_Pin);
  lcdInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  char inputChar;
  char buffer[30];
  uint16_t readValue;
  float Centegrade;
  int IntCentegrade;

  hour = 11;
  minute = 59;
  second = 55;

  //////////////////////superloop////////////////////////
  while (1)
  {
	  inputChar = find_key();

      //ADC code block to grab the current value and
      //convert to a readable temperature
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	  readValue = HAL_ADC_GetValue(&hadc1);
	  Centegrade = (readValue * 5 / 41) - 55;
	  IntCentegrade = Centegrade;

	  if (swTime > 1){
		  increment_clock();
		  lcdCommand(lcdClear);
		  sprintf(buffer,"%02d:%02d:%02d %s %02dC", hour, minute, second, ampm, IntCentegrade);

		  lcdString(buffer);
		  printf("%s\n", buffer);

		  swTime = 0;
	  }

        //break off into the set_time subroutine and disable the global interrupt before heading there
	  if (inputChar == '*'){
		  HAL_NVIC_DisableIRQ(TIM4_IRQn);
		  set_time();
		  HAL_NVIC_EnableIRQ(TIM4_IRQn);
	  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 15999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LCD_DB7_Pin|LCD_DB6_Pin|ROW4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RS_Pin|LCD_E_Pin|LCD_DB4_Pin|LD2_Pin
                          |ROW1_Pin|ROW2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_DB5_Pin|ROW3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_DB7_Pin LCD_DB6_Pin ROW4_Pin */
  GPIO_InitStruct.Pin = LCD_DB7_Pin|LCD_DB6_Pin|ROW4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RS_Pin LCD_E_Pin LCD_DB4_Pin LD2_Pin
                           ROW1_Pin ROW2_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_E_Pin|LCD_DB4_Pin|LD2_Pin
                          |ROW1_Pin|ROW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_DB5_Pin ROW3_Pin */
  GPIO_InitStruct.Pin = LCD_DB5_Pin|ROW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : COL3_Pin COL4_Pin */
  GPIO_InitStruct.Pin = COL3_Pin|COL4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : COL2_Pin COL1_Pin */
  GPIO_InitStruct.Pin = COL2_Pin|COL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim == &htim4){
		//toggle the global int switch to 1 (reset in the main loop)
		swTime ++;
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
