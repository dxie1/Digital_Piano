// Piano.c
// Deals with the PIANO mode

// 12 key Piano: PD2 PD3 PD6 PE1 PE2 PF1 PF2 PA7 PA2 PA3 PA4 PA5 PA6        
// Note:        lowC  D   E   F   G   A   B  hiC  C#  D#  F#  G#  A#   

// Heartbeat: PE0
// Mode lights: PC4, PC5, PC6

#include "tm4c123gh6pm.h"
#include "SysTickInts.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

//possible values for notes: a small s refers to a sharp
enum letters {lowC, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B, hiC, OFF};

//reload values for Systick corresponding to letters
unsigned short Systick_Rates[] = {1493,1409,1330,1256,1185,1119,1056,997,941,888,838,791,747}; 

void Piano_Init(void){ volatile unsigned long delay;
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // PORTA Clock
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // PORTE Clock
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // PORTD Clock
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC; // PORTC Clock	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // PORTF Clock

	delay = SYSCTL_RCGCGPIO_R;      // allow time for clock to stabilize
	delay = SYSCTL_RCGCGPIO_R;      // allow time for clock to stabilize


		// inputs PA7-PA2
  GPIO_PORTA_AMSEL_R &= ~0xFC;      
  GPIO_PORTA_PCTL_R &= ~0xFFFFFF00; 
  GPIO_PORTA_DIR_R &= ~0xFC;        
  GPIO_PORTA_AFSEL_R &= ~0xFC;      
  GPIO_PORTA_DEN_R |= 0xFC; 
	
		
		// inputs PE1, PE2 ; output PE0
	GPIO_PORTE_AMSEL_R &= ~0x07;      
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; 
  GPIO_PORTE_DIR_R |= 0x01;   // PE0 output
	GPIO_PORTE_DIR_R &= ~0x06;  // PE1, PE2 inputs
  GPIO_PORTE_AFSEL_R &= ~0x07;      
  GPIO_PORTE_DEN_R |= 0x07; 
	
	// outputs PC4, PC5, PC6 
	GPIO_PORTC_AMSEL_R &= ~0x70;      
  GPIO_PORTC_PCTL_R &= ~0x0FFF0000; 
  GPIO_PORTC_DIR_R |= 0x70;   //  outputs
  GPIO_PORTC_AFSEL_R &= ~0x70;      
  GPIO_PORTC_DEN_R |= 0x70; 

	// inputs PD6, PD3, PD2
	GPIO_PORTD_AMSEL_R &= ~0x4C;      
  GPIO_PORTD_PCTL_R &= ~0x0F00FF00; 
  GPIO_PORTD_DIR_R &= ~0xFC;        
  GPIO_PORTD_AFSEL_R &= ~0xFC;      
  GPIO_PORTD_DEN_R |= 0xFC; 
	
	// inputs PF1, PF2
	GPIO_PORTF_AMSEL_R &= ~0x06;      
  GPIO_PORTF_PCTL_R &= ~0x00000FF0; 
	GPIO_PORTF_DIR_R &= ~0x06;  // PF1, PF2 inputs
  GPIO_PORTF_AFSEL_R &= ~0x06;      
  GPIO_PORTF_DEN_R |= 0x06;



}

enum letters NOTE = OFF;
enum letters PREV = OFF;
								
void Note_Select(void){ 

		while(1){
		PREV = NOTE;
		NOTE = OFF; // asumme silent until button pressed


		if(GPIO_PORTD_DATA_R & 0x04)
			NOTE = lowC;
		else if(GPIO_PORTD_DATA_R & 0x08)
			NOTE = D;
		else if(GPIO_PORTD_DATA_R & 0x40)
			NOTE = E;
		else if(GPIO_PORTE_DATA_R & 0x02)
			NOTE = F;		
		else if(GPIO_PORTE_DATA_R & 0x04)
			NOTE = G;		
		else if(GPIO_PORTF_DATA_R & 0x02)
			NOTE = A;		
		else if(GPIO_PORTF_DATA_R & 0x04)
			NOTE = B;		
		else if(GPIO_PORTA_DATA_R & 0x80)
			NOTE = hiC;
		
		//black keys
		else if(GPIO_PORTA_DATA_R & 0x04)
			NOTE = Cs;
		else if(GPIO_PORTA_DATA_R & 0x08)
			NOTE = Ds;	
		else if(GPIO_PORTA_DATA_R & 0x10)
			NOTE = Fs;
		else if(GPIO_PORTA_DATA_R & 0x20)
			NOTE = Gs;
		else if(GPIO_PORTA_DATA_R & 0x40)
			NOTE = As;
			
		
		if(NOTE != OFF && NOTE != PREV) // only need to reinit if not the same note
			SysTick_Init(Systick_Rates[NOTE]);
		else if(NOTE == OFF)
				break;
	}
			// no button pressed
			DisableInterrupts();
			GPIO_PORTE_DATA_R &= ~0x01; // Hearbeat off
		
	
}



