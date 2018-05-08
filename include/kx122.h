#ifndef KX122_H_
#define KX122_H_

//数据寄存器
#define KX122_XOUT_L  0x06
#define KX122_XOUT_H  0x07
#define KX122_YOUT_L  0x08
#define KX122_YOUT_H  0x09
#define KX122_ZOUT_L  0x0A
#define KX122_ZOUT_H  0x0B
#define KX122_COTR    0X0C
//ID
#define KX122_ID	 	0X0F
//中断源寄存器
#define KX122_TSCP	 	0X10
#define KX122_TSPP	 	0X11
#define KX122_INS1	 	0X12
#define KX122_INS2	 	0X13
#define KX122_INS3	 	0X14
#define KX122_INT_REL	 		0X17
//状态寄存器
#define KX122_STATUS_REG	 	0X15
//数据输出速率寄存器
#define KX122_CNTL1	 	0X18
#define KX122_CNTL2	 	0X19
#define KX122_CNTL3	 	0X1A
#define KX122_ODCNTL	0X1B
#define KX122_INC1	 	0X1C
#define KX122_INC2	 	0X1D
#define KX122_INC3	 	0X1E
#define KX122_INC4	 	0X1F
#define KX122_INC5	 	0X20
#define KX122_INC6	 	0X21
#define KX122_BUF_CNTL1	 	0X3A
#define KX122_BUF_CNTL2	 	0X3B
#define KX122_BUF_STATUS1	 	0X3C
#define KX122_BUF_STATUS2	 	0X3D
#define KX122_BUF_CLEAR	 	0X3E
#define KX122_BUF_READ	 	0X3F

#define Kx122_CsHigh()       GPIO_SetBits(GPIOA,GPIO_Pin_4);
#define Kx122_CsLow()		GPIO_ResetBits(GPIOA,GPIO_Pin_4);



void Kx122_Init();
void Kx122_Config(int baud);
u8 Kx122_Read_Register(u8 reg);
void Kx122_Write_Register(u8 reg,u8 temp);
void Kx122_Read_ID();
void Sample_Rate(int baud);
void Kx122_Get_XYZ(u8 *pBuffer,u16 len);
void Kx122_Read_Buff(u8 *pBuf,u16 len);
void Kx122_Start_Measure();
void Kx122_Stop_Measure();
void kx122_data_check();
#endif
