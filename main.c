#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


#include <string.h>
#include <stdlib.h>



volatile uint32_t delayCount = 0;                      /* counts 1ms timeTicks       */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
	if (delayCount > 0) delayCount--;
}
void Delay (uint32_t dlyTicks) {                                              
  delayCount = dlyTicks;
  while (delayCount > 0);
}





void initAll()
{
 
		USART_InitTypeDef usart;
		GPIO_InitTypeDef gpio;
	
    __enable_irq();
 
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		SysTick_Config(SystemCoreClock/1000);
 
 
    GPIO_StructInit(&gpio);
 
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Pin = GPIO_Pin_11;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);
 
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);
 
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Pin = GPIO_Pin_0;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &gpio);

		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Pin = GPIO_Pin_1;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &gpio);
 
 
 
 
 
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
		
		GPIO_StructInit(&gpio);
 
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &gpio);
		
 
 
		USART_StructInit(&usart);
		usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		usart.USART_BaudRate = 115200;	
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_No ;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

		USART_Init(USART3, &usart);	
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		USART_Cmd(USART3, ENABLE);
		
		USART_StructInit(&usart);
		usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		usart.USART_BaudRate = 9600; 
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_No ;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

		USART_Init(UART4, &usart); 
		USART_ITConfig(UART4, USART_IT_TC, ENABLE);
		USART_Cmd(UART4, ENABLE);
	 
		
	 
}


void send_to_uart3(uint8_t data)  {
while(!(USART3->SR & USART_SR_TC));
USART3->DR=data;
}

void send_to_uart4(uint8_t data)  {
while(!(UART4->SR & USART_SR_TC));
UART4->DR=data;
}


 
void send_str_uart(uint8_t * string,void (*foo)(uint8_t)) {

uint8_t i=0;
while(string[i]) {
foo(string[i]);
i++;
}
foo('\0');
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




int main (void)
{
		uint8_t resiveData;
		uint8_t i = 0;
		uint8_t buf[9];
		uint8_t initBuf[20];
		buf[8] = '\0';
	
	
		initAll();
	
	
	while(1)	
	{
		read_str_uart3(initBuf);

	if (strcmp(initBuf, "connect"))
		{
			uint32_t speed;
			send_to_uart3('#');
			read_str_uart3(initBuf);
			speed = atoi(initBuf);
			send_to_uart3('*');
			send_to_uart3(speed);
			
		}
			
		
		
	} 
/*
		
	while(1)
	{
		
			if (i > 7) 
			{
				i = 0;
				send_to_uart3(buf[0]);
				send_to_uart3(buf[1]);
				send_to_uart3(buf[2]);
				send_to_uart3(buf[3]);
				send_to_uart3(buf[4]);
				send_to_uart3(buf[5]);
				send_to_uart3(buf[6]);
				send_to_uart3(buf[7]);
			}
			
			if (USART3->SR & USART_SR_RXNE) 
				{
					
					buf[i] = USART3->DR;
					i++;
				}
				
			
			
		
				
	
	
	
}
*/	
}


