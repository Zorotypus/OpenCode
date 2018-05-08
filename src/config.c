/*
config.c
配置文件
*/

#include "stm32f10x.h"
#include "config.h"


//时钟配置
ErrorStatus HSEStartUpStates;
void RCC_Configration()
{
	RCC_DeInit();
	//Configures the External High Speed oscillator (HSE).
	RCC_HSEConfig(RCC_HSE_ON);
	//Waits for HSE start-up.
	HSEStartUpStates=RCC_WaitForHSEStartUp();
	if(HSEStartUpStates==SUCCESS)
	{

		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		//配置PCLK1   36M
		RCC_PCLK1Config(RCC_HCLK_Div2);
		//配置PCLK2   72M
		RCC_PCLK2Config(RCC_HCLK_Div1);
		//配置HCLK=SYSCLK  72M
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		//配置PLL 8M*9=72M
		RCC_PLLCmd(ENABLE);
	}
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET)
	{
		//等待PLL启动
	}
	//PLL作为系统时钟
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	//返回0x08
	while(RCC_GetSYSCLKSource()!=0x08)
	{

	}
}

//中断优先级配置
void NVIC_Configration()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

