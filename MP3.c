#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pwm.h"
#include "lpc17xx_dac.h"
#include "math.h"
#define PI 3.14159265

uint32_t buffer;
char data[16];
DAC_CONVERTER_CFG_Type DACCfg;
PWM_TIMERCFG_Type PWMCfg;
PWM_CAPTURECFG_Type PWMCAPCfg;
int count = 0;
int counter = 0;
double val, angle;
int data3;
int data3max;
char datadisp[50];

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

	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 23;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Funcnum = 1;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &UARTConfigStruct);
   	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &UARTFIFOConfigStruct);
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);
}

int write_usb_serial_blocking(char *buf,int length)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t *)buf,length, BLOCKING));
}


void ADC(){
	
	ADC_Init(LPC_ADC, 200000);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	int count = 1;
	while(count!=100){count+=1;}
	
}

void DAC(){
	sprintf(data, "TEST TEST no\n\r");
	write_usb_serial_blocking(data, 16);
	DAC_Init(LPC_DAC);
	DAC_SetBias(LPC_DAC, 0);
	/*	
	PWM_ConfigStructInit(PWM_MODE_TIMER, &PWMCfg);
	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &PWMCfg);
	PWM_Cmd(LPC_PWM1, ENABLE);
	PWM_ChannelConfig(LPC_PWM1, 2, PWM_CHANNEL_SINGLE_EDGE);
	PWM_ConfigCapture(LPC_PWM1, &PWMCAPCfg);
	//
	DACCfg.CNT_ENA = 1;
	DACCfg.DBLBUF_ENA = 1;
	DACCfg.DMA_ENA = 1;
	DAC_ConfigDAConverterControl(LPC_DAC, &DACCfg);
	DAC_SetDMATimeOut(LPC_DAC, 100);
	*/
	while(count!=100){count+=1;}
	
}

void mp3_task2(void){
	while(1){
		counter += 1;
		if (counter / 100 >= 2) {
			angle += 1;
			counter -= 1000;
			val = PI / 180;
			DAC_UpdateValue(LPC_DAC, sin(angle*val)*512+512);
			sprintf(data, "%f\n\r", sin(angle*val)*512+512);
			write_usb_serial_blocking(data, 16);
		}
	}
}

void DMA_IRQHandler(void){
	sprintf(data, "TEST TEST Lo\n\r");
	write_usb_serial_blocking(data, 16);
}

void Task1(void){
	uint16_t test;
	int count = 1;
	char data1[50];
	while(count!=100){count+=1;}
	int i = 0;
	while(i < 200)
	{
		test = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
		test = test << 3;
		sprintf(data1, "%16d", test);
		while(count!=100){count+=1;}
		write_usb_serial_blocking(data1, 16);
		write_usb_serial_blocking("\n\r", 4);
	}
}

void Task3(void){
	uint16_t test;
	int count = 1;
	while(count!=100){count+=1;}
	int counterDac = 0;
	while(1)
	{	
		test = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0) >> 2;
		DAC_UpdateValue(LPC_DAC, test);
	}
}

		/*
		test = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
		data3 = test << 3;
		//data3 += 512;
		data3max = data3 % 512;
		sprintf(datadisp, "%16d\n\r", data3max);
		while(count!=100){count+=1;}
		//write_usb_serial_blocking(datadisp, 18);
		data3 /= 4;
		if (data3max == 0) {
			data3 = 1;		
		}
		else {	
			data3 /= data3max;
		}
		sprintf(datadisp, "%16d\n\r", data3);
		while(count!=100){count+=1;}
		//write_usb_serial_blocking(datadisp, 18);
		data3 *= 10;
		if (data3 <= 100000000) {
			DAC_UpdateValue(LPC_DAC, data3);		
		}
		//else {
		//	DAC_UpdateValue(LPC_DAC, 1024);
		//}
		counterDac = 0;
		*/


void main(void){
	serial_init();
	DAC();
	ADC();
	Task3();
}


