#ifndef SPI_H_
#define SPI_H_

#include "stm32f10x.h"

void SPI1_Init(void);
void SPI2_Init(void);
u8 SPI1_ReadWrite_Byte(u8 buff);
u8 SPI2_ReadWrite_Byte(u8 buff);
u8 spi1_write_byte(u8 *reg,u16 size);
u8 spi1_read_byte(u8 *pbuf,u16 size);
#endif
