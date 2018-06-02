#ifndef __SIM808_H__
#define __SIM808_H__	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-sim808 GSM/GPRS模块驱动	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2016/4/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved	
//********************************************************************************
//无
//////////////////////////////////////////////////////////////////////////////////	

#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8	//高低字节交换宏定义

extern u8 Scan_Wtime;

void sim_at_response(u8 mode);	
u8* sim808_check_cmd(u8 *str);
u8 sim808_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 sim808_wait_request(u8 *request ,u16 waittime);
u8 sim808_chr2hex(u8 chr);
u8 sim808_hex2chr(u8 hex);

#define SIM_OK 0
#define SIM_COMMUNTION_ERR 0xff
#define SIM_CPIN_ERR 0xfe
#define SIM_CREG_FAIL 0xfd
#define SIM_MAKE_CALL_ERR 0Xfc
#define GPS_OK 1
#define GPS_FAIL 0


extern u8 SIM900_CSQ[3];
extern u8 GSM_DectStatus(void);
extern u8 SIM_MAKE_CALL(u8 *number);
extern u8 GPS_Enable(void);
extern u8 SIM808_CONNECT_SERVER_SEND_INFOR(u8 *IP_ADD,u8 *COM);
extern u8 SIM808_GPRS_SEND_DATA(u8 *temp_data);

#endif





