//DAC.c
// deals with initializing and outputing to the r-2r resistor network ladder

#include "tm4c123gh6pm.h" // port addresses
#include "ADC.h"

void DAC_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // PORTB Clock
	
	delay = SYSCTL_RCGCGPIO_R;      // allow time for clock to stabilize
	
	GPIO_PORTB_AMSEL_R &= ~0xFF;      
  GPIO_PORTB_PCTL_R &= ~0xFFFFFFFF; 
  GPIO_PORTB_DIR_R |= 0xFF;        
  GPIO_PORTB_AFSEL_R &= ~0xFF;      
  GPIO_PORTB_DEN_R |= 0xFF;     
}


void DAC_Out(unsigned char data){ unsigned long divider;
	divider = (ADC0_InSeq3() + 1)/400; //get volume control switch measurement
                                //add one to prevent divide by zero in function Output_DAC; divider is between 0-9

	if(divider < 9) // 9 is the "silence" value
		GPIO_PORTB_DATA_R = data/(divider+1); 
	
	GPIO_PORTE_DATA_R ^= 0x01; //toggle status light
}
