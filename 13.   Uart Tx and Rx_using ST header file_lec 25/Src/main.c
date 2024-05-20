#include "stm32f1xx.h"
#include <stdint.h>
#define APB1_CLK 8000000			//8 MHZ
#define UART_BAUDRATE 9600

void delay(void)
	{for (uint32_t i=0; i<300000; i++);}

static uint32_t compute_uart_bd(uint32_t PeriphClk, uint32_t Baudrate)
	{    return (PeriphClk / (Baudrate));	}

void usart2_tx_init(void)
{	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;						//to enable gpio clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;					//to enable USART2 clock

	GPIOA->CRL &= ~(GPIO_CRL_CNF2);						//clear the CNF bits
	GPIOA->CRL |= GPIO_CRL_MODE2 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_0;		//Output mode, max speed 50 MHz and Alternate function output Push-pull and PA3 input mode to floating input (reset state)

	USART2->BRR  = compute_uart_bd(APB1_CLK, UART_BAUDRATE);	//set baud rate as EA6
	//*USART2_BRR = 0XEA6;

	// TE: Transmitter enable, UE: Usart enable and RE: Receiver enable
	USART2->CR1 |= USART_CR1_TE | USART_CR1_UE | USART_CR1_RE;
}

char usart2_read(void)
	{	while (!(USART2->SR & USART_SR_RXNE))
		{	}
		return USART2->DR;
	}

void usart2_write(int ch)
	{	//delay();
		while (!(USART2->SR & USART_SR_TXE))		// Make sure the transmit data register bit is empty (TXE) or either you can use TC, have little difference only
		{	}
		USART2->DR = (ch & 0xFF) ;					// Write to transmit data register
	}

int main(void)
{	usart2_tx_init();
	while(1)
	{	char ch = usart2_read();
		usart2_write(ch);
	}
	for(;;);
}
