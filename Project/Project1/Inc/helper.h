#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx.h"

void USART_setup(void);
void send_char(char letter);
void send_string(char* word);

void I2C_setup(void);
int I2C_write(char slave_addr, char* wdata, int bytes);
int I2C_read(char slave_addr, char* rdata, int bytes);
int I2C_read_from_reg(char slave_addr, char reg, char* rdata, int bytes);

void DAC_setup(void);
void TIM2_setup(void);
void encoder_setup(void);


/*
digipot_write

res_num: resistor number, [1-10]
resistance: res_MAX * res_value/256 = resistance (ohms), [0-255]

returns: 	0 = success
					-1 = failure
*/
int digipot_write(int res_num, uint8_t res_value);

/*
digipot_read

res_num: resistor number, [1-10]
res_value: res_MAX * res_value/256 = resistance, [0-255]

returns: 	res_value = resistance/res_MAX * 256 [0-255]
*/
uint8_t digipot_read(int res_num);

uint8_t char_to_hex(uint8_t letter);

char hex_to_char(int hex);




#endif /* HELPER_H_ */
