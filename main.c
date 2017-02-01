/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "i2c.h"
#include "mma8451.h"
#include "delay.h"

#define FLASH_DELAY 10
#define ACC_SENSITIVITY 40

void Init_Debug_Signals(void) {
}

void Init_Config_Signals(void) {
		// Make 3 pins GPIO
	/*PORTE->PCR[CONFIG1_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[CONFIG1_POS] |= PORT_PCR_MUX(1);          
	PORTE->PCR[CONFIG2_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[CONFIG2_POS] |= PORT_PCR_MUX(1);          
	PORTE->PCR[CONFIG3_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[CONFIG3_POS] |= PORT_PCR_MUX(1);*/
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	int16_t prev_acc_X, prev_acc_Y, prev_acc_Z;
	int n;
	int mode;
	
	Init_RGB_LEDs();
	Init_Debug_Signals();
	Init_Config_Signals();

	Control_RGB_LEDs(1, 1, 0);								/* yellow: starting up */
	i2c_init();																/* init i2c	*/
	Delay(200);
	
	if (!init_mma()) {												/* init mma peripheral */
		Control_RGB_LEDs(1, 0, 0);							/* Light red error LED */
		while (1)																/* not able to initialize mma */
			;
	}
	Control_RGB_LEDs(0, 0, 0);							

	Delay(50);
	
	while (1) {
		Delay(50);
		prev_acc_X = acc_X;
		prev_acc_Y = acc_Y;
		prev_acc_Z = acc_Z;
		
		mode = 6;
		
		
		read_full_xyz();
		
		if ((abs(prev_acc_X - acc_X) > ACC_SENSITIVITY) || 
			(abs(prev_acc_Y - acc_Y) > ACC_SENSITIVITY) || 
			(abs(prev_acc_Z - acc_Z) > ACC_SENSITIVITY)) {
			// Flash LEDs
				for (n=0; n<2; n++) {
					Control_RGB_LEDs(1, 1, 1);
					Delay(FLASH_DELAY);
					Control_RGB_LEDs(0, 0, 0);							
					Delay(FLASH_DELAY*2);		
				}
			}
		switch(mode) {
			// Mode 1; Port B = xx0, interpreted as 110
			case 6:
				Control_RGB_LEDs(1, 1, 0); // Yellow
				Delay(FLASH_DELAY);
				break;
			// Mode 2; Port B = x0x, interpreted as 101
			case 5:
				Control_RGB_LEDs(1, 0, 1); // Magenta
				Delay(FLASH_DELAY);
				break;
			case 3:
			// Mode 3; Port B = 0xx, interpreted as 011
				Control_RGB_LEDs(0, 1, 1); // Cyan
				Delay(FLASH_DELAY);
				break;
			default:
				Control_RGB_LEDs(0, 0, 0);
				Delay(FLASH_DELAY);
				break;
		}
	}
}

