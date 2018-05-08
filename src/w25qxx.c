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

	W25X_CsHigh();  //δѡ��flash

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

//��ȡ״̬�Ĵ���
u8 W25qxx_Read_SR()
{
	u8 temp;
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_ReadStatusRegister);
	temp=SPI2_ReadWrite_Byte(W25X_Read);
	W25X_CsHigh();
	return temp;
}
//д״̬�Ĵ���
void W25qxx_Write_SR(u8 status)
{
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_WriteStatusRegister);
	SPI2_ReadWrite_Byte(status);
	W25X_CsHigh();
}
//��ȡflash ID
u16 W25qxx_ReadID()
{
	u16 Temp=0;
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_ManuDeviceID);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	Temp|=SPI2_ReadWrite_Byte(W25X_Read)<<8;
	Temp|=SPI2_ReadWrite_Byte(W25X_Read);
	W25X_CsHigh();
	return Temp;
}
//дʹ��
void W25qxx_Write_Enable()
{
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_WriteEnable);
	W25X_CsHigh();
}
void W25qxx_Write_Disable()
{
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_WriteDisable);
	W25X_CsHigh();
}
//�ȴ�����
void W25qxx_Wait_Busy()
{
	u8 busyStatus=0x01;
	while((busyStatus&W25qxx_Read_SR())==0x01);
}
//����ģʽ
void W25qxx_Power_Down()
{
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_PowerDown);
	W25X_CsHigh();
}
//�˳�����ģʽ
void W25qxx_Release_Power_Down()
{
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_ReleasePowerDown);
	W25X_CsHigh();
}

/*
��ȡ���ݣ����65536�ֽ�
*pBuffer: �ֽ�ָ��
WriteAddr:��ַ
len:���ݳ���
*/


void W25qxx_Read_Page(u8 *pBuffer,u32 ReadAddr,u16 len)
{
	u32 i;
	W25X_CsLow();  //Ƭѡ
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
��ָ����ַд������   ���256�ֽ�
*pBuffer: ����ָ��
WriteAddr: д��ַ
len �����ݳ���
*/

void W25qxx_Write_Sector(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u16 i;
	W25qxx_Write_Enable();
	W25X_CsLow();  //Ƭѡ
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
��ָ����ַд������   ���256�ֽ�
*pBuffer: ����ָ��
WriteAddr: д��ַ
len �����ݳ���
*/
void W25qxx_Write_Page(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u8 i;
	W25qxx_Write_Enable();
	W25X_CsLow();  //Ƭѡ
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
��ָ����ַд������   ���65535�ֽ�
*pBuffer: ����ָ��
WriteAddr: д��ַ
len �����ݳ���
δ�����߳�����
*/
u16 sector_size=256;
void W25qxx_Write_NoCheck(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u16 secremain;
	secremain=sector_size-WriteAddr%sector_size; //ʣ����ֽڿռ�
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
��ָ����ַд������   ���65535�ֽ�
*pBuffer: ����ָ��
WriteAddr: д��ַ
len �����ݳ���
���Ƚ����߳�����
*/
u16 page_size=4096;
u8 W25qxx_Buffer[4096];  //��ΪҪ��ҳд�룬����Ҫ������һ��sector
void W25qxx_Write_Check(u8 *pBuffer,u32 WriteAddr,u16 len)
{
	u16 secremain;
	u16 secoff;
	u16 secnum;
	u8* W25qxx_Buff;
	u16 i;
	secnum=WriteAddr/page_size;
	secoff=WriteAddr%page_size;
	secremain=page_size-secoff; 	 //ʣ����ֽڿռ�
	W25qxx_Buff=W25qxx_Buffer;
	if(len<=secremain)secremain=len;  //����4096-secoff�ֽ�
	while(1)
	{
		W25qxx_Read_Page(W25qxx_Buff,secnum*page_size,page_size); //�ȶ�ȡflash����
		for(i=0;i<secremain;i++)
		{
			if(W25qxx_Buff[i+secoff]!=0xFF) break; //���i=secremain ����Ҫ����
		}
		if(i<secremain)         //˵���м���һ�����ݲ�Ϊ0xFF
		{
			W25qxx_Sector_Erase(secnum); //�������������ݿ�
			for(i=0;i<secremain;i++)     //����pBuffer
			{
				W25qxx_Buff[i+secoff]=pBuffer[i];
			}
			W25qxx_Write_NoCheck(W25qxx_Buff,secnum*page_size,page_size); //��ҳд��
		}
		else
		{
			W25qxx_Write_NoCheck(pBuffer,WriteAddr,secremain); //����Ҫ������д��
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

//ȫƬ�߳�
void W25qxx_Chip_Erase()
{
	W25qxx_Write_Enable();
	W25qxx_Wait_Busy();
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_ChipErase);
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}
//���߳�
void W25qxx_Block_Erase(u16 SectorNum)
{
	u32 sectoraddr;
	sectoraddr=SectorNum*65536+FLASH_WRProt_Page_START;
	W25qxx_Write_Enable();
	W25qxx_Wait_Busy();
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_BlockErase64);
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>16));
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>8));
	SPI2_ReadWrite_Byte((u8)sectoraddr);
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}
//sector�߳�
void W25qxx_Sector_Erase(u16 SectorNum)
{
	u32 sectoraddr;
	sectoraddr=SectorNum*4096+FLASH_WRProt_Page_START;
	W25qxx_Write_Enable();
	W25qxx_Wait_Busy();
	W25X_CsLow();  //Ƭѡ
	SPI2_ReadWrite_Byte(W25X_SectorErase);
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>16));
	SPI2_ReadWrite_Byte((u8)(sectoraddr>>8));
	SPI2_ReadWrite_Byte((u8)(sectoraddr));
	W25X_CsHigh();
	W25qxx_Wait_Busy();
}

//flash�������ģʽ
void W25qxx_PowerDown()
{
	W25X_CsLow();
	SPI1_ReadWrite_Byte(W25X_PowerDown);
	W25X_CsHigh();
}
//flash����ģʽ
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
//	u8 temp[]={"W25Q128����,1234567898555555555555555"};
//	W25qxx_Sector_Erase(0);
//	W25qxx_Write_Check(temp,FLASH_WRProt_Page1,sizeof(temp));
//	W25qxx_Read_Page(buff,FLASH_WRProt_Page1,sizeof(temp));
//
//	//while(1);
//}
