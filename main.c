/*
 * FALL 2013 Embedded Systems Competition
 * UT IEEECS
 * 9/21/13 - 9/28/13
 * Di Xie
 * Jesus Zarzar
 *
 * This system will play an instrument depending on the MODE
 * This system has three modes: TONE, TRUMPET
 * The PIANO mode will allow the user to play our 12-key PIANO
 *
 * ADC pins used: ADC0 - volume control switch (PE3) 
 * 8-bit DAC pins used: (MSB) PB7-PB0 (LSB)
 * 12 Piano key pins used (one octave of notes)
 * Volume Control Switch on PE3: Will adjust the volume that comes out of the speaker
 * Heartbeat is enabled: This is the yellow LED in the circuit diagram
 *
 * 
 */

#include "PLL.h"
#include "tm4c123gh6pm.h" // port addresses
#include "SysTickInts.h"
#include "Timer0A.h"
#include "ADC.h"
#include "Piano.h"
#include "mode.h"
#include "DAC.h"


// the different modes on our system
#define MIC 0
#define PIANO 1

//Global Variables
extern unsigned char CURRENT_MODE; // mode MIC on startup
#define Coefficients_Sum 429 // this is the sum of the values in Convoluted_Coefficients[]

void EnableInterrupts(void);
void DisableInterrupts(void);

int main(void){	
  /* General initialization */
  DisableInterrupts();
  PLL_Init();    // 50 MHz
	//Timer0A_Init(); // just in case we need another timer triggered interrupt
	Switch_Init();
	Piano_Init(); // Initalizes Piano Key interface, relevant LEDs, and Heartbeat
	ADC0_InitSWTriggerSeq3(0); // Initializes Volume Control Switch
//	ADC1_InitSWTriggerSeq3(9); // Initialize analog sound input: on startup, we will use mode MIC
	DAC_Init(); // Initalize SSI for outputing to DAC

  GPIO_PORTC_DATA_R |= 0x10; //initially turn on MIC LED
	while(1){

	
				Note_Select();


			if(GPIO_PORTF_DATA_R & 0x10) // check if user changed mode
				Switch();
			
	
	}
}
