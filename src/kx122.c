/*
kx122.c
Sensor
2017-12-22  23:45
Azreal
*/

#include "stm32f10x.h"
#include "kx122.h"
#include "spi.h"
#include "delay.h"
#include "diag/Trace.h"
#include "w25qxx.h"
#include "Timer.h"
#include "mymalloc.h"
#include "command.h"

command_line command;

void Kx122_Init()
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);

	SPI1_Init();
	Kx122_Read_ID();
}

//中断控制位
void Kx122_INT_Init()
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	EXTI_InitTypeDef   EXIT_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);

	EXIT_InitStruct.EXTI_Line=EXTI_Line11;
	EXIT_InitStruct.EXTI_LineCmd=ENABLE;
	EXIT_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXIT_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;

	EXTI_Init(&EXIT_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel=EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
}

//读取KX122 ID 。判断是否在线
void Kx122_Read_ID()
{
//	u8 IDval=0;
//	IDval=Kx122_Read_Register(KX122_ID);
	while(Kx122_Read_Register(KX122_ID)!=0x1B)
	{
#ifdef DEBUG
		trace_printf("KX122_Init failed!\n");
#endif
		timer_sleep(500);
	}
#ifdef DEBUG
	trace_printf("KX122_Init success!\n");
#endif
}
//清除中断
u8 Kx122_Read_INT()
{
	u8 tmpval=0;
	tmpval=Kx122_Read_Register(KX122_INT_REL);
	return tmpval;
}
//读取中断事件
u8 Kx122_Read_INS2()
{
	u8 tmpval=0;
	tmpval=Kx122_Read_Register(KX122_INS2);
	return tmpval;
}
//清除缓存
void Kx122_Clear_Buff()
{
	Kx122_Write_Register(KX122_BUF_CLEAR,0xff);
}
//开始采集
void Kx122_Start_Measure()
{
	u8 tmpval=0;
	Kx122_Clear_Buff();
	tmpval=Kx122_Read_Register(KX122_CNTL1);
	//Kx122_Write_Register(KX122_CNTL1,tmpval|0x40);
	Kx122_Write_Register(KX122_CNTL1,tmpval|0x80);
}
//停止采集
void Kx122_Stop_Measure()
{
	u8 tmpval;
	Kx122_Clear_Buff();
	tmpval=Kx122_Read_Register(KX122_CNTL1);
	Kx122_Write_Register(KX122_CNTL1,tmpval&(~0x80));
}
//读取样本数量
u16 Kx122_Buff_Len()
{
	u16 val1;
	u16 val2;
	u16 val;
	val1=Kx122_Read_Register(KX122_BUF_STATUS1);
	val2=Kx122_Read_Register(KX122_BUF_STATUS2);

	val=val1+((val2&0x07)<<8);
	return val;
}
//读取三轴数据
void Kx122_Read_XYZ(u8 *pBuffer,u8 len)
{
	Kx122_Read_Buff(pBuffer,len);
}
//直接得到三轴数据
void Kx122_Get_XYZ(u8 *pBuffer,u16 len)
{
	u8 i=0;
	u8 regval=0;
	regval=KX122_XOUT_L|0x80;
	Kx122_CsLow();
//	spi1_write_byte(&regval,1);
//	spi1_read_byte(pBuffer,len);
	SPI1_ReadWrite_Byte(regval);
	for(i=0;i<len;i++)
	{
		pBuffer[i]=SPI1_ReadWrite_Byte(0xff);
	}
	Kx122_CsHigh();
}

//KX122初始配置
void Kx122_Config(int baud)
{
	u8 tmpval=0;
#ifdef watemark
//	读取COTR值,查看CNTL2的COTR位
//	tmpval=Kx122_Read_Register(KX122_COTR);
//	tmpval=Kx122_Read_Register(KX122_CNTL2);
//	Kx122_Write_Register(KX122_CNTL2,tmpval|0x40);
//	tmpval=Kx122_Read_Register(KX122_COTR);
//	tmpval=Kx122_Read_Register(KX122_CNTL2);
//	Kx122_Write_Register(KX122_CNTL2,tmpval&~(0x40));
	//PC1=0  进入standby模式
	Kx122_Write_Register(KX122_CNTL1,tmpval);
	timer_sleep(5);

	tmpval=Kx122_Read_Register(KX122_CNTL1);
	Kx122_Write_Register(KX122_CNTL1, tmpval & (~0x80));
	//high resolution
	Kx122_Write_Register(KX122_CNTL1,tmpval|0x40);
	//8G分辨率
	Kx122_Write_Register(KX122_CNTL1,tmpval|0x10);
	//中断
	Kx122_Write_Register(KX122_CNTL1,tmpval|0x20);
	//采样率设置
	Sample_Rate(baud);
	tmpval=Kx122_Read_Register(KX122_ODCNTL);

	//初始化

	Kx122_Write_Register(KX122_ODCNTL,tmpval|0x40);
//	INC1 中断配置
//	bit<6.7>	PWSEL	中断宽度设置  缺省50us
//	bit5     	IEN1  	中断使能
//	bit4     	IEA1  	中断极性
//	bit3     	IEL1 	中断响应
//	bit1		STPOL	自测极性
//	bit0		SPI3E	3线SPI
	tmpval=Kx122_Read_Register(KX122_INC1);
	Kx122_Write_Register(KX122_INC1,tmpval|0x38);

	//配置哪个轴向触发中断。所有都能触发
//	tmpval=Kx122_Read_Register(KX122_INC2);
//	Kx122_Write_Register(KX122_INC2,tmpval|0x3F);

//	INC4 中断事件路由到INT1
//	bit7	FFI1
//	bit6	BFI1	Buffer full
//	bit5	WMI1	Watermark interrupt
//	bit4	DRDYI1	Data ready
//	bit2	TDTI1	Tap interrupt
//	bit1	WUFI1	Wake-Up
//	bit0	TPI1	Tilt position
//只开启1020个点才中断
	tmpval=Kx122_Read_Register(KX122_INC4);
	Kx122_Write_Register(KX122_INC4,tmpval|0x10);
//170*6=1020点
//	tmpval=Kx122_Read_Register(KX122_BUF_CNTL1);
//	Kx122_Write_Register(KX122_BUF_CNTL1,tmpval|0xaa);
//	//开启设置的中断和buffer full中断  1020和2040中断
//	tmpval=Kx122_Read_Register(KX122_BUF_CNTL2);
//	Kx122_Write_Register(KX122_BUF_CNTL2,tmpval|0xA0);

	tmpval=Kx122_Read_Register(KX122_CNTL1);
	Kx122_Write_Register(KX122_CNTL1,tmpval|0x10);
#endif
	//read cotr is 0x55 when cotc is 0
	tmpval = Kx122_Read_Register(KX122_COTR);

	//
	tmpval = Kx122_Read_Register(KX122_CNTL2);
	Kx122_Write_Register(KX122_CNTL2, tmpval | 0x40);

	tmpval = Kx122_Read_Register( KX122_COTR);

 	//clear cotc
	Kx122_Write_Register( KX122_CNTL2, tmpval & (~0x40));

	//复位芯片
	Kx122_Write_Register(KX122_CNTL1, 0x00);
	timer_sleep(5);

	tmpval = Kx122_Read_Register(KX122_CNTL1);
	//配置PC1为0，当需要改变配置控制
	Kx122_Write_Register(KX122_CNTL1, tmpval & (~0x80));
	//res = high resolution
	Kx122_Write_Register(KX122_CNTL1, tmpval | 0x40);
	//GSEL1, GSEL0 = 1,0 +/- 8G
	Kx122_Write_Register(KX122_CNTL1, tmpval | 0x10);

// 	//DRDY report ENABLE
	Kx122_Write_Register(KX122_CNTL1, tmpval | 0x20);

// 	//设置默认采样率
	//KX122_write_register(KX122_ODCNTL,ODR_RATIO_25600);
// 	//if device is KX022 max odr ratio is 1600 --> value 0x07 25600 ->value 0x0f  3200 ->0x0c 6400 ->0x0d,12800 -0x0e
// 	//800->0x06
	//IIR滤波器设置成ODR/2
	Sample_Rate(baud);
	tmpval=Kx122_Read_Register(KX122_ODCNTL);
	Kx122_Write_Register(KX122_ODCNTL, 0x0c|0x40);
	//指定采样水印中断数据阀值,101010011=339 when RES =1个数据点
	//10101010=170	170*6 =1020			 0xff *6 = 	1530
	//KX122_write_register(KX122_BUF_CNTL1,0x10);
	//KX122_write_register(KX122_BUF_CNTL1,0xAA);
	//配置外部中断源及中断的触发方式
	// bit 7 Reserved
// bit 6 Reserved
// bit 5 IEN1 1 使能打开
// bit 4 IEA1 1 极性高 0 极性低
// bit 3 IEL1 50us 脉宽
// bit 2 Reserved
// bit 1 STPOL
// bit 0 SPI3E

	tmpval = Kx122_Read_Register(KX122_INC1);
	Kx122_Write_Register(KX122_INC1, tmpval | 0x38);

	//配置哪个轴可以触发中断，默认可以全部轴和方向 00111111
	tmpval = Kx122_Read_Register(KX122_INC2);

	//控制哪些中断事件路由到INT1，
//  bit 7 ->0
// 	bit 6 ->BFI1
// 	bit 5 ->WMI1
// 	bit 4 ->DRDYI1
// 	bit 3 ->Reserved
// 	bit 2 ->TDTI1
// 	bit 1 ->WUFI1
// 	bit 0 ->TPI1
	Kx122_Write_Register(KX122_INC4, 0x10);

//控制INT2管脚
//  bit 7 ->0
// 	bit 6 ->0
// 	bit 5 ->IEN2 使能打开
// 	bit 4 ->EA2 极性高
// 	bit 3 ->IEL2 响应方式，一个50us的脉宽
// 	bit 2 ->0
// 	bit 1 ->0
// 	bit 0 ->0
	//自动50us脉冲的下降清中断源
	Kx122_Write_Register(KX122_INC5, 0x38 | 0x03);

	//控制哪些中断事件路由到INT2，
//  bit 7 ->0
// 	bit 6 ->BFI2
// 	bit 5 ->WMI2
// 	bit 4 ->DRDYI2
// 	bit 3 ->Reserved
// 	bit 2 ->TDTI2
// 	bit 1 ->WUFI2
// 	bit 0 ->TPI2
	Kx122_Write_Register(KX122_INC6, 0x60);
// 	//配置PC1为1，使能改变的配置，新采数据中断
	tmpval = Kx122_Read_Register(KX122_CNTL1);
	Kx122_Write_Register(KX122_CNTL1, tmpval | 0x10);

	Kx122_INT_Init();

}

//	0x50 400HZ
//	0x60 800HZ
//	0x70 1600Hz
//	0xC0 3200HZ
//	0xD0 6400HZ
//	0xE0 12800HZ
//	0xF0 25600HZ
void Sample_Rate(int baud)
{
	switch(baud)
	{
	case 400:
		Kx122_Write_Register(KX122_ODCNTL,0x05);
		break;
	case 800:
		Kx122_Write_Register(KX122_ODCNTL,0x06);
		break;
	case 1600:
		Kx122_Write_Register(KX122_ODCNTL,0x07);
		break;
	case 3200:
		Kx122_Write_Register(KX122_ODCNTL,0x0C);
		break;
	case 6400:
		Kx122_Write_Register(KX122_ODCNTL,0x0D);
		break;
	case 12800:
		Kx122_Write_Register(KX122_ODCNTL,0x0E);
		break;
	case 25600:
		Kx122_Write_Register(KX122_ODCNTL,0x0F);
		break;
	}
}

u8 Kx122_Read_Register(u8 reg)
{
	u8 temp;
	reg|=0x80;
	Kx122_CsLow();
	SPI1_ReadWrite_Byte(reg);
	temp=SPI1_ReadWrite_Byte(0xff);
	Kx122_CsHigh();
	return temp;
}

void Kx122_Write_Register(u8 reg,u8 temp)
{
	reg&=0x7F;
	Kx122_CsLow();
	SPI1_ReadWrite_Byte(reg);
	SPI1_ReadWrite_Byte(temp);
	Kx122_CsHigh();
}

void Kx122_Read_Buff(u8 *pBuf,u16 len)
{
	u16 i;
	u8 tmp;
	tmp=KX122_BUF_READ|0x80;
	Kx122_CsLow();
	SPI1_ReadWrite_Byte(tmp);
	for(i=0;i<len;i++)
	{
		pBuf[i]=SPI1_ReadWrite_Byte(0xff);
	}
	Kx122_CsHigh();
	Kx122_Clear_Buff();
}

u16 readsize;
u8 pBuffer_a[1020];
u8 pBuffer_b[1020];
u8 flag_buff=0;		//循环buff标志位
u16 datablock=0;		//要采集的数据块数

/*void EXTI15_10_IRQHandler()
{
//	val=Kx122_Read_INS2();
//	Kx122_Get_XYZ(pBuffer,6);
	//len=Kx122_Buff_Len();
//	pBuffer_a=mymalloc(1024);
//	pBuffer_b=mymalloc(1024);
//	if(pBuffer_a==NULL||pBuffer_b==NULL)
//	{
//		trace_printf("malloc size of sensorbuff error!");
//	}
//	while(len<1020);
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	Kx122_Read_INT();
	Kx122_Read_INS2();
	if(flag_buff==0)
	{
		Kx122_Read_Buff(pBuffer_a,1020);
		datablock++;
		flag_buff=1;
	}
	else
	{
		Kx122_Read_Buff(pBuffer_b,1020);
		datablock++;
		flag_buff=0;
	}
	if(flag_buff==1)
	{
		//写入flash
		W25qxx_Write_Check(pBuffer_a,(Flash_KX122_WRITE_ADDR+(datablock-1)*4096),sizeof(pBuffer_a));
		//myfree(pBuffer_a);
		//W25qxx_Read_Page(pBuffer,(Flash_KX122_WRITE_ADDR+(datablock-1)*4096),1020);
	}
	else
	{
		W25qxx_Write_Check(pBuffer_b,(Flash_KX122_WRITE_ADDR+(datablock-1)*4096),sizeof(pBuffer_b));
		//W25qxx_Read_Page(pBuffer_a,(flash_size+(datablock-1)*1020),1020);
		//myfree(pBuffer_b);
	}
	if(datablock>90000)
	{
		Kx122_Stop_Measure();
#ifdef DEBUG
		trace_printf("flash is full!");
#endif
	}
}
*/


void EXTI15_10_IRQHandler()
{
	static uint16_t i = 0;
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line11);
		if(flag_buff==0)
		{
			Kx122_Get_XYZ(pBuffer_a+i*6,6);
		}
		else if(flag_buff==1)
		{
			Kx122_Get_XYZ(pBuffer_b+i*6,6);
		}
		Kx122_Read_INT();

		i++;
		if(i>170)
		{
			i = 0;
			readsize = 1020;
			if(flag_buff == 0)
			{
				datablock++;
				W25qxx_Write_Check(pBuffer_a,(Flash_KX122_WRITE_ADDR+(datablock-1)*4096),sizeof(pBuffer_a));
				flag_buff = 1;
			}else
			if(flag_buff == 1)
			{
				datablock++;
				W25qxx_Write_Check(pBuffer_b,(Flash_KX122_WRITE_ADDR+(datablock-1)*4096),sizeof(pBuffer_b));
				flag_buff = 0;
			}
		}
	}
}

//void kx122_data_check()
//{
//	u16 n=0;
//	u32 i;
//	for(n=0;n<16;n++)
//	{
//		W25qxx_Read_Page(pBuffer_a,(flash_size+n*1020),1020);
//		for(i=0;i<1020;i++)
//		{
//			USART_SendData(USART2,pBuffer_a[i]);
//			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
//		}
//
//	}
//}

