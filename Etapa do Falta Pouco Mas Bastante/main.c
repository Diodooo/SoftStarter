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
#define ANGULO_TOTAL_DE_VARIACAO 		151
#define TAM_MSG      					8
#define STAND_BY1 						4
#define STAND_BY2						5
#define EMERGENCIA 						2
#define DESLIGADO 						1
#define LIGADO 							0
#define BUFFER_LENGTH 					1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//------------------------IHM------------------------------//
char msg[8] = "OFF\n";
char msg2[200] = "OFF\n";
float tempoSubida = 0.00;
float tempoDescida = 0.00;
int correnteDesligamento = 1000;
float sobrecorrenteLimite = 655;
uint8_t ctrl = 6;
//------------------------IHM------------------------------//

//------------------------PPM------------------------------//
typedef enum {SOBE=0,DESCE}borda_t;
float pulso[2]= {172, 178};
borda_t borda;
float fase = 0.00;
float angulo = 0.00;
int dummy1 = ANGULO_TOTAL_DE_VARIACAO - 1;
float frequenciaSubida = 0;
float frequenciaDescida = 0;
//------------------------PPM------------------------------//

//-------------------INTRUMENTACAO-------------------------//
float adc_volt[120] = {0};
float adc_voltCalc = 0;
float adc_voltCalcS = 0;
float adc_ad[BUFFER_LENGTH] = {0};
uint8_t  ctrlADC = 0;
//-------------------INTRUMENTACAO-------------------------//
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

//FUNCAO UTILIZADA PARA LIMPAR A VARIAVEL MSG[7], ASSIM, RENOVANDO O SEU USO E PERMITINDO RECEBER OU ENVIAR VALORES NOVAMENTE
void limpaBuffer(char sujeira[7]){
	for(int i = 0; i<=7; i++){
		sujeira[i] = 0;
	}
}

//FUNCAO UTILIZADA PARA CONVERTER O VALOR EM BIT DO CONVERSOR A/D PARA SEU NÍVEL DE TENSÃO CORRESPONDENTE (TENSAO MÁXIMA = 3.3V)
float converteBitVolt(float valorBit){
	float valorConvertido = (valorBit*3.3)/4096;
	return(valorConvertido);
}

//TRANSMITE MENSAGENS VIA SERIAL E PREPARA O ARM PARA RECEBER NOVAMENTE
void transmiteDados(char dados[4]){
	sprintf(msg, dados);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
	HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
}

//EXECUTA AS MEDIDAS DE EMERGENCIA GENERICAS E PEDE PARA A FUNCAO transmiteDados ENVIAR PARA O MONITOR UMA MENSAGEM DE EMERGENCIA
void medidasDeEmergencia(char dados[4]){
	HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);
	dummy1 = ANGULO_TOTAL_DE_VARIACAO - 1;
	pulso[0] = 20 + dummy1;
	pulso[1] = 28 + dummy1;
	transmiteDados(dados);
	ctrl = STAND_BY1;
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
  //------------------------PPM------------------------------//
  HAL_TIM_Base_Start_IT(&htim10);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulso[borda]);
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
  //------------------------PPM------------------------------//
  //------------------------IHM------------------------------//
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), TIMEOUT);
  limpaBuffer(msg);
  HAL_UART_Receive_IT(&huart2, (uint8_t*)msg, 7);
  //------------------------IHM------------------------------//
  //-------------------INTRUMENTACAO-------------------------//
  HAL_ADC_Start_IT(&hadc1);
  HAL_TIM_Base_Start(&htim2);
  //-------------------INTRUMENTACAO-------------------------//
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(ctrl == 6){
			transmiteDados("OFF\n");
			ctrl = STAND_BY1;
	  }
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){     // ESTE TIM E RESPONSAVEL POR AVANCAR O PULSO DO PPM

	if(adc_voltCalcS >= correnteDesligamento){					 	// A CORRENTE DO MOTOR EXCEDEU A CORRENTE DE DESLIGAMENTO?
		medidasDeEmergencia("CDD\n");							 		// ACIONAR AS MEDIDAS DE EMERGENCIA
	}
	if(adc_voltCalcS >= sobrecorrenteLimite){					 	// A CORRENTE DO MOTOR EXEDEU A SOBRECORRENTE LIMITE
		transmiteDados("SCL\n");
		return;													 		// PARA DE AVANCAR O PULSO DO PPM
	} else {
		transmiteDados("COP\n");
	}

	if(ctrl == EMERGENCIA){											 // BOTAO DE EMERGENCIA ACIONADO?
		medidasDeEmergencia("EME\n");
	}

	if(ctrl == LIGADO){																	// O USUARIO ACIONOU O MOTOR?
		if(dummy1 <= 135 && adc_voltCalcS < 50){											// FOI PERDIDA A CONECXÃO COM A REDE?
			medidasDeEmergencia("PDE\n");
			return;
		}

		HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);											// INICIALIZA OS PULSOS DO PPM
		htim10.Instance->ARR = (FREQ_BASE_DIV_PELO_PSC/frequenciaSubida)-1;					// ALTERA A FREQUENCIA DO TIM10 PARA REMOVER 1º DO PPM A CADA CICLO POR xx SEGUNDOS
		transmiteDados("RDS\n");

		if(dummy1 < ANGULO_TOTAL_DE_VARIACAO && dummy1 > 0){								// CASO O PPM NAO TENHA PERCORRIDO 150º AINDA
			pulso[0] = 20 + dummy1;																// RECUA A SUBIDA DO PULSO
			pulso[1] = 28 + dummy1;																// RECUA A DESCIDA DO PULSO
			dummy1--;																			// SUBTRAI O ANGULO DE DISPARO
		} else {																			// ACABOU DE RECUAR O PULSO?
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);									// ATIVA O RELE DE BYPASS
			HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);											// DESATIVA OS PULSOS DO PPM
			transmiteDados("ON\n");
			ctrl = STAND_BY1;
		}
	}
	if(ctrl == DESLIGADO){																// O USUARIO DESLIGOU O MOTOR
		if(dummy1 >= 14 && adc_voltCalcS < 50){												// FOI PERDIDA A CONECXÃO COM A REDE?
			medidasDeEmergencia("PDE\n");
			return;
		}

		HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);											// INICIALIZA OS PULSOS DO PPM
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);								// DESATIVA O RELE DE BYPASS
		htim10.Instance->ARR = (FREQ_BASE_DIV_PELO_PSC/frequenciaDescida)-1;				// ALTERA A FREQUENCIA DO TIM10 PARA ENTREGAR 1 GRAU PARA O PPM A CADA CICLO POR XX SEGUNDOS
		transmiteDados("RDD\n");

		if(dummy1 >= 0 && dummy1 < ANGULO_TOTAL_DE_VARIACAO - 1){                           // ENQUANTO O ANGULO FOR DIFERENTE DO ANGULO MAXIMO (150º)
			pulso[0] = 20 + dummy1;																// AVANCA A SUBIDA DO PULSO
			pulso[1] = 28 + dummy1;																// AVANCA A DESCIDA DO PULSO
			dummy1++;																			// AUMENTA O ANGULO DE DISPARO
		} else {
			transmiteDados("OFF\n");
			ctrl = STAND_BY2;
		}
	}

	if(ctrl == STAND_BY1){										 							// MODO DE ESPERA
		if(dummy1 <= 135 && adc_voltCalcS < 50){											// FOI PERDIDA A CONEXAO COM A REDE?
			medidasDeEmergencia("PDE\n");
			return;
		}
	}

	if(ctrl == STAND_BY2){										 							// MODO DE ESPERA
		if(dummy1 >= 14 && adc_voltCalcS < 50){												// FOI PERDIDA A CONECXÃO COM A REDE?
			medidasDeEmergencia("PDE\n");
			return;
		}
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){			// RESPONSAVEL POR GERAR O PULSO DO PPM, ETAPA DESENVOLVIDA PELO PROFESSOR ZUCCO NA AULA DO DIA 11/05/2022
	if(borda == SOBE){														// SE A BORDA SUBIU "ANTES"
		borda = DESCE;															// "AGORA" DESCE
	} else {																// CASO CONTRARIO
		borda = SOBE;															// SOBE A BORDA
	}
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulso[borda]);				// ATUALIZA O VALOR DO CCRx DURANTE A EXECUCAO DO CODIGO COM BASE NA POSICAO DOS PULSOS
}
//------------------------PPM------------------------------//
//-------------------Instrumentação------------------------//
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){							// CONVERSOR A/D
	adc_volt[ctrlADC] =  pow(converteBitVolt(HAL_ADC_GetValue(&hadc1)),2);			// RECEBE O VALOR DO CONVERSOR, TRADUZ PARA VOLTS, ELEVA AO QUADRADO E GUARDA O RESULTADO EM UMA LISTA (adc_volt)
	ctrlADC++;																		// AVANCA O "SLOT"/INDICE DA LISTA

	if(adc_voltCalcS >= correnteDesligamento){										// A CORRENTE EXCEDEU O LIMITE?
		medidasDeEmergencia("CDD\n");
	}

	if(ctrlADC == 120){																// FOI REGISTRADO 120 VALORES NA LISTA?
		for(uint8_t i=0;i!=120;i++){													// SOMA TODOS OS 120 VALORES
		 adc_voltCalc = (adc_voltCalc + adc_volt[i]);
			 if(i == 119){
				 adc_voltCalcS = (sqrt(adc_voltCalc/120)*1000)/1.238;					// CALCULA O VALOR RMS COM BASE NA MEDIA DELES
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
/* ------------------------------------------------------- //
 * A IHM NESTE PROJETO TEM UM FUNCIONAMENTO PECULIAR QUE VOU LISTAR AQUI
 *
 * 	ELA FUNCIONA POR MEIO DO RECEBIMENTO E ENVIO DE MENSAGENS PROTOCOLADAS
 * 	RECEPCAO (7 BITS):
 * 		3 BITS = CONJUNTO DE LETRAS QUE INDICAM A FINALIDADE DA MENSAGEM
 * 		4 BITS = CONJUNTO DE NUMEROS QUE PODEM OU NAO CONTER UMA INFORMACAO
 * 			EX1: "onn0000" --> "onn" (LIGAR O MOTOR), "0000" (SEM INFORMACAO QUANTITATIVA)
 * 			EX2: "rap5000" --> "rap" (RAMPA DE SUBIDA COM TEMPO MENOR QUE 10 SEGUNDOS), "5000" (LIGAR O MOTOR EM 5 SEGUNDOS)
 * 	TRANSMISSAO (4BITS):
 * 		2~3 BITS = CONJUNTO DE LETRAS QUE INDICAM A FINALIDADE DA MENSAGEM, DIFERENTE DO ANTERIOR NAO TEM UM VALOR ENGESSADO, PODENDO VARIAR DE 2 A 3 LETRAS POR MENSAGEM
 * 		ULTIMO BIT = "\n" (SEM UTILIZAR O \n O CUBE MONITOR NAO TERMINA O PROCESSAMENTO DA MENSAGEM, ASSIM, GERANDO UM OVERFLOW NA UART)
 * ------------------------------------------------------- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(strcmp(msg, "onn0000") == 0){										// MENSAGEM DE INICIALIZACAO DO MOTOR
		ctrl = LIGADO;															// SOLICITA PARA O CODIGO REALIZAR A ROTINA DE INICIALIZACAO DO MOTOR
		limpaBuffer(msg);														// LIMPA A MENSAGEM
	}
	if(strcmp(msg, "off0000") == 0){										// MENSAGEM DE DESLIGAMENTO DO MOTOR
		ctrl = DESLIGADO;														// SOLICITA PARA O CODIGO REALIZAR A ROTINA DE DESLIGAMENTO DO MOTOR
		limpaBuffer(msg);
	}
	if(strcmp(msg, "eme0000") == 0){										// MENSAGEM DE MODO DE EMERGENCIA
		ctrl = EMERGENCIA;														// SOLICITA PARA O CODIGO REALIZAR A ROTINA EMERGENCIAL
		limpaBuffer(msg);
	}
	if(strstr(msg, "ram") != NULL){											// MENSAGEM CONTENDO O VALOR DA RAMPA DE SUBIDA PARA TEMPOS MAIORES QUE 10 SEGUNDOS
		for(int j = 0; j!=3;j++){												// REMOVE O CABECALHO DA MENSAGEM
			msg[j] = '0';
		}
		tempoSubida = atoi(msg)/100;											// TRANSFORMA O RESTANTE EM VARIAVEL TIPO INT E AJUSTA O VALOR REMOVENDO OS DIGITOS DESNECESSARIOS
		frequenciaSubida = 1/(tempoSubida/ANGULO_TOTAL_DE_VARIACAO);			// COM BASE NO ANGULO TOTAL DE VARAICAO E O TEMPO REQUISITADO PELO USUSARIO, CALCULAMOS A FREQUENCIA DE SUBIDA DA RAMPA A FIM DE OBTER O INCREMENTO DE 1 GRAU POR ESTOURO DE TIMER
		limpaBuffer(msg);
	}
	if(strstr(msg, "rap") != NULL){											// MENSAGEM CONTENDO O VALOR DA RAMPA DE SUBIDA PARA TEMPOS MENORES QUE 10 SEGUNDOS
		for(int j = 0; j!=3;j++){												// REMOVE O CABECALHO DA MENSAGEM
			msg[j] = '0';
		}
		tempoSubida = atoi(msg)/1000;											// TRANSFORMA O RESTANTE EM VARIAVEL TIPO INT E AJUSTA O VALOR REMOVENDO OS DIGITOS DESNECESSARIOS
		frequenciaSubida = 1/(tempoSubida/ANGULO_TOTAL_DE_VARIACAO);			// COM BASE NO ANGULO TOTAL DE VARAICAO E O TEMPO REQUISITADO PELO USUSARIO, CALCULAMOS A FREQUENCIA DE SUBIDA DA RAMPA A FIM DE OBTER O INCREMENTO DE 1 GRAU POR ESTOURO DE TIMER
		limpaBuffer(msg);
	}
	if(strstr(msg, "rad") != NULL){											// MENSAGEM CONTENDO O VALOR DA RAMPA DE SUBIDA PARA TEMPOS MAIORES QUE 10 SEGUNDOS
		for(int j = 0; j!=3;j++){											// O RESTANTE DO "IF" SEGUE IDENTICO A RAMPA DE SUBIDA
			msg[j] = '0';
		}
		tempoDescida = atoi(msg)/100;
		frequenciaDescida = 1/(tempoDescida/ANGULO_TOTAL_DE_VARIACAO);
		limpaBuffer(msg);
	}
	if(strstr(msg, "rpd") != NULL){											// MENSAGEM CONTENDO O VALOR DA RAMPA DE SUBIDA PARA TEMPOS MENORES QUE 10 SEGUNDOS
		for(int j = 0; j!=3;j++){											// O RESTANTE DO "IF" SEGUE IDENTICO A RAMPA DE SUBIDA
			msg[j] = '0';
		}
		tempoDescida = atoi(msg)/1000;
		frequenciaDescida = 1/(tempoDescida/ANGULO_TOTAL_DE_VARIACAO);
		limpaBuffer(msg);
	}
	if(strstr(msg, "cdm") != NULL){											// CORRENTE DE DESLIGAMENTO PARA VALORES ACIMA DE 1000mA
		for(int j = 0; j!=3;j++){												// REMOVE O CABECALHO
			msg[j] = '0';
		}
		correnteDesligamento = atoi(msg);										// ATUALIZA O VALOR DA CORRENTE DE DESLIGAMENTO
		limpaBuffer(msg);
	}
	if(strstr(msg, "cdc") != NULL){											// CORRENTE DE DESLIGAMENTO PARA VALORES ABAIXO DE 1000mA
		for(int j = 0; j!=3;j++){												// REMOVE O CABECALHO
			msg[j] = '0';
		}
		correnteDesligamento = atoi(msg)/10;									// ATUALIZA O VALOR DA CORRENTE DE DESLIGAMENTO
		limpaBuffer(msg);
	}
	if(strstr(msg, "scc") != NULL){											// MENSAGEM CONTENDO OS VALORES DE SOBRECORRENTE LIMITE
		for(int j = 0; j!=3;j++){
			msg[j] = '0';
		}
		sobrecorrenteLimite = atoi(msg)/10;									// ATUALIZA O VALOR DA SOBRECORRENTE LIMITE
		limpaBuffer(msg);
	}
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
