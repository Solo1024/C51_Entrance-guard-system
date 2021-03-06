/*********************************************************************
			
**********************************************************************
实验序列：
实验任务: 	
实现现象：
硬件接线：
注意事项：(1) 板载晶振为12MHz，波特率为2400、4800、7200均可找到误差不太
			  大的设置值，而有些波特率如9600则无法找到误差小的设置值，
			  因此不要设置9600这类波特率。
		  (2) 整个串口的代码和波特率计算，都可以由单片机小精灵工具自动
		      计算和生成，但是自己要理解计算过程，要能结合数据手册看懂
			  代码是怎么回事，不能只知其然不知其所以然
		  (3) 注意上位机串口助手的设置参考本文件夹中图“8.1.串口助手设置.png”
		  (4) 因为上位机中串口不能被2个程序同时打开，因此在stcisp软件下载
		      程序前必须将串口助手中串口关闭，否则一旦串口助手中打开串口
			  情况下进行下载，则stcisp软件会提示下载串口打开失败
		  
版    本：
作    者：陈步步
时	  间：										  
**********************************************************************/


/*
**********************************************************************
*                         头文件包含
**********************************************************************
*/
#include <reg52.h>
#include "main.h"
#include "rc522.h"
#include<string.h>

sbit gBuzzer=P3^3;//无源蜂鸣器
sbit PWM=P3^2;	 //舵机

/*
**********************************************************************
*                         本地全局变量
**********************************************************************
*/
	
u8 UID[5],Temp[4];
u16 count = 0, time=0;
u8 UI[5]={0XCA,0xEF,0x7C,0x29 };	   //E	此处为6张卡的信息
u8 UI1[5]={0XCA,0X6F,0X3C,0X29}; //A
u8 UI2[5]={0x51,0x7E,0xC9,0xCF}; //B		  
u8 UI3[5]={0x31,0x11,0x1B,0xD0}; //C
u8 UI4[5]={0x8C,0xF3,0xDA,0x0B}; //D
u8 UI5[5]={0x19,0x74,0xD9,0x59}; //E
void Rc522Test1(void);
void Rc522Test2(void);


/*********************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 参数列表       : 无
* 函数输出    	 : 无
*********************************************************************/
void init0()
{
	TMOD = 0x01; 	// 方式一
	TH0 = 0X0FF;
	TL0 = 0XA3;
	TR0 = 1;       // 开启定时器0
	EA = 1;		   // 开启总中断
	ET0 = 1;       // 定时器0允许中断
}
//舵机
void T0_inter() interrupt 1
{
	TR0 = 0;
	TH0 = 0XFF;
	TL0 = 0XA3;
	TR0 = 1;
	

	time ++ ;
	if (time < count)	
	{
		PWM = 1;
	}
	else
	{
		PWM = 0;
	}
	if (time >= 200)
	{
		time = 0;
	}
}
void main(void)
{
	gBuzzer=0;
    init0();

	
	SysInit();
	Rc522Test1(); //最终运行代码
//	Rc522Test2(); //读取卡片数据，视情况test1和test2选一
	
}


/*********************************************************************
* 函 数 名       : BuzzerOnce
* 函数功能		 : 蜂鸣器叫1下表示寻卡成功
* 参数列表       : 无
* 函数输出    	 : 无
*********************************************************************/
void BuzzerOnce(void)
{
	u8 a,b,i;
	for(i=2;i>0;i--){
		for(b=50;b>0;b--){
        	    for(a=2;a>0;a--){
					gBuzzer = 0;
					delay10us(10);
					gBuzzer = 1;
					delay10us(10);
				}
	   }
	}
}

/*********************************************************************
* 函 数 名       : delay1ms
* 函数功能		 : 延时
* 参数列表       : 无
* 函数输出    	 : 无
*********************************************************************/
void delay1ms(unsigned int i)   //误差 0us,括号内填什么数字就是多少毫秒
{
    unsigned char a,b,c;
	for(;i>0;i--)
 	   for(c=1;c>0;c--)
    	    for(b=142;b>0;b--)
        	    for(a=2;a>0;a--);
}


/*********************************************************************
* 函 数 名       : delay10us
* 函数功能		 : 延时
* 参数列表       : 无
* 函数输出    	 : 无
*********************************************************************/
void delay10us(unsigned char i)   //误差 0us,括号内填什么数字就是多少个十微秒
{
    unsigned char a,b;
	for(;i>0;i--)
    	for(b=1;b>0;b--)
        	for(a=2;a>0;a--);
}

/*********************************************************************
* 函 数 名       : UartSendBytes
* 函数功能		 : 通过串口往外发送数据
* 参数列表       : unsigned char *c,要发的数据
* 函数输出    	 : 无
*********************************************************************/

void UartSendByte(u8 c)	//从串口发送1字节数据
{
	u8 i = 0;
	EA = 0;

	SBUF = c;
	while (!TI);
	TI = 0;

	EA = 1;
}

/*********************************************************************
* 函 数 名       : Rc522Test
* 函数功能		 : test1为运行代码，test2用于读取卡片id
* 参数列表       : 无
* 函数输出    	 : 无
*********************************************************************/
//运行代码,正确的卡号响一次，其他卡响三次
void Rc522Test1(void)
{
   while(1)
   {
	   		
   	   if (PcdRequest(0x52, Temp) == MI_OK)
	   {
	   		if (PcdAnticoll(UID) == MI_OK)
			{
			//此处为判断是否为代码内有标注的卡序列号，除非这五张卡，否则不执行判断
				if(strcmp(UID,UI)==0||strcmp(UID,UI4)==0||strcmp(UID,UI1)==0||strcmp(UID,UI2)==0||strcmp(UID,UI5)==0)
				{
	 				
					BuzzerOnce();
					delay1ms(50);
					BuzzerOnce();
					count = 3;						  //舵机转动
					delay1ms(1500);			
					count=18;						   //舵机转回去

				}
				else
				{  
					BuzzerOnce();
				   delay1ms(30);
					BuzzerOnce();
				   delay1ms(30);
					BuzzerOnce() ;
				   delay1ms(30);
					BuzzerOnce();
				   delay1ms(30);
				}	
			}
	   }
   }
}
//读取卡数据
void Rc522Test2(void)
{
   while(1)
   {
   	   if (PcdRequest(0x52, Temp) == MI_OK)
	   {
	   		if (PcdAnticoll(UID) == MI_OK)
			{
				
	 			UartSendByte(UID[0]);		//读取卡序列号前四位
				UartSendByte(UID[1]);
				UartSendByte(UID[2]);
				UartSendByte(UID[3]);
				delay1ms(500);
				delay1ms(500);										
			}
	   }
   }
}

/*********************************************************************
* 函 数 名       : SysInit
* 函数功能		 : 串口初始化为2400波特率，寻卡，读写卡数据
* 参数列表       : 无
* 函数输出    	 : 无
*********************************************************************/
void SysInit(void)
{
    TMOD = 0x21;		// T1设置为8位自动重装载定时器			
    SCON = 0x40;		// 串口工作在模式1：8位UART波特率可变，且禁止接收
    TH1 = 0xE8;			// 单片机小精灵V1.3算出的2400波特率且波特率
    TL1 = TH1;			// 加倍时的定时器设置值。
    PCON = 0x80;		// 设置为波特率加倍
	EA = 1;				// 开总中断
	ES = 1;			    // 开串口中断
	TR1 = 1;		    // 定时器1开启计数

	gBuzzer = 0;
    PcdReset();				//复位RC522
    PcdAntennaOff(); 	 	//关天线 		
    PcdAntennaOn();  		//开天线
	M500PcdConfigISOType('A');
}