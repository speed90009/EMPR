#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"

Status DataAttempt;
char currentstatus[20] = "";
I2C_M_SETUP_Type TransferCfg;
I2C_M_SETUP_Type RecieveCfg;
char response[12] = "";
int no_devices = 0;
int addinc = 0;
uint8_t data[1] = {0x00};
uint8_t clearscreen[2] = {0x40,0xA0};
uint8_t resetscreen[2] = {0x00,0x02};
uint8_t keydata[1];
char keyoutput[8] = "";
int count; 
int counter;
uint8_t setHigh[1] = {0x0F};
uint8_t setLow[1] = {0xF0};
int colNo = 0;
uint8_t calc[16] = {0xB1,0xB2,0xB3,0xAB,0xB4,0xB5,0xB6,0xAD,0xB7,0xB8,0xB9,0xAA,0xA0,0xB0,0xBD,0xAF};
int rpointer = -1;

void serial_init(void)
{
	//Enable Ext Interupt
	NVIC_EnableIRQ(EINT3_IRQn);	

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
	GPIO_IntCmd(0,0x00800000,1);
}


unsigned int int_to_int(unsigned int k) {
    return (k == 0 || k == 1 ? k : ((k % 2) + 10 * int_to_int(k / 2)));
}

void I2CTypeSetup (void) {
	TransferCfg.sl_addr7bit = addinc;
	TransferCfg.tx_data = data;
	TransferCfg.tx_length = 1;
	TransferCfg.rx_data = NULL;
	TransferCfg.rx_length = 0;
	TransferCfg.retransmissions_max = 3;
}

void I2CScreenSetup (void) {
	addinc = 0x3B;
	uint8_t screendata[12] = {0x00,0x34,0x0c,0x06,0x35,0x04,0x10,0x42,0x9f,0x34,0x02,0x01};
	TransferCfg.sl_addr7bit = addinc;
	TransferCfg.tx_data = screendata;
	TransferCfg.tx_length = 12;
	I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
}

void I2CSend (uint8_t* inputdata, uint32_t inputsize) {
	TransferCfg.sl_addr7bit = addinc;
	TransferCfg.tx_data = inputdata;
	TransferCfg.tx_length = inputsize;
	TransferCfg.rx_data = NULL;
	TransferCfg.rx_length = 0;
	TransferCfg.retransmissions_max = 3;
	I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
}

void I2CRecieve (uint8_t* outputdata, int outputsize) {
	RecieveCfg.sl_addr7bit = 0x21;
	RecieveCfg.tx_data = NULL;
	RecieveCfg.tx_length = 0;
	RecieveCfg.rx_data = outputdata;
	RecieveCfg.rx_length = outputsize;
	I2C_MasterTransferData(LPC_I2C1, &RecieveCfg, I2C_TRANSFER_POLLING);
}

void I2CScreenClear (void) {
	addinc = 0x3B;
	TransferCfg.sl_addr7bit = addinc;
	for (counter=0; counter<80; counter++) {
		TransferCfg.tx_data = clearscreen;
		TransferCfg.tx_length = 2;
		I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
	}
	uint8_t setstartpos[2] = {0x00, 0x02};
	I2CSend(setstartpos, 2);
}

void CalcInit (void) {
	I2C_Init(LPC_I2C1, 100000);
	I2C_Cmd(LPC_I2C1, ENABLE);
	// Initialise the Screen
	I2CScreenSetup();
	while(count!=10000000){count+=1;}
	// Clears the screen
	I2CScreenClear();
	// Writes Hello World
	uint8_t writecalc[11] = {0x40, 0xC3, 0xC1, 0xCC, 0xC3, 0xD5, 0xCC, 0xC1, 0xD4, 0xCF, 0xD2};
	I2CSend(writecalc, 11);
}

void task3 (void) {
	count=0;while(count!=10000000){count+=1;}
	I2CScreenClear();
	addinc = 0x21;
	I2CSend(setHigh, 1);
}

void writer (int pointer){
	addinc = 0x3B;
	if (calc[pointer] == 0xA0) {
		I2CScreenClear();
	}
	else {
		uint8_t writechar[2] = {0x40, calc[pointer]};
		I2CSend(writechar, 2);
	}	
	if (calc[pointer] == 0xBD) {
		count=0;while(count!=1000000){count+=1;}
		I2CScreenClear();
	}
}

void RowCheck (uint8_t DataIn) {
	switch(DataIn) {
		case 0x0E :
			rpointer += 12;
			writer(rpointer);
			break;
		case 0x0D :
			rpointer += 8;
			writer(rpointer);
			break;
		case 0x0B :
			rpointer += 4;
			writer(rpointer);
			break;
		case 0x07 :
			writer(rpointer);
			break;
	}
}
int testfunc (void){

	addinc = 0x21;
	uint8_t ColCheck[1] = {0xEF};
	I2CSend(ColCheck,1);
	I2CRecieve(keydata,1);
	keydata[0] = keydata[0] & 0x0F;
	if (keydata[0] != 0x0F) {
		rpointer += 4;
		RowCheck(keydata[0]);
		return 0;
	}
	ColCheck[0] = 0xDF;
	I2CSend(ColCheck,1);
	I2CRecieve(keydata,1);
	keydata[0] = keydata[0] & 0x0F;
	if (keydata[0] != 0x0F) {
		rpointer += 3;
		RowCheck(keydata[0]);
		return 0;
	}
	ColCheck[0] = 0xBF;
	I2CSend(ColCheck,1);
	I2CRecieve(keydata,1);
	keydata[0] = keydata[0] & 0x0F;
	if (keydata[0] != 0x0F) {
		rpointer += 2;
		RowCheck(keydata[0]);
		return 0;
	}
	ColCheck[0] = 0x7F;
	I2CSend(ColCheck,1);
	I2CRecieve(keydata,1);
	keydata[0] = keydata[0] & 0x0F;
	if (keydata[0] != 0x0F) {
		rpointer += 1;
		RowCheck(keydata[0]);
		return 0;
	}
	return 0;
}
void EINT3_IRQHandler(void) {
	addinc = 0x21;
	testfunc();

	NVIC_ClearPendingIRQ(EINT3_IRQn);
	//GPIO_SetDir(1,0x00B40000,1);
	//GPIO_SetValue(1,0x00B40000);
	count = 0;
	while(count!=1000000){count+=1;}
	GPIO_ClearInt(0,0x00B40000);
	//GPIO_ClearValue(1,0x00B40000);
	rpointer = -1;
	addinc = 0x21;
	I2CSend(setLow, 1);
	NVIC_ClearPendingIRQ(EINT3_IRQn);
}

void task4 (void){

}

int main (void) {
	serial_init();
	CalcInit();
	task3();
	while(1) {
	}
}
