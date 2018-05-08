/*
USART.C
SPI
*/

#include"stm32f10x.h"
#include"usart.h"
#include "command.h"
#include "string.h"
u8 UART_BUF[USART_RX_LEN];

void uart1_init(u32 baud)
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	NVIC_InitTypeDef NVIC_InitStrue;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,ENABLE);

	gpio.GPIO_Mode=GPIO_Mode_AF_PP;
	gpio.GPIO_Pin=GPIO_Pin_9;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);

	gpio.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin=GPIO_Pin_10;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&gpio);

	NVIC_InitStrue.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStrue);


	usart.USART_BaudRate=baud;
	usart.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	usart.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	usart.USART_StopBits=USART_StopBits_1;
	usart.USART_Parity=USART_Parity_No;
	usart.USART_WordLength=USART_WordLength_8b;

	USART_Init(USART1,&usart);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
}

void uart2_init()
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	NVIC_InitTypeDef NVIC_InitStrue;
	DMA_InitTypeDef dma;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	USART_DeInit(USART2);

	gpio.GPIO_Mode=GPIO_Mode_AF_PP;
	gpio.GPIO_Pin=GPIO_Pin_2;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);

	gpio.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin=GPIO_Pin_3;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&gpio);

	NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=7;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStrue);


	usart.USART_BaudRate=9600;
	usart.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	usart.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	usart.USART_StopBits=USART_StopBits_1;
	usart.USART_Parity=USART_Parity_No;
	usart.USART_WordLength=USART_WordLength_8b;

//	DMA_DeInit(DMA1_Channel6);
//	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);;
//	dma.DMA_MemoryBaseAddr = (uint32_t) UART_BUF;
//	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
//	dma.DMA_BufferSize = 512;
//	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	dma.DMA_Mode = DMA_Mode_Normal;
//	dma.DMA_Priority = DMA_Priority_Medium;
//	dma.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel6, &dma);
//
//	DMA_Cmd(DMA1_Channel6, ENABLE);
//	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);

	USART_Init(USART2,&usart);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
//	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART2,ENABLE);
}

void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32	cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStruction;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA_CHx);

	DMA_InitStruction.DMA_PeripheralBaseAddr=cpar;
	DMA_InitStruction.DMA_MemoryBaseAddr=cmar;
	DMA_InitStruction.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStruction.DMA_BufferSize=cndtr;
	DMA_InitStruction.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStruction.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStruction.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStruction.DMA_Priority=DMA_Priority_Medium;
	DMA_InitStruction.DMA_M2M=DMA_M2M_Disable;
	DMA_InitStruction.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStruction.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;

	DMA_Init(DMA_CHx,&DMA_InitStruction);
}
void MYDMA_Enable(DMA_Channel_TypeDef* DMA_CHx)
{
	MYDMA_Config(DMA_CHx,(u32)&USART2->DR,(u32)UART_BUF,USART_RX_LEN);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	DMA_Cmd(DMA_CHx,ENABLE);
}


void USART1_IRQHandler()
{
	u8 res;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		res=USART_ReceiveData(USART1);
		if(!(USART_RX_STA1&0x8000))   //接收到了数据 ,未接收到0x0a
		{
			if(USART_RX_STA1&0x4000) //接收到了空格
			{
				if(res!=0x0a)       //未接收到回车
				{
					USART_RX_STA1=0;
				}
				else
				{
					USART_RX_STA1|=0x8000;
				}
			}
			else
			{
				if(res==0x0d)   //接受到了空格
				{
					USART_RX_STA1|=0x4000;
				}
				else
				{
					USART_RX_BUFF1[USART_RX_STA1&0X3FFF]=res;
					USART_RX_STA1++;
					if(USART_RX_STA1>USART_RX_LEN-1)USART_RX_STA1=0;
				}
			}
		}
	}
}

void USART2_IRQHandler()
{
	if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)
	{
		USART_ReceiveData(USART2);  	//清除IDLE标志位
		DMA_Cmd(DMA1_Channel6,DISABLE);

		command.cmd_len=USART_RX_LEN-DMA_GetCurrDataCounter(DMA1_Channel6);	//总共传输了多少字节数据
		memcpy(command.cmd_buff, UART_BUF,command.cmd_len);
		DMA_SetCurrDataCounter(DMA1_Channel6,USART_RX_LEN);
		DMA_Cmd(DMA1_Channel6,ENABLE);
	}
//	if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)
//	{
//		USART_ReceiveData(USART2); //clear idle flag bit
//	}
}

void USART2_SEND_DATA(u8 *databuff,u16 len)
{
	u16 i;
	for (i = 0; i < len; i++)
	{
		while (USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET)
			;
		USART_SendData(USART2, *(databuff + i));
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
}
//void test_usart1()
//{
//	u8 i;
////	if(USART_RX_STA1&0x8000)
////	{
//		for(i=0;i<sizeof(test_buff);i++)
//		{
//			USART_SendData(USART1,test_buff[i]);
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
//		}
//		USART_RX_STA1=0;
////	}
//}
