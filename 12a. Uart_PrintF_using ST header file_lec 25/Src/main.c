
//code is same as we previously did for UART (code no 11a) but use the stm header file and its function

#include "stm32f1xx.h"
#include "stdio.h"

void delay(unsigned long delay)
	{	while (delay)
			delay--;	}

int  __io_putchar(int ch)
	{	while (!(USART2->SR & USART_SR_TXE)) 		// Wait until TXE bit is set
		{  }
		USART2->DR = (ch & 0xFF);					// Write character to data register}
		return(ch);
	}

int main(void)
{   // Enable PORT A, USART2 clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Configure RX as floating input and TX as output push-pull
    GPIOA->CRL &= ~(GPIO_CRL_CNF2);						//clear the CNF bits
    GPIOA->CRL |= GPIO_CRL_MODE2 | GPIO_CRL_CNF2_1;

    // Enable USART and TX
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;

    // Configure Baud Rate
    USART2->BRR = 8000000/9600;  // Set 115200 Baud, 8 MHz crystal

    while (1)
    {   //SendChar('A'); // Test UART
        printf("Hello world \n");
        delay(50000);
    }
    for(;;);
}
