Triangle-to-Sine Converter

The triangle-to-sine converter is an analog synth circuit that expects a triangle wave input and modifies the wave to a sine wave via three different controls: Shape, Bias, and Top/Bottom Symmetry. The amount that each control modifies the triangle wave depends on an associated potentiometer. The potentiometers are digipots, and their value can be set either by an associated rotary encoder or by typing commands into a PuTTy terminal. 

The PuTTy terminal requires a baud rate of 115200 to communicate via USART (serial). 

Input:
Triangle wave that oscillates between 0-5V. This will be supplied by the DAC from the STM32FO.

Output:
Modified triangle wave. Connect the output signal to a speaker to hear it. 

Modifying the Triangle Wave:
The synth circuit uses two 100 Kohm digipots (AD5144ABRUZ100) and one 1 Mohm digipot (AD5242BRUZ1M). The AD5144ABRUZ100 chip has 4 potentiometers, and the AD5242BRUZ1M has two. Our program allows for the implentation of two AD5144ABRUZ100 chips and one AD5242BRUZ1M chip--in other words, the program assumes that there are eight 100 Kohm potentiometers (numbered 1-8) and two 1 Mohm potentiometers (numbered 9-10). 
The mapping of potentiometers to controls for the triangle-to-sine converter is as follows:

1 - Sine Shape
2 - Top/Btm Symmetry
9 - Sine Bias

The STM32FO communicates to the digital potentiometers--both reading their current values and setting their values--via I2C. Each potentiometer has one corresponding rotary encoder that can change its value. Potentiometers' values can also be changed via commands typed in the PuTTy terminal. The first command specifies the resistor whose value is to be changed, and involves one input to the terminal. The second command specifies the value to set the resistor to, and involves two inputs to the terminal.
Command 1: Resistor number. 
	Terminal Input 1: number between 1-10
Command 2: Resistor value. Hexidecimal between 00 and FF, which represents the proportion of the full value of the potentiometer.
	Terminal Input 2: (number between 0-9) OR (character between a-f)
	Terminal Input 3: (number between 0-9) OR (character between a-f) 

Some sample inputs to the terminal and their meanings are detailed below:
	1	// Change the value of resistor 1 (modify the sine shape of the input wave)
	8	
	0 	// Set the resistor value to half its full value: 50 Kohm. (80 is half of FF)

	9	// Change the value of resistor 9 (modify the sine bias of the input wave)
	F	
	F	// Set the resistor to its full value: 1 Mohm. 

PuTTy also accepts one other command: typing the character 'r' to the terminal results in the current values of potentiometers 1, 2, and 9 to be displayed to the terminal. 

Any other input to the terminal will result in an error message and the terminal will wait for further commands. 

We wrote our program so that it can be easily generalized to any digipot- and rotary encoder-based circuit; its functionality will be much broader than only the triangle-to-sine converter we built. 


Authors:
Dirk Lamb
Michelle Simmons

