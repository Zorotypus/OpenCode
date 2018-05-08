#ifndef COMMAND_H_
#define COMMAND_H_

#include "stm32f10x.h"


extern u16 datablock;

#define FLASH_WRProt_Page_START         ((uint32_t)0x00000000)	//flash开始地址
#define Flash_KX122_WRITE_ADDR         	((uint32_t)0x00001000)	//数据采集地址
#define cmd_addr			       		((uint32_t)0x00000100)	//命令写入地址
#define CMD_RX_LEN	512

#define CMD_LORA_LORA_CONF		"AT+CMDGET_LORA_CONF"
#define CMD_LORA_START_SAMPLE	"AT+CMDGET_SAMPLE_START"
#define	CMD_LORA_STOP_SAMPLE	"AT+CMDGET_SAMPLE_STOP"
#define CMD_LORA_DEST_CONFIG	"AT+CMDGET_DEST_ADDR"
#define CMD_LORA_LOCAL_CONFIG	"AT+CMDGET_LOCAL_ADDR"
#define CMD_LORAID				"AT+CMDGET_LORAID"
#define CMD_LORA_AWAKE			"AT+CMDGET_LORA_AWAKE"
typedef struct
{
	u16 blockval;
	u16 cmd_len;
	u8 cmd_buff[CMD_RX_LEN];
	u16 samplerate;
	u32 samplepoint;
}command_line;

command_line command;

u16 Daq_Stop(u16 block);
void Daq_Transmit(u16 block,u16 samplepoint);
void handle_recv_cmd();
void clear_cmd_buff();
#endif
