#include "sim808.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "string.h"    
#include "usart2.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-sim808C GSM/GPRS模块驱动	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2016/4/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved	
//********************************************************************************
//无

u8 Scan_Wtime = 0;//保存扫描需要的时间
u8 BT_Scan_mode=0;//蓝牙扫描设备模式标志

u8 SIM900_CSQ[3];
u8 dtbuf[50];   								//打印缓存器	

//usmart支持部分 
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//     1,清零USART2_RX_STA;
void sim_at_response(u8 mode)
{
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",USART2_RX_BUF);	//发送到串口
		if(mode)USART2_RX_STA=0;
		
	} 
}
//////////////////////////////////////////////////////////////////////////////////
//ATK-sim808C 各项测试(拨号测试、短信测试、GPRS测试、蓝牙测试)共用代码

//sim808C发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* sim808c_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向sim808C发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim808_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);//发送命令
	
	if(waittime==1100)//11s后读回串口数据(蓝牙扫描模式)
	{
		 Scan_Wtime = 11;  //需要定时的时间
		 TIM4_SetARR(9999);//产生1S定时中断
	}
	
	
	if(ack&&waittime)		//需要等待应答
	{ 
	   while(--waittime)	//等待倒计时
	   {
		   if(BT_Scan_mode)//蓝牙扫描模式
		   {
			   res=KEY_Scan(0);//返回上一级
			   if(res==WKUP_PRES)return 2;
		   }
		   delay_ms(10);
		   if(USART2_RX_STA&0X8000)//接收到期待的应答结果
		   {
			   if(sim808c_check_cmd(ack))break;//得到有效数据 
			   USART2_RX_STA=0;
		   } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//接收sim808C返回数据（蓝牙测试模式下使用）
//request:期待接收命令字符串
//waittimg:等待时间(单位：10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim808c_wait_request(u8 *request ,u16 waittime)
{
	 u8 res = 1;
	 u8 key;
	 if(request && waittime)
	 {
		while(--waittime)
		{   
		   key=KEY_Scan(0);
		   if(key==WKUP_PRES) return 2;//返回上一级
		   delay_ms(10);
		   if(USART2_RX_STA &0x8000)//接收到期待的应答结果
		   {
			  if(sim808c_check_cmd(request)) break;//得到有效数据
			  USART2_RX_STA=0;
		   }
		}
		if(waittime==0)res=0;
	 }
	 return res;
}

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim808c_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim808c_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}


u8 sim900a_work_test(void)
{
	if(sim808_send_cmd((u8 *)"AT",(u8 *)"OK",100))
	{
		if(sim808_send_cmd((u8 *)"AT",(u8 *)"OK",100))return SIM_COMMUNTION_ERR;	//通信不上
	}		
	if(sim808_send_cmd((u8 *)"AT+CPIN?",(u8 *)"READY",400))return SIM_CPIN_ERR;	//没有SIM卡
	if(sim808_send_cmd((u8 *)"AT+CREG?",(u8 *)"0,1",400))
	{
		if(strstr((const char*)USART2_RX_BUF,"0,5")==NULL)
		{
			 if(!sim808_send_cmd((u8 *)"AT+CSQ",(u8 *)"OK",200))	
			 {
					memcpy(SIM900_CSQ,USART2_RX_BUF+15,2);
			 }
			 return SIM_CREG_FAIL;	//等待附着到网络
		}
	}
	printf("OKKKKKK\r\n");
	return SIM_OK;
}

u8 GSM_DectStatus(void)
{
	u8 res;
	res=sim900a_work_test();	
	switch(res)
	{
		case SIM_OK:
			printf("GSM模块自检成功\r\n");
			break;
		case SIM_COMMUNTION_ERR:
			printf("与GSM模块未通讯成功，请等待\r\n");
			break;
		case SIM_CPIN_ERR:
			printf("没检测到SIM卡\r\n");	
			break;
		case SIM_CREG_FAIL:
			printf("注册网络中。。。\r\n");	
			printf("当前信号值：%s\r\n", SIM900_CSQ);	
			break;		
		default:
			break;
	}
	return res;
}
/**
* 拨打电话
*/
u8 SIM_MAKE_CALL(u8 *number)
{
	u8 cmd[20];
	sprintf((char*)cmd,"ATD%s;",number);
	if(sim808_send_cmd(cmd,(u8 *)"OK",200))	return SIM_MAKE_CALL_ERR;
	return SIM_OK;
}

/**
* GPS测试
*/
u8 GPS_Enable(void)
{
	if(sim808_send_cmd("AT+CGPSPWR=1","OK",200)) //打开GPS电源
	{
		printf("open GPS Power Fail\n");
		return GPS_FAIL;
	}
	if(sim808_send_cmd("AT+CGNSTST=1","OK",200)) //打开NMEA数据输出
	{
		printf("Open NMEA Data Output Fail\n");
		return GPS_FAIL;
	}
	return GPS_OK;
}

u8 SIM808_CONNECT_SERVER(u8 *IP_ADD,u8 *COM)
{		
		if(sim808_send_cmd((u8 *)"AT+CGATT?",(u8 *)": 1",100))	 return 1;
		//if(sim900a_send_cmd((u8 *)"AT+CIPHEAD=1",(u8 *)"OK",100))	 return 7;
	  if(sim808_send_cmd((u8 *)"AT+CIPSHUT",(u8 *)"OK",500))	return 2;
		if(sim808_send_cmd((u8 *)"AT+CSTT",(u8 *)"OK",200))	return 3;
		if(sim808_send_cmd((u8 *)"AT+CIICR",(u8 *)"OK",600))	return 4;
		if(!sim808_send_cmd((u8 *)"AT+CIFSR",(u8 *)"ERROR",200))	return 5;		
		sprintf((char*)dtbuf,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",IP_ADD,COM);
	  if(sim808_send_cmd((u8 *)dtbuf,(u8 *)"CONNECT OK",200))	return 6;		
	  return 0;
}	
u8 SIM808_CONNECT_SERVER_SEND_INFOR(u8 *IP_ADD,u8 *COM)
{
	u8 res;	
	res=SIM808_CONNECT_SERVER(IP_ADD,COM);
	switch(res)
	{
		case 0:
			printf("服务器连接成功\n");
			break;
		case 1:
			printf("等待GSM模块附着网络\n");
		  break;
		case 2:
			printf("场景关闭失败\n");	
			break;
		case 3:
			printf("CSTT失败\n");	
			break;
		case 4:
			printf("CIICR失败\n");	
			break;
		case 5:
			printf("CIFSR失败\n");	
			break;
		case 6:
			printf("连接服务器失败\n");	
			break;
		default:
			break;
	}
	return res;
}
u8 SIM808_GPRS_SEND_DATA(u8 *temp_data)
{		
	 //UART3SendString("启动数据发送，数据内容：",strlen("启动数据发送，数据内容："));	
	 if(sim808_send_cmd("AT+CIPSEND",">",100))	 return 1;
	 //UART3SendString((u8*)temp_data,strlen((u8*)temp_data));	UART3SendString("\r\n",2);
	 if(sim808_send_cmd(temp_data,NULL,0))	return 2;
	 if(sim808_send_cmd((u8 *)0x1a,(u8 *)"SEND OK",1500))	return 3;		
	 //UART3SendString("数据发送成功",strlen("数据发送成功"));	UART3SendString("\r\n",2);
	 return 0;
}	

