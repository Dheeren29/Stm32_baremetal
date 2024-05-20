/*Enable the clock source for USART1 and the GPIOA port.
Configure the Tx pin as output and the Rx pin as input in alternate function configuration.
Calculate the baud rate.
Configure the baud rate register.
Enable USART baud generation.
Set the TE and RE flags in the CR1 registers to start transmitting and receiving data.

Enable clocK access to GPIOA
SET PA2 MOODE TO Alternate function Mode
SET PA2 Alternative function type to UART_TX (AF7)
CONFIGURE UART MODULE
ENABLE CLOCK ACCESS TO UART2
CONFIGURE BAUDRATE
CONFIGURE TRANSFER DIRECTION
ENABLE UART MODULE					*/

#include <stdint.h>
#include <stdio.h>
#define APB1_CLK 8000000			//8 MHZ
#define UART_BAUDRATE 9600

uint32_t *RCC_APB2ENR 	= (uint32_t*) 0x40021018;			//to enable gpioA
uint32_t *GPIOA_CRL 	= (uint32_t*) 0x40010800;			//to enable alternate function mode in mode register

uint32_t *RCC_APB1ENR 	= (uint32_t*) 0x4002101c;			//to enable usart2
uint32_t *USART2_SR 	= (uint32_t*) 0x40004400;
uint32_t *USART2_DR 	= (uint32_t*) 0x40004404;
uint32_t *USART2_BRR 	= (uint32_t*) 0x40004408;
uint32_t *USART2_CR1 	= (uint32_t*) 0x4000440c;

void delay(void)
	{for (uint32_t i=0; i<30000; i++);}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t Baudrate)
	{    return (PeriphClk / (Baudrate));	}

void usart2_tx_init(void)
{	*RCC_APB2ENR = *RCC_APB2ENR | (0x1<<2);				//to enable gpioA clock for pin PA2 as transmitter

	*GPIOA_CRL   = *GPIOA_CRL   & ~(0xf<<8);
	*GPIOA_CRL   = *GPIOA_CRL   | (0xb<<8);				//Output mode, max speed 50 MHz and Alternate function output Push-pull

	//CONFIGURE UART MODULE
	*RCC_APB1ENR = *RCC_APB1ENR | (0x1<<17);			//to enable USART2
	*USART2_BRR  = compute_uart_bd(APB1_CLK, UART_BAUDRATE);

	//CONFIGURE TRANSFER DIRECTION
	*USART2_CR1 = *USART2_CR1 | (0x1<<3);			 // TE: Transmitter enable
	//*USART2_CR1 = 0x8;			 					// TE: Transmitter enable
	*USART2_CR1 = *USART2_CR1 | (0x1<<13);			 // UE: Usart enable
}

int __io_putchar(int ch)
{	delay();
	while (!(*USART2_SR & (0x1<<7)))			// Make sure the transmit data register bit is empty (TXE)
		{	}
	*USART2_DR = (ch & 0xFF);						// Write to transmit data register
	return ch;
}

int main(void)
{	usart2_tx_init();
	while(1)
	{	printf("Hello world \n");
	}
	for(;;);
}
