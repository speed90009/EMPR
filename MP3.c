#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "math.h"

uint32_t buffer;
char output[16];
DAC_CONVERTER_CFG_Type DACCfg;

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
	
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &UARTConfigStruct);
   	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &UARTFIFOConfigStruct);
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);
}

int write_usb_serial_blocking(char *buf,int length)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t *)bruf,length, BLOCKING));
}


void ADC(){
	
	ADC_Init(LPC_ADC, 200000);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	int count = 1;
	while(count!=100){count+=1;}
	
}

void DAC(){
	
	DAC_Init(LPC_DAC);
	DAC_ConfigDAConverterControl(LPC_DAC, DACCfg);
	DACCfg.CNT_ENA = 1;
	DACCfg.DBLBUF_ENA = 1;
	DACCFG.DMA_ENA = 1;
	DAC_SetDMATimeout(LPC_DAC, 1000);
	while(count!=100){count+=1;}
	
}

void mp3_task2(int amp, double frq){
	if (counter / 100 >= 10) {
		counter -= 1000;
		DAC(		
	}
}


void main(void){
	ADC();
	uint16_t test;
	int count = 1;
	char data[50];
	while(count!=100){count+=1;}
	serial_init();
	int i = 0;
	while(i < 200)
	{
		test = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
		test = test << 3;
		sprintf(data, "%16d", test);
		while(count!=100){count+=1;}
		write_usb_serial_blocking(data, 16);
		write_usb_serial_blocking("\n\r", 4);
	}
}


