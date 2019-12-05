#include "helper.h"

void USART_setup() {
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	//PC4,5: alternate function (USART tx,rx)
	GPIOC->MODER |= GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1;
	GPIOC->MODER &= ~(GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0);
	//PC4,5 alternate function 1 (0b0001)
	GPIOC->AFR[0] &= ~((0xE << GPIO_AFRL_AFSEL4_Pos) | (0xE << GPIO_AFRL_AFSEL5_Pos));
	GPIOC->AFR[0] |= (0x1 << GPIO_AFRL_AFSEL4_Pos) | (0x1 << GPIO_AFRL_AFSEL5_Pos);	
	//setup USART
	USART3->CR1 &= ~(USART_CR1_UE);	//disable to allow setup
	USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200; // 115200 baud
	USART3->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;	
	
	NVIC_EnableIRQ(USART3_4_IRQn);
	
	USART3->CR1 |= USART_CR1_UE; //renable
	send_string("\nUSART running\n");
}

void send_char(char letter) {
	while (!(USART3->ISR & USART_ISR_TXE)) {
		//wait for TX to empty
	}
	USART3->TDR = letter;
}

void send_string(char* word) {
	for (int i = 0; word[i] != NULL; i++) {
		send_char(word[i]);
	}
}

void I2C_setup() {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
 	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	// PB11,13 to alternate function
	// PB14 output
	GPIOB->MODER |= GPIO_MODER_MODER11_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODER11_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_1);
	// PB11, 13 open drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_13;
	// PB14 push-pull
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_14);
	
	// PB11 AF1 (I2C2_SDA) PB13 AF5 (I2C2_SCL)
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL11 | GPIO_AFRH_AFSEL13_Pos);
	GPIOB->AFR[1] |= (0x1 << GPIO_AFRH_AFSEL11_Pos) | (0x5 << GPIO_AFRH_AFSEL13_Pos);
	
	// PB14 output high
	GPIOB->ODR |= GPIO_ODR_14;

	
	//PC0 output
	GPIOC->MODER |= GPIO_MODER_MODER0_0;
	GPIOC->MODER &= ~(GPIO_MODER_MODER0_1);
	//PC0 push-pull
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_0);
	
	//PC0 output high
	GPIOC->ODR |= GPIO_ODR_0;
	
	
	I2C2->TIMINGR &= ~(I2C_TIMINGR_PRESC | I2C_TIMINGR_SCLL | I2C_TIMINGR_SCLH | I2C_TIMINGR_SDADEL | I2C_TIMINGR_SCLDEL_Pos);
	I2C2->TIMINGR |= (0x1 << I2C_TIMINGR_PRESC_Pos) | (0x13 << I2C_TIMINGR_SCLL_Pos) | (0xF << I2C_TIMINGR_SCLH_Pos) | 
		(0x2 << I2C_TIMINGR_SDADEL_Pos) | (0x4 << I2C_TIMINGR_SCLDEL_Pos);
	
	I2C2->CR1 |= I2C_CR1_PE;
}

int I2C_write(char slave_addr, char* wdata, int bytes) {
	//I2C2 transaction setup
	I2C2->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES);
	//Slave address, #bytes: bytes
	I2C2->CR2 |= ((slave_addr << 1) | (bytes << I2C_CR2_NBYTES_Pos));
	//request write
	I2C2->CR2 &= ~(I2C_CR2_RD_WRN);
	//start
	I2C2->CR2 |= I2C_CR2_START;
	
	send_string("transmitting\n");
	
	for (int i = 0; i < bytes; i++) {
		while(!((I2C2->ISR & I2C_ISR_TXIS) || (I2C2->ISR & I2C_ISR_NACKF))) {}
		if (I2C2->ISR & I2C_ISR_NACKF) {
			send_string("NACK\n");
			return -1;
		}
		else {
			send_string("TACK\n");
		}
		
		I2C2->TXDR = wdata[i];
		send_string("sent_byte: ");
		send_char(wdata[i]);

		send_char('\n');
	}
	return 0;
}
int I2C_read(char slave_addr, char* rdata, int bytes) {
	//I2C2 transaction setup
	I2C2->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES);
	//Slave address: 0x6b, #bytes: bytes
	I2C2->CR2 |= ((slave_addr << 1) | (bytes << I2C_CR2_NBYTES_Pos));
	//request read
	I2C2->CR2 |= I2C_CR2_RD_WRN;
	//start
	I2C2->CR2 |= I2C_CR2_START;
	
	for (int i = 0; i < bytes; i++) {
		while(!((I2C2->ISR & I2C_ISR_RXNE) || (I2C2->ISR & I2C_ISR_NACKF))) {}
		if (I2C2->ISR & I2C_ISR_NACKF) {
			send_string("NACK\n");
			return -1;
		}
		else {
			send_string("RACK\n");
		}
		
		send_string("read byte\n");
		rdata[i] = I2C2->RXDR;
	}
	return 0;
}

int I2C_read_from_reg(char slave_addr, char reg, char* rdata, int bytes) {
	char wdata[1] = {reg};
	I2C_write(slave_addr, wdata, 1);
	while(!(I2C2->ISR & I2C_ISR_TC)) {}
	return I2C_read(slave_addr, rdata, bytes);
}

/*
digipot_write

res_num: resistor number, [1-10]
res_value: res_MAX * res_value/256 = resistance, [0-255]

returns: 	0 = success
					-1 = failure
*/
int digipot_write(int res_num, uint8_t res_value) {
	// Validating inputs
	if (res_num < 1 || res_num > 10) {
		send_string("Error: resistor number invalid.");
		return -1;
	}	

	
	int reg_num = (res_num - 1) % 4;
	
	uint8_t cmd;
	uint8_t addr;
	if (res_num <= 4) { 			// VCO chip digipots
		addr = 0x28;
		cmd = (1 << 4) | reg_num;
	} 
	else if (res_num <= 8) { 	// VCF chip digipots
		addr = 0x2a;
		cmd = (1 << 4) | reg_num;
	}
	else {
		addr = 0x2c;
		cmd = reg_num << 7;
	}
	
	char wdata[2] = {cmd, res_value};
	int status = I2C_write(addr, wdata, 2);
	//send_string("waiting\n");
	while(!(I2C2->ISR & I2C_ISR_TC)) {}
	return 0;
}

/*
digipot_read

res_num: resistor number, [1-10]
res_value: res_MAX * res_value/256 = resistance, [0-255]

returns: 	res_value = resistance/res_MAX * 256 [0-255]
*/
uint8_t digipot_read(int res_num) {
	/*wdata[0] = 0x30;
	wdata[1] = 0x03;
	status = I2C_write(0x28, wdata, 2);
	while(!(I2C2->ISR & I2C_ISR_TC)) {}
	char RDAC1_data[1];
	I2C_read(0x28, RDAC1_data, 1);
*/
	// Validating inputs
	if (res_num < 1 || res_num > 10) {
		send_string("Error: resistor number invalid.");
		return -1;
	}	

	
	int reg_num = (res_num - 1) % 4;
	
	uint8_t cmd;
	uint8_t addr;
	if (res_num <= 4) {
		addr = 0x28;
		cmd = (3 << 4) | reg_num;
	} 
	else if (res_num <= 8) {
		addr = 0x2a;
		cmd = (3 << 4) | reg_num;
	}
	else {
		addr = 0x2c;
		cmd = reg_num << 7;
	}
	
	int status;
	if (res_num < 8) {
		//TODO check statuses
		char wdata[2] = {cmd, 0x03}; //NOTE: 0x03 means RDAC instead of EEPROM
		status = I2C_write(addr, wdata, 2);
	}
	else {
		char wdata[1] = {cmd};
		status = I2C_write(addr, wdata, 1);
	}
	while(!(I2C2->ISR & I2C_ISR_TC)) {}
		
	char rdata[1];
	status = I2C_read(addr, rdata, 1);
	return rdata[0];
}

uint8_t char_to_hex(uint8_t letter) {
	if (letter >= 0x30 && letter <= 0x39) {
		return letter - 0x30;
	}
	else if (letter >= 0x41 && letter <= 0x46) {
		return letter - 0x41 + 0xA;
	}
	else if (letter >= 0x61 && letter <= 0x66) {
		return letter -0x61 + 0xa;
	}
	else {
		send_string("invalid character\n");
		return 0x8;
	}
}

char hex_to_char(int hex) {
	if (hex >= 0x0 && hex <= 0x9) {
		return hex + 0x30;
	}
	else if (hex <= 0xF) {
		return hex + 0x41 - 0xA;
	}
	else {
		send_string("invalid hex\n");
		return -1;
	}
}

// DAC was used in 5780 project to generate triangle wave
// Not needed for sp
void DAC_setup() {
	//DAC setup
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	//PA4 (DAC_OUT1): Analog
	GPIOA->MODER |= GPIO_MODER_MODER4_0 | GPIO_MODER_MODER4_1;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4_0 | GPIO_MODER_MODER4_1);
	//DAC1->CR &= ~(DAC_CR_TEN1);
	DAC1->CR |= DAC_CR_TSEL1;
	DAC1->CR |= DAC_CR_EN1;
	//end DAC setup
}

void TIM2_setup() {
	NVIC_EnableIRQ(TIM2_IRQn);
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//Init Timer 2
  TIM2->PSC = 79; //(8MHz/(79+1)) = 100KHz
	TIM2->ARR = 7;	//100KHz/7 = 14285.7Hz -> 446.4 Hz triangle wave
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
}

void encoder_setup() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

	// TODO
	//PC6, PC7, PC8: XOR1 XOR2 XOR3  interuptable
	//PC9, PA8, PA9: A1 A2 A3  
	//all pins to input mode
	GPIOC->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
	GPIOA->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
	
	// TODO
	//PC9, PA8, PA9 pulled up
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR9);
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR9_0;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0;
	
	// TODO
	//unmask EXTI6,7,8
	EXTI->IMR |= EXTI_IMR_IM6 | EXTI_IMR_IM7 | EXTI_IMR_IM8;
	//enable rising edge triggers
	EXTI->RTSR |= EXTI_RTSR_RT6 | EXTI_RTSR_RT7 | EXTI_RTSR_RT8;
	//enable falling edge triggers
	EXTI->FTSR |= EXTI_FTSR_FT6 | EXTI_FTSR_FT7 | EXTI_FTSR_FT8;
	
	//select interrupt source
	SYSCFG->EXTICR[1] = SYSCFG_EXTICR2_EXTI6_PC | SYSCFG_EXTICR2_EXTI7_PC;
	SYSCFG->EXTICR[2] = SYSCFG_EXTICR3_EXTI8_PC;
	
	//Enable EXTI interrupt
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}
