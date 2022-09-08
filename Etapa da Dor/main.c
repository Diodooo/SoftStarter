/* USER CODE BEGIN Header */
/**
  **************************
  * @file           : main.c
  * @brief          : Main program body
  **************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//IHM
#define TIMEOUT       1000
#define TAM_MSG       8
//IHM
//OC
#define CH1 0
//OC
//INSTRUMENTAÇÃO --> ADC1
#define BUFFER_LENGTH 1
//INSTRUMENTAÇÃO --> ADC1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//IHM
char msg[8] = "Opaida\n";
char msg2[] = "Opaida\n";
float dummy2 = 0;
uint8_t ctrl = 5;         //deve ser inicalizado com valor diferente de 1 2 3
//IHM
//OC
typedef enum {SOBE=0,DESCE}borda_t;
float pulso[2]= {8, 58};
borda_t borda;
float fase = 0.00;
float angulo = 0.00;
//OC
//TIM DE APOIO
int dummy1 = 0;
//TIM DE APOIO
//ADC --> ETAPA DE INTRSUMENTAÇÃO
uint32_t adc_volt[50] = {0};
uint32_t adc_voltCalc = 0;
float adc_voltCalcS = 0;
uint16_t adc_ad[BUFFER_LENGTH] = {0};
uint8_t  ctrlADC = 0;
//ADC --> ETAPA DE INTRSUMENTAÇÃO
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void limpaBuffer(char sujeira[7]){
	for(int i = 0; i<=7; i++){
		sujeira[i] = 0;
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_TIM3_Init();
  MX_DMA_Init();
  MX_TIM10_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  //TIM DE APOIO
  HAL_TIM_Base_Start_IT(&htim10);
  //TIM DE APOIO
  //OC
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulso[borda]);
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
  //OC
  //IHM
  HAL_UART_Transmit(&huart2, msg, strlen(msg), TIMEOUT);
  limpaBuffer(msg);
  HAL_UART_Receive_IT(&huart2, msg, 7);
  //IHM
  //ADC --> ETAPA DE INTRSUMENTAÇÃO
  HAL_ADC_Start_DMA(&hadc1, adc_ad, BUFFER_LENGTH);
  HAL_TIM_Base_Start(&htim2);
  //ADC --> ETAPA DE INTRSUMENTAÇÃO
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//PPM
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(ctrl == 0 && fase < 180){
		if(dummy1 == dummy2){
			ctrl=5;
			dummy1 = 0;
		}
		if(dummy1 < dummy2 && ctrl != 5){
			dummy1++;
			fase =  dummy1 * angulo;
			pulso[0] = 8 + (fase*10);
			pulso[1] = 58 + (fase*10);
		}
	}
	if(ctrl == 1 && fase > 0){
		if(dummy1 == dummy2){ //etapad e controle e correção de angulo
			ctrl=5;
			dummy1 = 0;
			fase = 0;
			pulso[0] = 8;
			pulso[1] = 58;
		}
		if(dummy1 < dummy2 && ctrl != 5){
			dummy1++;
			fase =  180 - (dummy1 * angulo);
			pulso[0] = (fase*10) - 8;
			pulso[1] = (fase*10) - 58;
		}
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	if(borda == SOBE){
		borda = DESCE;
	} else {
	borda = SOBE;
	}
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulso[borda]);
}
//PPM
//Instrumentação
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	adc_volt[ctrlADC] = adc_ad[0];
	ctrlADC -=-(!0); //ctrl++

	if(ctrlADC == 50){
		ctrlADC = 0;
	}

	for(uint8_t i=0;i!=50;i++){
		 adc_voltCalc = (adc_voltCalc + adc_volt[i]);
		 if(i == 49){
			adc_voltCalcS = (((adc_voltCalc/50)*3.3)/4096);
			//sprintf(msg, "Tensao:%.2f", adc_voltCalcS);  COMO QUE ENVIA ISSO SEM TRAVAR O CARALHO DO CODIGO?
			adc_voltCalc = 0;
		}
	}
}
//Instrumentação
//IHM
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(strcmp(msg, "onn0000") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		ctrl = 0;
		limpaBuffer(msg);
		sprintf(msg, "ON\n");
	}
	if(strcmp(msg, "off0000") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		ctrl = 1;
		limpaBuffer(msg);
		sprintf(msg, "OFF\n");
	}
	if(strcmp(msg, "eme0000") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		ctrl = 2;
		limpaBuffer(msg);
		sprintf(msg, "EMERG\n");
	}
	if(strstr(msg, "ram") != NULL){
		for(int j = 0; j!=3;j++){
			msg[j] = '0';
		}
		dummy2 = atoi(msg)/100;
		angulo = (175 / dummy2);
		limpaBuffer(msg);
	}
	if(strstr(msg, "rap") != NULL){
		for(int j = 0; j!=3;j++){
			msg[j] = '0';
		}
		dummy2 = atoi(msg)/1000;
		angulo = (175 / dummy2);
		limpaBuffer(msg);
	}
	//FALTA MANDAR MENSAGENS DE RAMPA DE SUBIDA, RAMPA DE DESCIDA, SOBRECORRENTE DE 150% E 200% DE CORRENTE LIMITE
	HAL_UART_Transmit(&huart2, msg, strlen(msg), TIMEOUT);
	HAL_UART_Receive_IT(&huart2, msg, 7);
}
//IHM
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
