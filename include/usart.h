#ifndef USART_H_
#define USART_H_

#include "stm32f10x.h"

#define  USART_RX_LEN   256                   //���յ���Ŀ


u8 USART_RX_BUFF1[USART_RX_LEN];       //��������
u16 USART_RX_STA1;                      //���ձ�־
u16 USART_RX_STA2;                      //���ձ�־

void uart1_init(u32 baud);
void uart2_init();
void test_usart1();
void uart_send_data(uint8_t *pdata, uint16_t len);
void MYDMA_Enable(DMA_Channel_TypeDef* DMA_CHx);
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32	cmar,u16 cndtr);
void USART2_SEND_DATA(u8 *databuff,u16 len);
#endif
