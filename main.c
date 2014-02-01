#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "DES.h"
#include "UART_IO.h"


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


void bitTransToMas(char inBytes[8],char outMas[64])
{

	long long int byte64 = 0;
	long long int mask;
	uint8_t i;
	uint8_t j;
	memcpy(&byte64, inBytes, 8);

	
	for(i = 0; i < 64; i++) 
	{
		mask = 1;
		for(j = 63;j > i; j--) mask *= 2;
		if (byte64 & mask) {outMas[i] = 1; }
		else {outMas[i] = 0; }

	}


}


void masTransToBit(char outBytes[8],char inMas[64])
{

	long long int byte64 = 0;
	long long int mask = 1;
	uint8_t i;
	uint8_t j;
	

	for(i = 0; i < 64; i++) 
	{
		mask = 1;
		if(inMas[i]) 
		{
			for(j = 63;j > i; j--) mask *= 2;
			byte64 |= mask;
		}
		
	}

	memcpy(outBytes, &byte64, 8);

}







int main (void)
{
		uint8_t* initStr = "ARM: init ok, mode = "; 
		uint8_t mode = 'e';
		uint8_t i = 0;
		uint8_t buf[9];
		uint8_t initBuf[20];
		uint8_t key[56] = {0};
		uint8_t outBlk[64] = {0};
		uint8_t inBlk[64] = {0};
		
		buf[8] = '\0';
	
	
		initAll();
	


mode = '0';
while (mode != 'e' && mode != 'd') if (USART3->SR & USART_SR_RXNE) mode = USART3->DR;
		
		
		send_str_uart(initStr,strlen(initStr),0,send_to_uart3);
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		send_to_uart3(mode);
		if (mode == 'e') GPIO_SetBits(GPIOD, GPIO_Pin_12);
		if (mode == 'd') GPIO_SetBits(GPIOD, GPIO_Pin_15);
		
	while(1)
	{
		
			if (i > 7) 
			{
				i = 0;
				
					

				GPIO_SetBits(GPIOD, GPIO_Pin_14);
				bitTransToMas(buf,inBlk);
				if(mode == 'e') EncryptDES(key,outBlk,inBlk);
				else DecryptDES(key,outBlk,inBlk);
				masTransToBit(buf, outBlk);
				
				GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
				send_str_uart(buf,8,0,send_to_uart3);
			}
			
			if (USART3->SR & USART_SR_RXNE) 
				{
					
					buf[i] = USART3->DR;
					i++;
				}
				
				
				
			
			
		
				
	
	
	
}

}


