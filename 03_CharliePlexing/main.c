/*
 * Objective : To learn Charlie Plexing with help of
 * 			   CharliePLexed Leds On Voyager
 * Connections : TIVA PORTA Pin P6 P4 P3 P2 mapped to P4 P3 P2 P1 on Voyager
 * Author(s) : Rahul Sharma
 * CEDT
 */

/*
 * Header Files
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

/*
 * CONSTANTS AND DEFINES
 */
#define CP_SYSPERIPH SYSCTL_PERIPH_GPIOA		//Setting Sys Periph. PORTA
#define CP_BASE GPIO_PORTA_BASE					//Selecting PORTA
const uint8_t CP_LED[] = {GPIO_INT_PIN_2,GPIO_INT_PIN_3,GPIO_INT_PIN_4,GPIO_INT_PIN_6}; //Selecting GPIO pins of PORTA

int temp,k;
const int LED[] = {2210,2021,2120,221,1220,2012,2102,212,1202,122,1022,2201}; //Led On Cond. Array
void setIO(int pin_mode,int pin_no);		//Func. to set I/O for GPIO pin
void ledOn(int led_no);						//Func. to turn specific LED

/*
 * MAIN FUNCTION
 */

int main(void) {
	
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);		//Configure the clock settings of the controller
	SysCtlPeripheralEnable(CP_SYSPERIPH);										//Enable the Peripherals

	int div_t = 10;
	int flag = 0;
	while(1){
		int i;
		for(i=1;i<13;i++){
			ledOn(i);
			SysCtlDelay(SysCtlClockGet()/div_t);
			if(flag == 1){
				div_t--;
			}
			else
				div_t++;
			if(div_t==1210)
				flag = 1;
			if(div_t==10)
				flag = 0;
		}
	}
	return 0;
}

/*
 * Function Definations
 */

void ledOn(int led_no){
	int i;
	temp = LED[led_no-1];
	for(i=0 ; i<4 ; i++){
		k = temp%10;
		setIO(k,i);
		temp = temp/10;
	}
}

void setIO(int pin_mode,int pin_no){
	switch(pin_mode){
	case 0 :	GPIOPinTypeGPIOOutput(CP_BASE,CP_LED[pin_no]);			//Case 0 to set Output 0
				GPIOPinWrite(CP_BASE,CP_LED[pin_no],0);
				break;
	case 1 :	GPIOPinTypeGPIOOutput(CP_BASE,CP_LED[pin_no]);			//Case 1 to set Output 1
				GPIOPinWrite(CP_BASE,CP_LED[pin_no],CP_LED[pin_no]);
				break;
	case 2 :	GPIOPinTypeGPIOInput(CP_BASE,CP_LED[pin_no]);			//Case 2 to set High Z (input)
				//GPIOPinWrite(CP_BASE|CP1_BASE,CP_LED[pin_no]);
				break;
	default : 	break;
	}
}
