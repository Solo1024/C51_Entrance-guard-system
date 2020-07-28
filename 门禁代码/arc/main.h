#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

/*
**********************************************************************
*                         头文件包含
**********************************************************************
*/
#include "common.h"


/*
**********************************************************************
*                         本地宏定义
**********************************************************************
*/
#define  OSC_FREQ        	(22118400L)
#define  RCAP2_50us      	(65536L - OSC_FREQ/40417L)
#define  RCAP2_1ms       	(65536L - OSC_FREQ/2000L)
#define  RCAP2_10ms      	(65536L - OSC_FREQ/1200L)
#define  TIME0_500us     	(65536L - OSC_FREQ/8000L)
#define  TIME0_10ms      	(65536L - OSC_FREQ/200)
#define  CALL_isr_UART() 	(TI = 1)
#define  TRUE 				1
#define  FALSE 				0

/*
**********************************************************************
*                         外部函数原型声明
**********************************************************************
*/
void SysInit(void);
void BuzzerOnce(void);
void delay1ms(unsigned int i);
void UartSendBytes(u8 *c);
void UartSendByte(u8 c);
void delay10us(u8 i);

#endif