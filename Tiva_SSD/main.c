/*
 * 	Rahul Sharma
 *	4-Digit 7-Segment Display
 * 	CEDT
 */

/*
 * Header Files
 */
#include "stdint.h"
#include "stdbool.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"

/*
 * Constants
 */
#define GP_A SYSCTL_PERIPH_GPIOA
#define GP_E SYSCTL_PERIPH_GPIOE
#define GA_B GPIO_PORTA_BASE
#define GE_B GPIO_PORTE_BASE
const uint8_t ssdS[8] = {GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,	// A B C D //Voyager 17 A & B shorted
						GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_1, GPIO_PIN_0};	// E F G H
const uint8_t ssdC[4] = {GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};	// CC4 CC3 CC2 CC1 map to CC4 CC2 CC3 CC1 of voyager no 17

const unsigned int num[16] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0,	//0 1 2 3 4 5 6 7
							0xFE, 0xF6, 0xEE, 0xFF,	0x9C, 0xFD, 0x9E, 0x8E};	//8 9 A B. C D. E F
const unsigned int cc[4] = {0x1, 0x2, 0x4, 0x8};
int delay[4] = {1000,1500,2000,2500};
uint32_t ui32Period;
unsigned int count = 0x0000;
int timer_div = 2;

/*
 * Functions
 */

void gOn(int n,int state){

	if(n == 7 || n == 6)
		if(state == 1)
			GPIOPinWrite(GE_B,ssdS[n],ssdS[n]);
		else
			GPIOPinWrite(GE_B,ssdS[n],0);
	else
		if(state == 1)
			GPIOPinWrite(GA_B,ssdS[n],ssdS[n]);
		else
			GPIOPinWrite(GA_B,ssdS[n],0);

}

void segON(int seg, int ct){

	unsigned int cath = ct;
	unsigned int sgmt = seg;
	int i = 7, j = 0;

	while(i >= 0){					// 8 bits
		if(sgmt & 0x80)				// if sgmt = 0xFC (to display 0) sgmt = 11111100 & 10000000 = 1
			gOn(7-i,1);				// sgmt << 1 = 11111000 & 10000000 .....
		else						// sgmt << 1 = 00000000 & 10000000 .....
			gOn(7-i,0);
		i--;
		sgmt = sgmt << 1;
	}

	while(j < 4){					// 4 bits
		if(cath & 0x1)
			GPIOPinWrite(GE_B,ssdC[j],ssdC[j]);
		j++;
		cath = cath >> 1;
	}

}

void ssdDisp(unsigned int val){

	int i;
	int shift = 0;
	for( i = 0; i < 4; i++ ){
		segON(num[val>>shift & 0xF],cc[i]);
		shift += 4;
		SysCtlDelay(SysCtlClockGet()/delay[i]);
		GPIOPinWrite(GE_B,ssdC[i],0);
	}

}

void setGPIO(){

	SysCtlPeripheralEnable(GP_A);
	SysCtlPeripheralEnable(GP_E);
	int i;
	for( i = 0; i < 6; i++ ){
		GPIOPinTypeGPIOOutput(GA_B,ssdS[i]);
	}
	GPIOPinTypeGPIOOutput(GE_B,ssdS[6]);
	GPIOPinTypeGPIOOutput(GE_B,ssdS[7]);
	for( i = 0; i < 4; i++ ){
		GPIOPinTypeGPIOOutput(GE_B,ssdC[i]);
	}

}

void Interrupt(void){

	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);				// Clear the timer interrupt
	if(count == 0xFFFF)
		count = 0x0000;
	count++;

}

void setTimer(){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() / timer_div);				//setting timer period
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	TimerIntRegister(TIMER0_BASE, TIMER_A, Interrupt);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

}

/*
 * Main
 */

int main(void) {
	
	unsigned int any_num = 0x9999;

	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);		//Configures the clock of the controller (80Mhz)

	setGPIO();
	setTimer();

	while(1){

		ssdDisp(any_num);
	}

	return 0;
}
