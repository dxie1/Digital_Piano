/* mode.c 
 * Choose between MIC and PIANO modes and alternate when button pressing if PF4 is pressed, also handles button bouncing
 */

#include "ADC.h"
#include "Timer0A.h"

// user button connected to PF4 (increment counter on falling edge)

#define NVIC_EN0_INT30          0x40000000  // Interrupt 30 enable
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))  // IRQ 0 to 31 Set Enable Register
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))  // IRQ 28 to 31 Priority Register
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_IS_R         (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R        (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R        (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R         (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_PDR_R        (*((volatile unsigned long *)0x40025514))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

// delay function for testing from sysctl.c
// which delays 3*ulCount cycles
__asm void
Delay(unsigned long ulCount) {
    subs    r0, #1
    bne     Delay
    bx      lr
}


// the different modes on our system
#define TONE 0
#define OBOE 1
#define BASSOON 2
#define modeStatus       (*((volatile unsigned long *)0x400061C0))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void Switch_Init(void){
  DisableInterrupts();
                                // (a) activate clock for port F
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	//PF4 input
  GPIO_PORTF_AMSEL_R &= ~0x10;      
  GPIO_PORTF_PCTL_R &= ~0x000F0000; 
  GPIO_PORTF_DIR_R &= ~0x10;        
  GPIO_PORTF_AFSEL_R &= ~0x10;      
  GPIO_PORTF_DEN_R |= 0x10; 
}

unsigned char CURRENT_MODE = BASSOON; // MODE IS MIC on start

void Switch(void){ unsigned long i;
  DisableInterrupts(); 

	
	if(CURRENT_MODE == OBOE){
		CURRENT_MODE = BASSOON;
		modeStatus = 0x10; //LED
	}else if(CURRENT_MODE == TONE){
		CURRENT_MODE = OBOE;
		modeStatus = 0x40; //LED
	}else if(CURRENT_MODE == BASSOON){
		CURRENT_MODE = TONE;
		modeStatus = 0x20; //LED
	} 
		
	for(i=0;i<1000;i++) // prevent button-bouncing
		Delay(7000);
}
