/*
LowPowerCtrl.c
LowPowerCtrl
2018-01-01  22:02
Azreal
*/

#include "stm32f10x.h"
#include "w25qxx.h"
#include "spi.h"
#include "kx122.h"
//进入待机模式
void Sys_Enter_Standby_Mode()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE); //使能PWR外设时钟
	PWR_WakeUpPinCmd(ENABLE);
	PWR_EnterSTANDBYMode();
}
//退出待机模式
void Sys_Quit_Standby_Mode()
{
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);
	Sys_Enter_Standby_Mode();
}
//kx122进入standby mode   0.9uA
void Kx122_Enter_Standby_Mode()
{
	Kx122_Stop_Measure();
}
//kx122进入采集模式
void Kx122_Quit_Standby_Mode()
{
	Kx122_Start_Measure();
}
