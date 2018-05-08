/*
w25q128.c
flash
*/

#include "stm32f10x.h"
#include "w25qxx.h"
#include "spi.h"
#include "diag/Trace.h"
#include "Timer.h"
#include "command.h"


u16 W25X_ID=W25Q128;

void W25qxx_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_12);

	W25X_CsHigh();  //未选中flash

	SPI2_Init();
	while(W25X_ID!=W25qxx_ReadID())
	{
#ifdef DEBUG
		trace_printf("W25Q128 Init failed! \n");
#endif
		timer_sleep(500);
	}
#ifdef DEBUG
	trace_printf("W25Q128 Init SUCCESS! \n");
#endif
}

//读取状态寄存器
u8 W25qxx_Read_SR()
{
	u8 temp;
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_ReadStatusRegister);
	temp=SPI2_ReadWrite_Byte(W25X_Read);
	W25X_CsHigh();
	return temp;
}
//写状态寄存器
void W25qxx_Write_SR(u8 status)
{
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_WriteStatusRegister);
	SPI2_ReadWrite_Byte(status);
	W25X_CsHigh();
}
//读取flash ID
u16 W25qxx_ReadID()
{
	u16 Temp=0;
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_ManuDeviceID);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	Temp|=SPI2_ReadWrite_Byte(W25X_Read)<<8;
	Temp|=SPI2_ReadWrite_Byte(W25X_Read);
	W25X_CsHigh();
	return Temp;
}
//写使能
void W25qxx_Write_Enable()
{
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_WriteEnable);
	W25X_CsHigh();
}
void W25qxx_Write_Disable()
{
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_WriteDisable);
	W25X_CsHigh();
}
//等待空闲
void W25qxx_Wait_Busy()
{
	u8 busyStatus=0x01;
	while((busyStatus&W25qxx_Read_SR())==0x01);
}
//掉电模式
void W25qxx_Power_Down()
{
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_PowerDown);
	W25X_CsHigh();
}
//退出掉电模式
void W25qxx_Release_Power_Down()
{
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_ReleasePowerDown);
	W25X_CsHigh();
}

/*
读取数据，最多65536字节
*pBuffer: 字节指针
WriteAddr:地址
len:数据长度
*/


void W25qxx_Read_Page(u8 *pBuffer,u32 ReadAddr,u16 len)
{
	u32 i;
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_ReadData);
	SPI2_ReadWrite_Byte((u8)(ReadAddr>>16));
	SPI2_ReadWrite_Byte((u8)(ReadAddr>>8));
	SPI2_ReadWrite_Byte((u8)ReadAddr);
	for(i=0;i<len;i++)
	{
		pBuffer[i]=SPI2_ReadWrite_Byte(W25X_Read);
	}
	W25X_CsHigh();
	//W25qxx_Wait_Busy();
}
/*
向指定地址写入数据   最大256字节
*pBuffer: 数据指针
WriteAddr: 写地址
len ：数据长度
*/

void W25qxx_Write_Sector(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u16 i;
	W25qxx_Write_Enable();
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_PageProgram);
	SPI2_ReadWrite_Byte((u8)(WriteAddr>>16));
	SPI2_ReadWrite_Byte((u8)(WriteAddr>>8));
	SPI2_ReadWrite_Byte((u8)WriteAddr);
	for(i=0;i<len;i++)
	{
		SPI2_ReadWrite_Byte(pBuffer[i]);
	}
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}
/*
向指定地址写入数据   最大256字节
*pBuffer: 数据指针
WriteAddr: 写地址
len ：数据长度
*/
void W25qxx_Write_Page(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u8 i;
	W25qxx_Write_Enable();
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_PageProgram);
	SPI2_ReadWrite_Byte((u8)(WriteAddr>>16));
	SPI2_ReadWrite_Byte((u8)(WriteAddr>>8));
	SPI2_ReadWrite_Byte((u8)WriteAddr);
	for(i=0;i<len;i++)
	{
		SPI2_ReadWrite_Byte(pBuffer[i]);
	}
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}

/*
向指定地址写入数据   最大65535字节
*pBuffer: 数据指针
WriteAddr: 写地址
len ：数据长度
未进行檫除操作
*/
u16 sector_size=256;
void W25qxx_Write_NoCheck(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u16 secremain;
	secremain=sector_size-WriteAddr%sector_size; //剩余的字节空间
	if(len<secremain)
	{
		secremain=len;
		//W25qxx_Write_Sector(pBuffer,WriteAddr,secremain);
	}
	while(1)
	{
		W25qxx_Write_Sector(pBuffer,WriteAddr,secremain);
		if(len==secremain) break;
		else{
			pBuffer+=secremain;
			WriteAddr+=secremain;
			len-=secremain;
			if(len>sector_size)
			{
				secremain=sector_size;
			}
			else secremain=len;
		}
	}

}

/*
向指定地址写入数据   最大65535字节
*pBuffer: 数据指针
WriteAddr: 写地址
len ：数据长度
首先进行檫除操作
*/
u16 page_size=4096;
u8 W25qxx_Buffer[4096];  //因为要整页写入，所以要先声明一个sector
void W25qxx_Write_Check(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u16 secremain;
	u16 secoff;
	u16 secnum;
	u8* W25qxx_Buff;
	u16 i;
	secnum=WriteAddr/page_size;
	secoff=WriteAddr%page_size;
	secremain=page_size-secoff; 	 //剩余的字节空间
	W25qxx_Buff=W25qxx_Buffer;
	if(len<=secremain)secremain=len;  //少于4096-secoff字节
	while(1)
	{
		W25qxx_Read_Page(W25qxx_Buff,secnum*page_size,page_size); //先读取flash数据
		for(i=0;i<secremain;i++)
		{
			if(W25qxx_Buff[i+secoff]!=0xFF) break; //如果i=secremain 则不需要擦除
		}
		if(i<secremain)         //说明中间有一个数据不为0xFF
		{
			W25qxx_Sector_Erase(secnum); //擦除整个个数据块
			for(i=0;i<secremain;i++)     //复制pBuffer
			{
				W25qxx_Buff[i+secoff]=pBuffer[i];
			}
			W25qxx_Write_NoCheck(W25qxx_Buff,secnum*page_size,page_size); //整页写入
		}
		else
		{
			W25qxx_Write_NoCheck(pBuffer,WriteAddr,secremain); //不需要擦除的写入
		}
		if(secremain==len)break;
		else
		{
			secnum++;
			secoff=0;
			pBuffer+=secremain;
			WriteAddr+=secremain;
			len-=secremain;
			if(len>page_size)
			{
				secremain=page_size;
			}
			else secremain=len;
		}
	}
}

//全片檫除
void W25qxx_Chip_Erase()
{
	W25qxx_Write_Enable();
	W25qxx_Wait_Busy();
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_ChipErase);
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}
//块檫除
void W25qxx_Block_Erase(u16 SectorNum)
{
	u32 sectoraddr;
	sectoraddr=SectorNum*65536+FLASH_WRProt_Page_START;
	W25qxx_Write_Enable();
	W25qxx_Wait_Busy();
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_BlockErase64);
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>16));
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>8));
	SPI2_ReadWrite_Byte((u8)sectoraddr);
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}
//sector檫除
void W25qxx_Sector_Erase(u16 SectorNum)
{
	u32 sectoraddr;
	sectoraddr=SectorNum*4096+FLASH_WRProt_Page_START;
	W25qxx_Write_Enable();
	W25qxx_Wait_Busy();
	W25X_CsLow();  //片选
	SPI2_ReadWrite_Byte(W25X_SectorErase);
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>16));
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>8));
	SPI2_ReadWrite_Byte((u8)(sectoraddr));
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}

//flash进入掉电模式
void W25qxx_PowerDown()
{
	W25X_CsLow();
	SPI1_ReadWrite_Byte(W25X_PowerDown);
	W25X_CsHigh();
}
//flash唤醒模式
void W25qxx_WakeUp()
{
	W25X_CsLow();
	SPI1_ReadWrite_Byte(W25X_ReleasePowerDown);
	W25X_CsHigh();
}

void W25qxx_Sector_Init(u8 sectornum)
{
	int i;
	for(i=0;i<sectornum;i++)
	{
		W25qxx_Sector_Erase(i);
	}
#ifdef DEBUG
	trace_printf("erase sector success!\n");
#endif
}

//void test_W25qxx()
//{
//	u8 buff[100]={0};
//	u8 temp[]={"W25Q128测试,1234567898555555555555555"};
//	W25qxx_Sector_Erase(0);
//	W25qxx_Write_Check(temp,FLASH_WRProt_Page1,sizeof(temp));
//	W25qxx_Read_Page(buff,FLASH_WRProt_Page1,sizeof(temp));
//
//	//while(1);
//}
