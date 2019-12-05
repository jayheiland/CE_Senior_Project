/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "helper.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
const uint8_t triangle_table[32] = {0,15,31,47,63,79,95,111,127,142,158,174,
190,206,222,238,254,238,222,206,190,174,158,142,127,111,95,79,63,47,31,15};
const uint8_t sine_table[32] = {127,151,175,197,216,232,244,251,254,251,244,232,216,
197,175,151,127,102,78,56,37,21,9,2,0,2,9,21,37,56,78,102};
/* USER CODE END 0 */
static volatile char USART_INPUT;
static volatile char USART_READY = 0;

// TODO tune these to sine wave config
static volatile uint8_t res_1_val = 0xA0;
static volatile uint8_t res_5_val = 0x50;
static volatile uint8_t res_6_val = 0x00;
static volatile int ENCODER_FLAG = 0;

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

  /* Configure the system clock */
  SystemClock_Config();

 	I2C_setup();
	
	USART_setup();
	
	DAC_setup();
	TIM2_setup();
	encoder_setup();
	
	// quad 100k digipot 1 addr: 0101000 = 0x28 (matched to Vlogic)
	// RDACy = 000100xx = 0x1y
		
	send_string("setup complete\n");

	// digipot basic test
	// write initial values
	//digipot_write(1, res_1_val);
	digipot_write(5, res_5_val);
	digipot_write(6, res_6_val);
		
	send_string("wrote 0x80 to RDAC1\n");
		
	//uint8_t value_1 = digipot_read(1);
	uint8_t value_5 = digipot_read(5);
	uint8_t value_6 = digipot_read(6);
		
	if (value_5 == res_5_val && value_6 == res_6_val) { // value_1 == res_1_val &&
		send_string("Self test passed\n");
	}
	else {
		send_string("Self test failed\n");
	}

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {		
    //Digipot control loop
		send_string("\nResistor?\n");

		while(USART_READY == 0) {}
		USART_READY = 0;
		
		char res_num_char = USART_INPUT;
		send_char(res_num_char);
		if (res_num_char == 'r' || res_num_char == 'R') {
			send_string("res_1_val: ");
			send_char(hex_to_char((res_1_val & 0xF0) >> 4));
			send_char(hex_to_char(res_1_val & 0xF));
			send_char('\n');
			send_string("res_5_val: ");
			send_char(hex_to_char(res_5_val >> 4));
			send_char(hex_to_char(res_5_val & 0xF));
			send_char('\n');
			send_string("res_6_val: ");
			send_char(hex_to_char(res_6_val >> 4));
			send_char(hex_to_char(res_6_val & 0xF));
			send_char('\n');
		}
		else {
			uint8_t res_num = char_to_hex(res_num_char);
			
			send_string("\nValue MSN?\n");

			while(USART_READY == 0) {}
			USART_READY = 0;
			
			char res_value_MSN = USART_INPUT;
			send_char(res_value_MSN);
			send_char('\n');
				
			send_string("\nValue LSN?\n");

			while(USART_READY == 0) {}
			USART_READY = 0;
			
			char res_value_LSN = USART_INPUT;
			send_char(res_value_LSN);
			send_char('\n');
			
			uint8_t res_value = (char_to_hex(res_value_MSN) << 4) | char_to_hex(res_value_LSN);
				
			digipot_write(res_num, res_value);
			switch (res_num) {
				case 1:
					res_1_val = res_value;
					break;
				case 5:
					res_5_val = res_value;
				case 6:
					res_6_val = res_value;
					break;
			}
		}
		if (ENCODER_FLAG) {
			send_string("updating");
			//digipot_write(1, res_1_val);
			digipot_write(5, res_5_val);
			digipot_write(6, res_6_val);
			ENCODER_FLAG = 0;
		}
	}
}

void TIM2_IRQHandler(void)
{
	static volatile int i;
	DAC1->DHR8R1 = triangle_table[i];
	i++;
	if (i > 32) {
		i = 0;
	}
	TIM2->SR &= ~(TIM_SR_UIF);
}

void USART3_4_IRQHandler(void)
{
	USART_INPUT = USART3->RDR;
	//send_char(USART_INPUT);
	USART_READY = 1;
}

// Quadrature Encoder Matrix
const int QEM [16] = {0, -1, 1, 2, 1, 0, 2, -1, -1, 2, 0, 1, 2, 1, -1, 0}; 
static volatile int old_enc_1 = 0;
static volatile int old_enc_2 = 0;
static volatile int old_enc_5 = 0;
static volatile int old_enc_6 = 0;

void EXTI4_15_IRQHandler(void) 
{
	if (EXTI->PR & EXTI_PR_PIF6) {
		int enc_1_A = GPIOC->IDR & GPIO_IDR_9;
		int enc_1_B = enc_1_A ^ (GPIOC->IDR & GPIO_IDR_6);
		int new_enc_1 = (enc_1_A << 1) | enc_1_B;	
		int res_1_change = QEM[(old_enc_1 * 4) + new_enc_1];
		
		if (res_1_val == 0x0 && res_1_change < 0) { }
		else if (res_1_val == 0xFF && res_1_change > 0) { }
		else if (res_1_change == 2) {send_string("enc_1_error\n");}
		else {
			res_1_val += res_1_change;
		}
		
		//digipot_write(1, res_1_val);
		old_enc_1 = new_enc_1;
				
		//Clear the interrupt flag to indicate it has been handled
		EXTI->PR |= EXTI_PR_PIF6;
	}
	else if (EXTI->PR & EXTI_PR_PIF7) {
		int enc_5_A = GPIOA->IDR & GPIO_IDR_8;
		int enc_5_B = enc_5_A ^ (GPIOC->IDR & GPIO_IDR_7);
		int new_enc_5 = (enc_5_A << 1) | enc_5_B;	
		int res_5_change = QEM[(old_enc_5 * 4) + new_enc_5];
		
		if (res_5_val == 0x0 && res_5_change < 0) { }
		else if (res_5_val == 0xFF && res_5_change > 0) { }
		else if (res_5_change == 2) {send_string("enc_5_error\n");}
		else {
			res_5_val += res_5_change;
		}
		
		//digipot_write(5, res_5_val);
		old_enc_5 = new_enc_5;
		
		//Clear the interrupt flag to indicate it has been handled
		EXTI->PR |= EXTI_PR_PIF7;
	}
	else if (EXTI->PR & EXTI_PR_PIF8) {
		int enc_6_A = GPIOA->IDR & GPIO_IDR_9;
		int enc_6_B = enc_6_A ^ (GPIOC->IDR & GPIO_IDR_8);
		int new_enc_6 = (enc_6_A << 1) | enc_6_B;	
		int res_6_change = QEM[(old_enc_6 * 4) + new_enc_6];
		
		if (res_6_val == 0x0 && res_6_change < 0) { }
		else if (res_6_val == 0xFF && res_6_change > 0) { }
		else if (res_6_change == 2) {send_string("enc_6_error\n");}

		else {
			res_6_val += res_6_change;
		}
		
		//digipot_write(6, res_6_val);
		old_enc_6 = new_enc_6;
		
		//Clear the interrupt flag to indicate it has been handled
		EXTI->PR |= EXTI_PR_PIF8;
	}
	send_char('y');
	ENCODER_FLAG = 1;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
