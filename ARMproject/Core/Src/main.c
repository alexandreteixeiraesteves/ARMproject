/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
typedef struct {
	char magic[8];
	char idx;
	char data[256];
} data_store_t;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim10;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

  char uart_buf[50];
  int uart_buf_len;
  uint16_t timer_val;
  uint16_t nb_push_second = 0; //nombre de seconde entre appuie et relachement 
  size_t state = 0;
  
  uint8_t buffer;
  
  static const data_store_t datastore __attribute__((__section__(".datastore"))) = {    // mémoire de la carte | pas flash 
	.idx = 10,
	.magic = "M4GIKNB",
	.data = "alexandre"
  };
  
  size_t mdp_max_size = 10;
  size_t mdp_size = 0;
  uint8_t mdp[10];
  
  size_t mdp_try_size = 0;
  uint8_t mdp_try[10];
  

  
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM10_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	if(state == 0){
		HAL_UART_Transmit(&huart2, "Received : ", 11, 10);
		if(buffer == '\r')
		{
			HAL_UART_Transmit(&huart2, "End of MDP ! There is \' ", 24, 100);
			HAL_UART_Transmit(&huart2, mdp, mdp_size, 100);
			HAL_UART_Transmit(&huart2, " \'\r\n", 3, 100);
			state = 1;
		}	
		else
			if (mdp_size >= mdp_max_size){
				HAL_UART_Transmit(&huart2, "Your MDP is too long, rewrite it please with max 10 caracters\r\n", 63, 100);
				mdp_size = 0;
			}else{
				HAL_UART_Transmit(&huart2, &buffer, 1, 10);
				HAL_UART_Transmit(&huart2, "\r\n", 2, 100);
				mdp[mdp_size++] = buffer;
			}		
	}
	else if(state == 1){
		HAL_UART_Transmit(&huart2, "[TRY] Received : ", 17, 10);
		if(buffer == '\r')
		{
			HAL_UART_Transmit(&huart2, "End of TRY ! \r\n", 15, 100);
			if((mdp_try_size == mdp_size) && (strncmp(mdp_try,mdp_try,mdp_size)==0)){
				state = 2;
				HAL_UART_Transmit(&huart2, "Deverouillage Done ! \r\n", 23, 100);
			} else {
				HAL_UART_Transmit(&huart2, "Deverouillage Failed ! \r\n", 25, 100);
			}
			
		}	
		else{
			if (mdp_try_size >= mdp_max_size){
				HAL_UART_Transmit(&huart2, "Your MDP is too long, rewrite it please with max 10 caracters\r\n", 63, 100);
				mdp_try_size = 0;
			}else{
				HAL_UART_Transmit(&huart2, &buffer, 1, 10);
				HAL_UART_Transmit(&huart2, "\r\n", 2, 100);
				mdp_try[mdp_try_size++] = buffer;
			}
		}
	}
	
	
}

/*
	~~~   Note Utile ~~~
	
	Pour écrire un retour à la ligne il faut un \r\n 
	Pour capter un retour à la ligne il faut capté \r
*/


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
  MX_TIM10_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // Start timer
  HAL_TIM_Base_Start(&htim10);
  
  //TODO décommenter pour voir élément dans la mémoire 
  //HAL_UART_Transmit(&huart2, &datastore, sizeof(datastore), 200);
  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    
    
    //HAL_UART_Transmit(&huart2, &buffer, 10, 10);
    
    
    switch( state )
		   {
		      case 0:   // state 0 : carte non configuré
		        // TODO initialisation de la carte ? 
		        
		        HAL_UART_Receive_DMA(&huart2, &buffer, 1);
		        
		        // si carte pas initialisé clé de chiffrement &mot de passe vide sinon clé et mot de passe en flash
		        // clé et mot de passe en flash utile que mise hors tenssion de la carte.
		        //  commande d'initialisation mot de passe  "./mon-script init "mon-mot-de-passe"
		        //	tant que clé et mots de passe sont NULL alors on est à létat 0 sinon on passe état 1
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    			 HAL_Delay(100);
    			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    			 HAL_Delay(100);
    			 break;
                     case 1:   // state 1 : carte vérouillé
                     	 // pour dévérouillé on appuie et on a 30s pour écrire le mot de passe 
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
                        HAL_Delay(1000);
                        nb_push_second++;
                        break;
                     case 2:   // state 3 : carte dévérouillé
                     	 //une fois la carte dévérouiller elle peux effectuer le chiffrement d'un fichier qui lui est envoyé
                     	 
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    			 HAL_Delay(1000);
    			 nb_push_second++;
    			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    			 HAL_Delay(1000);
    			 nb_push_second++;
    			 break;
                     default:
                     	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                     	 break;
                  }
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 16 - 1;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 65536 - 1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	int tmp;
	if( GPIO_Pin == GPIO_PIN_13 ) {
		tmp = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
		if (tmp == 0) // on appuie
		{
		   nb_push_second = 0;
		} 
		else		// onrelache
		{
		   switch( state )
		   {
		      case 0:
                        HAL_UART_Transmit(&huart2,"0 --> 1\r\n",11, 100);
                        state = 1;
                        break;
                     case 1:
                     	 if(nb_push_second < 1){   	// appuie cours de moins 1s
                     	    				//TODO accepte dévérouillage pendant 30s (si dévérouillage state = 2, sinon state = 1
                     	    state = 2;
                     	    HAL_UART_Transmit(&huart2,"1 --> 2\r\n", 11,100);
                     	    HAL_UART_Receive_DMA(&huart2, &buffer, 1); // Lecture du mot de pass //TODO 30s max et teste si ok
                     	 }
                     	 else
                     	 	HAL_UART_Transmit(&huart2,"1 --> 1\r\n", 11,100);
                        break;
                     case 2:
                        if(nb_push_second > 5){  	// appuie de plus de 5s reset la carte 
                        	state = 0;
                        	HAL_UART_Transmit(&huart2,"2 --> 0\r\n", 11,100);
                        }
                        else {			// appuie de plus de moins de 1s vérouillage de la carte  
                        	state = 1;
                        	HAL_UART_Transmit(&huart2,"2 --> 1\r\n", 11,100);
                        }
                        break;
                     default:
			 break;                     	
                  }
		}
		
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
