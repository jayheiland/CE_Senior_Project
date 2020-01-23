#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx.h"

//---------------USART------------------------
/*
USART_setup
initializes the USART 3 peripheral.
Must be ran before any USART based functions will work

PC4: TX
PC5: RX
*/
void USART_setup(void);
/*
send_char
sends a character over USART3.
USART_setup() must be called first.

letter: the character to be sent.
*/
void send_char(char letter);
/*
send_string
sends a string over USART3.
USART_setup() must be called first.

word: the string to be sent.
*/
void send_string(char* word);

//------------------I2C------------------------
/*
I2C_setup

Initializes the I2C2 peripheral
Must be run before I2C based functions
PB11: SDA
PB13: SCL
PB14: controls the slave address when in I2C mode (no need for external connection)
PC0: connected to the SPI/I2C mode select pin (no need for external connection)
*/
void I2C_setup(void);
/*
I2C_write

Attempt to write to an I2C device.

slave_addr: address of the external device
wdata: array of bytes to send. The first byte will usually be a register address or command
bytes: number of bytes to send

returns: 
	0 on success
	nonzero (-1) on error
*/
int I2C_write(char slave_addr, char* wdata, int bytes);
/*
I2C_read
Attempt to read from an I2C device.
This function assumes the slave is already trying to send data

slave_addr: address of slave device
rdata: array that will hold read data.
	NOTE: must have at least $bytes pre-allocated
bytes: number of bytes to read
*/
int I2C_read(char slave_addr, char* rdata, int bytes);
/*
I2C_read_from_reg

Attempt to read from data from a specific register on an I2C slave device
This function consists of a single byte write (to request the register),
followed by a $bytes byte read.

slave_addr: address of the slave
reg: register address to read from
rdata: array that will hold read data.
	NOTE: must have at least $bytes pre-allocated
bytes: number of bytes to read
*/
int I2C_read_from_reg(char slave_addr, char reg, char* rdata, int bytes);

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


//---------------------------ENCODER------------------------
/*
encoder_setup

Initializes the GPIO pins and EXTI interrupts used by the rotary encoders

PC6, PC7, PC8: XOR1 XOR2 XOR3
PC9, PA8, PA9: A1 A2 A3
*/
void encoder_setup(void);




/*
char_to_hex

letter: the alpha-numeric character to be converted to ASCII

returns: the ASCII representation of letter
*/
uint8_t char_to_hex(uint8_t letter);

/*
hex_to_char

hex: an ASCII representation of an alpha-numeric character

returns: the alpha-numeric character represented by the ascii code
*/
char hex_to_char(int hex);




#endif /* HELPER_H_ */
