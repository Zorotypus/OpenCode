/*
config.c
�����ļ�
*/

#include "stm32f10x.h"
#include "config.h"


//ʱ������
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
		//����PCLK1   36M
		RCC_PCLK1Config(RCC_HCLK_Div2);
		//����PCLK2   72M
		RCC_PCLK2Config(RCC_HCLK_Div1);
		//����HCLK=SYSCLK  72M
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		//����PLL 8M*9=72M
		RCC_PLLCmd(ENABLE);
	}
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET)
	{
		//�ȴ�PLL����
	}
	//PLL��Ϊϵͳʱ��
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	//����0x08
	while(RCC_GetSYSCLKSource()!=0x08)
	{

	}
}

//�ж����ȼ�����
void NVIC_Configration()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

