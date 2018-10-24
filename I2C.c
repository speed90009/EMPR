#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"

Status DataAttempt;
char currentstatus[20] = "";
char addrboi[15] = "";
I2C_M_SETUP_Type TransferCfg;
char response[12] = "";
int no_devices = 0;
int addinc = 0;
uint8_t data[1] = {0};
uint8_t clearscreen[2] = {0x40,0xA0};
uint8_t resetscreen[2] = {0x00,0x02};
int count; 
int counter;

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

	//UART1
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
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

unsigned int int_to_int(unsigned int k) {
    return (k == 0 || k == 1 ? k : ((k % 2) + 10 * int_to_int(k / 2)));
}

void I2CTypeSetup (void) {
	TransferCfg.sl_addr7bit = addinc;
	TransferCfg.tx_data = data;
	TransferCfg.tx_length = 1;
	TransferCfg.rx_data = 0;
	TransferCfg.rx_length = 0;
	TransferCfg.retransmissions_max = 3;
}

void I2CScreenSetup (void) {
	uint8_t screendata[12] = {0x00,0x34,0x0c,0x06,0x35,0x04,0x10,0x42,0x9f,0x34,0x02,0x01};
	TransferCfg.sl_addr7bit = addinc;
	TransferCfg.tx_data = screendata;
	TransferCfg.tx_length = 12;
	I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
}

void I2CScreenClear (void) {
	for (counter=0; counter<80; counter++) {
		TransferCfg.tx_data = clearscreen;
		TransferCfg.tx_length = 2;
		I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
	}
}

void I2CSend (uint8_t* inputdata, int inputsize) {
	TransferCfg.tx_data = inputdata;
	TransferCfg.tx_length = inputsize;
	I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
}

void task1 (void)
{
	serial_init();
	I2C_Init(LPC_I2C1, 100000);
	I2C_Cmd(LPC_I2C1, ENABLE);	
	for (addinc=0; addinc<128; addinc++) {	
		I2CTypeSetup();
		DataAttempt = I2C_MasterTransferData(LPC_I2C1, &TransferCfg, I2C_TRANSFER_POLLING);
		if (DataAttempt == SUCCESS) {
			no_devices += 1;
		}
	}
	sprintf(currentstatus, "%d devices connected to i2c bus\n\r", no_devices);	
	write_usb_serial_blocking(currentstatus, 32);
}

void task2 (void) {
	addinc = 0x3B;
	// Initialise the Screen
	I2CScreenSetup();
	while(count!=10000000){count+=1;}
	// Clears the screen
	I2CScreenClear();
	// Writes Hello Worls
	uint8_t writehello[6] = {0x40, 0xC8, 0x65, 0x6c, 0x6c, 0x6f};
	I2CSend(writehello, 6);	
	uint8_t writenewline[2] = {0x00, 0xc0};
	I2CSend(writenewline, 2);
	uint8_t writeworld[6] = {0x40, 0xD7, 0x6f, 0x72, 0x6c, 0x64};
	I2CSend(writeworld, 6);
	
}

int main (void) {
	task1();
	task2();
	return 0;
}
