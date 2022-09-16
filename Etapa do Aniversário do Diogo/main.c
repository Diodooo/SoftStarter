/* USER CODE BEGIN Header */
/**
  ****
  * @file           : main.c
  * @brief          : Main program body
  ****
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ****
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
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
#define FREQ_BASE_DIV_PELO_PSC 			10000
#define TIMEOUT       					1000
#define VALOR_RESISTOR 					470
#define ANGULO_TOTAL_DE_VARIACAO 		150
#define TAM_MSG      					8
#define STAND_BY 						5
#define EMERGENCIA 						2
#define DESLIGADO 						1
#define LIGADO 							0
#define BUFFER_LENGTH 					1
#define CH1 							0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//IHM
char msg[8] = "OFF\n";
char msg2[200] = "OFF\n";
float tempoSubida = 0.00;
float tempoDescida = 0.00;
uint8_t ctrl = 4;         //deve ser inicalizado com valor diferente de 1 2 3
//IHM
//OC
typedef enum {SOBE=0,DESCE}borda_t;
float pulso[2]= {160, 166};
borda_t borda;
float fase = 0.00;
float angulo = 0.00;
//OC
//TIM DE APOIO
int dummy1 = ANGULO_TOTAL_DE_VARIACAO - 1;
float frequenciaSubida = 0;
float frequenciaDescida = 0;
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

float converteBitVolt(float valorBit){
	float valorConvertido = (valorBit*3.3)/4096;
	return(valorConvertido);
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
  MX_TIM10_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  //TIM DE APOIO
  HAL_TIM_Base_Start_IT(&htim10);
  //TIM DE APOIO
  //OC
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulso[borda]);
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
  //OC
  //IHM
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
  limpaBuffer(msg);
  HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
  //IHM
  //ADC --> ETAPA DE INTRSUMENTAÇÃO
  HAL_ADC_Start_IT(&hadc1);
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
//------------------------PPM------------------------------//
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(ctrl == LIGADO){
		if(dummy1 == ANGULO_TOTAL_DE_VARIACAO - 1){
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
			htim10.Instance->ARR = (FREQ_BASE_DIV_PELO_PSC/frequenciaSubida)-1;
			sprintf(msg, "RDS\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
			HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
		}

		if(dummy1 < ANGULO_TOTAL_DE_VARIACAO && dummy1 > 0){
			pulso[0] = 20 + dummy1;
			pulso[1] = 28 + dummy1;
			dummy1--;
		} else {
			/*anguloAnterior[0] = 2;
			anguloAnterior[1] = 9;*/
			ctrl = STAND_BY;
			sprintf(msg, "ON\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
			HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
		}
	}
	if(ctrl == DESLIGADO){
		if(dummy1 == 0){
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
			htim10.Instance->ARR = (FREQ_BASE_DIV_PELO_PSC/frequenciaDescida)-1;
			sprintf(msg, "RDD\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
			HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
		}
		if(dummy1 >= 0 && dummy1 < ANGULO_TOTAL_DE_VARIACAO - 1){
			pulso[0] = 20 + dummy1;
			pulso[1] = 28 + dummy1;
			dummy1++;
		} else {
			/*anguloAnterior[0] = 160;
			anguloAnterior[1] = 168;*/
			sprintf(msg, "OFF\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
			HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
			ctrl = STAND_BY;
		}
	}
	if(ctrl == EMERGENCIA){
		HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);
		dummy1 = ANGULO_TOTAL_DE_VARIACAO - 1;
		pulso[0] = 20 + dummy1;
		pulso[1] = 28 + dummy1;
	}

	if(ctrl == STAND_BY){
		return;
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
//------------------------PPM------------------------------//
//-------------------Instrumentação------------------------//
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
    adc_volt[ctrlADC] =  HAL_ADC_GetValue(&hadc1);
    ctrlADC++;

    if(ctrlADC == 50){
        for(uint8_t i=0;i!=50;i++){
         adc_voltCalc = (adc_voltCalc + adc_volt[i]);
             if(i == 49){
                 adc_voltCalcS = (converteBitVolt(adc_voltCalc/50)/VALOR_RESISTOR)*1000;
                 adc_voltCalc = 0;
             }
        }
        sprintf(msg2, "%.2fmA\n", adc_voltCalcS);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg2, strlen(msg2), TIMEOUT);
        HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
        ctrlADC = 0;
    }
}
//-------------------Instrumentação------------------------//
//------------------------IHM------------------------------//
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(strcmp(msg, "onn0000") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		ctrl = LIGADO;
		limpaBuffer(msg);
	}
	if(strcmp(msg, "off0000") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		ctrl = DESLIGADO;
		limpaBuffer(msg);
	}
	if(strcmp(msg, "eme0000") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		ctrl = EMERGENCIA;
		limpaBuffer(msg);
		sprintf(msg, "EMERG\n");
	}
	if(strstr(msg, "ram") != NULL){
		for(int j = 0; j!=3;j++){
			msg[j] = '0';
		}
		tempoSubida = atoi(msg)/100;
		frequenciaSubida = 1/(tempoSubida/ANGULO_TOTAL_DE_VARIACAO);
		limpaBuffer(msg);
	}
	if(strstr(msg, "rap") != NULL){
		for(int j = 0; j!=3;j++){
			msg[j] = '0';
		}
		tempoSubida = atoi(msg)/1000;
		frequenciaSubida = 1/(tempoSubida/ANGULO_TOTAL_DE_VARIACAO);
		limpaBuffer(msg);
	}
	if(strstr(msg, "rad") != NULL){
		for(int j = 0; j!=3;j++){
			msg[j] = '0';
		}
		tempoDescida = atoi(msg)/100;
		frequenciaDescida = 1/(tempoDescida/ANGULO_TOTAL_DE_VARIACAO);
		limpaBuffer(msg);
	}
	if(strstr(msg, "rpd") != NULL){
		for(int j = 0; j!=3;j++){
			msg[j] = '0';
		}
		tempoDescida = atoi(msg)/1000;
		frequenciaDescida = 1/(tempoDescida/ANGULO_TOTAL_DE_VARIACAO);
		limpaBuffer(msg);
	}
	//FALTA MANDAR MENSAGENS DE RAMPA DE SUBIDA, RAMPA DE DESCIDA, SOBRECORRENTE DE 150% E 200% DE CORRENTE LIMITE
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
	HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
}
//------------------------IHM------------------------------//
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
