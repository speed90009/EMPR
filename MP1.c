#include "lpc17xx_gpio.h"		// Central include files
#include "lpc17xx_systick.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include <stdio.h>
#include <stdlib.h>
#include "lpc_types.h"
#include "led.h"			// Local files


uint32_t tick;
int tick_1s;
int state;
int counts;
char counter[4] = "";

int write_usb_serial_blocking(char *buf,int length);

void alllightson (void)
{
	GPIO_SetDir(1,0x00B40000,1);
	GPIO_SetValue(1,0x00B40000);
}

void alllightsoff (void)
{
	GPIO_SetDir(1,0x00B40000,1);
	GPIO_ClearValue(1,0x00B40000);
}

void firston (void)
{
	GPIO_SetValue(1,0x00040000);
}

void secondon (void)
{
	GPIO_SetDir(1,0x00100000,1);
	GPIO_SetValue(1,0x00100000);
}

void thirdon (void)
{
	GPIO_SetDir(1,0x00200000,1);
	GPIO_SetValue(1,0x00200000);
}

void forthon (void)
{
	GPIO_SetDir(1,0x00800000,1);
	GPIO_SetValue(1,0x00800000);
}

unsigned int int_to_int(unsigned int k) {
    return (k == 0 || k == 1 ? k : ((k % 2) + 10 * int_to_int(k / 2)));
}

void task1 (void)
{
	int count = 1;
	alllightson();
	while(count!=10000000){count+=1;}
	alllightsoff();
	count = 1;
	firston();
	while(count!=10000000){count+=1;}
	alllightsoff();
	count = 1;
	secondon();
	while(count!=10000000){count+=1;}
	alllightsoff();
	count = 1;
	thirdon();
	while(count!=10000000){count+=1;}
	alllightsoff();
	count = 1;
	forthon();
	while(count!=10000000){count+=1;}
	alllightsoff();
}

void task2 (void)
{
	int item;
	int led1 [8] = {1,3,5,7,9,11,13,15};
	int led2 [8] = {2,3,6,7,10,11,14,15};
	int led3 [8] = {4,5,6,7,12,13,14,15};
	int led4 [8] = {8,9,10,11,12,13,14,15};
	counts += 1;
	for (item = 0; item<8; item++)
	{
		if (led1[item] == counts)
		{
			firston();
		}
		if (led2[item] == counts)
		{
			secondon();
		}
		if (led3[item] == counts)
		{
			thirdon();
		}
		if (led4[item] == counts)
		{
			forthon();
		}
	}
}

void task3 (void)
{
	//SYSTICK_InternalInit(0xEA60);
	SYSTICK_InternalInit(10);
	//SYSTICK_ExternalInit(10000000, 600);
	SYSTICK_Cmd(ENABLE);
	SYSTICK_IntCmd(ENABLE);
}

void SysTick_Handler(void)
{
	tick++;
	if (tick % 100 == 0) {
		tick_1s = 1;
	}
	if (tick_1s == 1) {
		alllightsoff();
		if (counts == 15)
		{
		write_usb_serial_blocking("Finished count\n\r", 16);
		exit(0);
		}
		else {
		task2();
		sprintf(counter, "%d %x %d\n\r", counts, counts, int_to_int(counts));
		write_usb_serial_blocking(counter, 16);
		}
		//if (state == 0) {
			//alllightson();
			//state = 1;
		//}
		//else {
			//alllightsoff();
			//state = 0;
		//}
	tick_1s = 0;
	}	

}

void serial_init(void)
{
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;

	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);
	
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &UARTConfigStruct);
    UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &UARTFIFOConfigStruct);
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);
}

int read_usb_serial_none_blocking(char *buf,int length)
{
	return(UART_Receive((LPC_UART_TypeDef *)LPC_UART0, (uint8_t *)buf, length, NONE_BLOCKING));
}

int write_usb_serial_blocking(char *buf,int length)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t *)buf,length, BLOCKING));
}

void task4(void)
{
	serial_init();
	write_usb_serial_blocking("Hello World\n", 12);
}

void MP1_Demonstrations(void)
{
	serial_init();
	write_usb_serial_blocking("Starting Count\n\r", 16);
	task3();
}

int main (void) 
{	
	MP1_Demonstrations();
	tick_1s = 0;
	state = 0;
	tick = 0;
	return 0;
}
