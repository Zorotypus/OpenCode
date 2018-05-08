#ifndef W25QXX_H_
#define W25QXX_H_

#include "stm32f10x.h"

#define W25X_CsHigh()       GPIO_SetBits(GPIOB,GPIO_Pin_12);
#define W25X_CsLow()		GPIO_ResetBits(GPIOB,GPIO_Pin_12);

#define W25Q8 	0XEF13
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

//÷∏¡Ó±Ì
#define W25X_WriteEnable              0x06
#define W25X_WriteDisable 	          0x04
#define W25X_WriteStatusRegister      0x01
#define W25X_ReadStatusRegister       0x05
#define W25X_PageProgram			  0x02
#define W25X_SectorErase			  0x20
#define W25X_BlockErase32		 	  0x52
#define W25X_BlockErase64			  0xD8
#define W25X_ChipErase				  0xC7
#define W25X_ReadData                 0x03
#define W25X_FastRead				  0x0B
#define W25X_PowerDown				  0xB9
#define W25X_ReleasePowerDown		  0xAB
#define W25X_PowerDown				  0xB9
#define W25X_ManuDeviceID			  0x90
#define W25X_Read                     0xFF






void W25qxx_Init(void);
u16 W25qxx_ReadID(void);
void test_W25qxx(void);
void W25qxx_Read_Page(u8 *pBuffer,u32 ReadAddr,u16 len);
void W25qxx_Wait_Busy(void);
void W25qxx_Sector_Erase(u16 SectorNum);
void W25qxx_Write_Check(u8 *pBuffer,u32 WriteAddr,u16 len);
void W25qxx_Write_NoCheck(u8 *pBuffer,u32 WriteAddr,u16 len);
void W25qxx_Write_Page(u8 *pBuffer,u32 WriteAddr,u16 len);
void W25qxx_Write_Sector(u8 *pBuffer,u32 WriteAddr,u16 len);
void W25qxx_Sector_Init(u8 sectornum);

#endif
