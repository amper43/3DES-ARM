#include "UART_IO.h"

void send_to_uart3(uint8_t data)  {
while(!(USART3->SR & USART_SR_TC));
USART3->DR=data;
}

void send_to_uart4(uint8_t data)  {
while(!(UART4->SR & USART_SR_TC));
UART4->DR=data;
}


 
void send_str_uart(uint8_t * string,int length, uint8_t numb,void (*foo)(uint8_t))
{

uint8_t i;
	
for(i = 0;i < length;i++)	
	if (numb) foo(string[i]+'0');
	else foo(string[i]);
}


void read_str_uart3(uint8_t* buf)
{
	uint8_t c = '0';
	uint8_t i = 0;
	while(c != '\0')
	{		
		if(USART3->SR & USART_SR_RXNE) 
				{
					c = USART3->DR;
					buf[i] = c;
					i++;
				}
		
	}
	send_to_uart3('r');
}
