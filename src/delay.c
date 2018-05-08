/*
delay.c
delay
*/

#include"stm32f10x.h"

u8 fac_us=0;
u16 fac_ms=0;

void delay_init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=SystemCoreClock/8000000;
	fac_ms=fac_us*1000;
}
void delay_ms(int ms)
{
	u32 temp=0;
	SysTick->LOAD=(u32)ms*fac_ms;
	SysTick->VAL=0;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL&=SysTick_CTRL_ENABLE_Pos;
	SysTick->VAL=0;
}

void delay_us(int us)
{
	u16 temp=0;
	SysTick->LOAD=us*fac_ms;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL=0;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL&=SysTick_CTRL_ENABLE_Pos;
	SysTick->VAL=0;
}
