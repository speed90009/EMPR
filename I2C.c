#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"
uint8_t setHigh[1] = {0x0F};
uint8_t setLow[1] = {0xF0};
I2C_M_SETUP_Type TransferCfg;
char currentstatus[20] = "";
int count;

void serial_init(void)
{
	//I2C
	PINSEL_CFG_Type PinCfg;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 3;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	//EINT3
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 23;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0,0x00800000,0);
	GPIO_IntCmd(0,0x00800000,0);
}

void EINT3_IRQHandler(void) {
	GPIO_SetDir(1,0x00B40000,1);
	GPIO_SetValue(1,0x00B40000);
	TransferCfg.sl_addr7bit = 0x21;
	TransferCfg.tx_data = NULL;
	TransferCfg.tx_length = 0;
	TransferCfg.rx_data = setLow;
	TransferCfg.rx_length = 1;
	I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
	
	while(count!=10000000){count+=1;}
	GPIO_SetDir(1,0x00B40000,1);
	GPIO_ClearValue(1,0x00B40000);
}

int main (void) {
	serial_init();
	NVIC_EnableIRQ(EINT3_IRQn);
	TransferCfg.sl_addr7bit = 0x21;
	TransferCfg.tx_data = NULL;
	TransferCfg.tx_length = 0;
	TransferCfg.rx_data = setHigh;
	TransferCfg.rx_length = 1;
	I2C_Init(LPC_I2C1, 100000);
	I2C_Cmd(LPC_I2C1, ENABLE);
	I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
	if (NVIC_GetPendingIRQ(EINT3_IRQn) == 1) {
		GPIO_SetDir(1,0x00B40000,1);
		GPIO_SetValue(1,0x00B40000);
	}
	while(1) {
	}
}
