#include "main.h"
#include "stdio.h"
#include "string.h"

TIM_HandleTypeDef htim4;
UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART2_UART_Init(void);

//delay function in microseconds
void delay (uint16_t time)
	{
		__HAL_TIM_SET_COUNTER(&htim4, 0);
		while ((__HAL_TIM_GET_COUNTER(&htim4))<time);
	}

void debug_print(const char *msg)
{   HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);}

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

float Temperature = 0;
float Humidity = 0;
char uartData[50];
uint8_t Presence = 0;

//function to set the pins as outputs
void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	}

//function to set pin as input
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL; //can be changed to PULLUP if no data is received from the pin
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	}

//Define the pin and the port for DHT11 Sensor
#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_4

//Function to send the start signal
void DHT11_Start (void)
	{
		Set_Pin_Output (DHT11_PORT, DHT11_PIN); //set the dht pin as output
		//HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 1); //initialize with data pin high
		//HAL_Delay(1000); //wait for 1000 milliseconds
		HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0); //pull the pin low
		delay(18000); //wait 18 milliseconds
		HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1); //pull the pin high
		delay(20); //wait for 20 microseconds
		Set_Pin_Input(DHT11_PORT, DHT11_PIN); //set the pin as input
	}

//dh11 function to check response
uint8_t DHT11_Check_Response (void)
	{
		uint8_t Response = 0;
		delay(40); 										//first wait for 40 microseconds
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) //check for pin to be low
		{	delay(80); 									//wait for 80 microseconds
			if((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
			{	Response = 1; 							//check if pin is high and return 1 to show sensor is present
				debug_print("\nResponse detected");
			}
			else
			{	Response = -1;							//255
				debug_print("\nResponse not detected");
			}
		}
		while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) //wait for the pin to go low again
		{  debug_print("\nPin is still high");
			HAL_Delay(1000);
		}

		char response_str[20];
		sprintf(response_str, "\nResponse: %d\n", Response);
		debug_print(response_str);

		return Response;
	}

//function to read data from dht11 signal pin
uint8_t DHT11_Read (void)
	{
		uint8_t i, j;
		for (j=0;j<8;j++)
		{
			while(!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))); //wait for the pin to change to high
			delay(40); //wait for 40 microseconds
			if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) //if the pin is low
			{
				i&= ~(1<<(7-j)); //write 0
			}
			else i|= (1<<(7-j)); //if the pin is high write 1
			while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))); //wait for the pin to go low
		}
		return i;
	}

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();

  HAL_TIM_Base_Start(&htim4);
  /* USER CODE BEGIN WHILE */
  while (1)
  {	//sprintf(uartData, "\r\nTemp (C) =\t %.1f\r\nHumidity (%%) =\t %.1f%%\r\n", Temperature, Humidity);
	//HAL_UART_Transmit(&huart2, (uint8_t *)uartData, strlen(uartData), 10);
	//HAL_Delay(500);
	DHT11_Start();
	debug_print("\nDht11 started");
	//debug_print("\nResponse ");
	Presence = DHT11_Check_Response(); //record the response from the sensor

	//Five bytes of data
	Rh_byte1 = DHT11_Read ();
	Rh_byte2 = DHT11_Read ();
	Temp_byte1 = DHT11_Read ();
	Temp_byte2 = DHT11_Read ();
	SUM = DHT11_Read ();

	TEMP = Temp_byte1;
	RH = Rh_byte1;

	Temperature = (float) TEMP;
	Humidity = (float) RH;

	debug_print("\nTemperature ");
	char temp_str[20];
	sprintf(temp_str, "%.1f", Temperature);
	debug_print(temp_str);

	//sprintf(uartData, "\r\nTemp (C) =\t %.1f\r\nHumidity (%%) =\t %.1f%%\r\n", Temperature, Humidity);
	//HAL_UART_Transmit(&huart2, (uint8_t *)uartData, strlen(uartData), 10);
	HAL_Delay(1200);
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

static void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 72-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0xffff-1;
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
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
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
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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
