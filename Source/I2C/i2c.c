/*****************************************************************************
 *   i2c.c:  I2C C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "lpc17xx.h"
#include "i2c.h"

uint8_t buf[32];

                                                                        /* 定义用于和I2C中断传递信息的  */
                                                                        /* 全局变量                     */
volatile uint8_t     I2C_sla;                                             /* I2C器件从地址                */
volatile uint32_t    I2C_suba;                                            /* I2C器件内部子地址            */
volatile uint8_t     I2C_suba_num;                                        /* I2C子地址字节数              */
volatile uint8_t     *I2C_buf;                                            /* 数据缓冲区指针               */
volatile uint32_t    I2C_num;                                             /* 要读取/写入的数据个数        */
volatile uint8_t     I2C_end;                                             /* I2C总线结束标志：结束总线是  */
                                                                        /* 置1                          */
volatile uint8_t     I2C_suba_en;         /*  子地址控制。
                                               0--子地址已经处理或者不需要子地址
                                               1--读取操作
                                               2--写操作
                                        */
/* 
From device to device, the I2C communication protocol may vary, 
in the example below, the protocol uses repeated start to read data from or 
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO 
for master write: the sequence is: STA,Addr(W),length,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could 
be READ or WRITE depending on the I2CCmd.
*/   
/*******************************************************************************
**   Dealy Function  Delay()
*******************************************************************************/
void Delay(uint32_t delaydata)
{
	uint32_t i,j,k;
	for(i=0;i<delaydata;i++)
		for(j=0;j<1000;j++)
			for(k=0;k<100;k++)
			;
}
/*****************************************************************************
** Function name:		I2C0_IRQHandler
**
** Descriptions:		I2C0 interrupt handler, deal with master mode
**						only.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void I2C0_IRQHandler(void)  
{
  uint8_t StatValue;
  StatValue = LPC_I2C0->I2STAT & 0xf8;
	
  switch ( StatValue )
  {
	case 0x08:		 										/* 已发送起始条件 */
	if (I2C_suba_en == 1)
	{	LPC_I2C0->I2DAT = I2C_sla &0xfe;	
	}
	else
	{	LPC_I2C0->I2DAT = I2C_sla;	
	}	
	LPC_I2C0->I2CONCLR = (1 << 3)|(1 << 5);
	break;
	
	case 0x10:												/*已发送重复起始条件 */ 
	LPC_I2C0->I2DAT = I2C_sla;
	LPC_I2C0->I2CONCLR = 0x28;
	break;
	
	case 0x18:											/*  已接收ACK 发送数据*/
	case 0x28:											/* 已发送I2DAT中的数据，已接收ACK */
	if (I2C_suba_en == 0)
	{
	  
	  if(I2C_num>0)
	  {  
		  LPC_I2C0->I2DAT = *I2C_buf++; 			
		  LPC_I2C0->I2CONCLR = 0x28;
		  I2C_num--;
		  Delay(1);
	  }
	  else
	  {
		LPC_I2C0->I2CONSET = (1 << 4); 				
		LPC_I2C0->I2CONCLR = 0x28;  				
		I2C_end =1;
	  }
		
	}
	if(I2C_suba_en == 1)
	{
		if(I2C_suba_num == 2)
		{
		  LPC_I2C0->I2DAT = ((I2C_suba >> 8) & 0xff);
		  LPC_I2C0->I2CONCLR = 0x28;
		  I2C_suba_num--; 
		  break;
		 }
		if(I2C_suba_num == 1)
		{
		  LPC_I2C0->I2DAT = (I2C_suba & 0xff);
		  LPC_I2C0->I2CONCLR = 0x28;
		  I2C_suba_num--; 
		  break;
		 }
		 if(I2C_suba_num == 0)
		 {
		  LPC_I2C0->I2CONCLR = 0x08;
		  LPC_I2C0->I2CONSET = 0x20;
		  I2C_suba_en = 0;
		  break;		  		 
		 }   	
	}

	if ( I2C_suba_en == 2 )
	{
	  if(I2C_suba_num > 0)
	  {
		  if(I2C_suba_num == 2)
		  {
			  LPC_I2C0->I2DAT = ((I2C_suba >> 8) & 0xff);
			  LPC_I2C0->I2CONCLR = 0x28;
			  I2C_suba_num--; 
			  break;
		  }
		  if(I2C_suba_num == 1)
		  {
			  LPC_I2C0->I2DAT = (I2C_suba & 0xff);
			  LPC_I2C0->I2CONCLR = 0x28;
			  I2C_suba_num--; 
			  I2C_suba_en  = 0;
			  break;
		  }
	  }
	}
	break;
	
	case 0x40:									 /* 已发送SLA+R,已接收ACK */
	if (I2C_num <= 1)
	{	LPC_I2C0->I2CONCLR = 1 << 2;
	}
	else
	{
		LPC_I2C0->I2CONSET = 1 << 2;			/* assert ACK after data is received */
	 }
	LPC_I2C0->I2CONCLR = 0x28;
	break;

	case 0x20:								/* regardless, it's a NACK *//* 已发送SLA+W,已接收非应答 */
	case 0x30:	 							/* 已发送I2DAT中的数据，已接收非应答     */	
	case 0x38:		
	case 0x48:								/* 已发送SLA+R,已接收非应答              */
	LPC_I2C0->I2CONCLR = 0x28;
	I2C_end = 0xFF;
	break;
		
	case 0x50:								/* 已接收数据字节，已返回ACK */
	*I2C_buf++ = LPC_I2C0->I2DAT;
	I2C_num--;
	if (I2C_num ==1)					    /* 接收最后一个字节             */
	{	LPC_I2C0->I2CONCLR = 0x2c;	   		/* STA,SI,AA = 0                */
	}
	else
	{	LPC_I2C0->I2CONSET = 0x04;			
		LPC_I2C0->I2CONCLR = 0x28;	   
	}
	break;

	case 0x58: 								/* 已接收数据字节，已返回非应答 */	
	*I2C_buf++ = LPC_I2C0->I2DAT;
	LPC_I2C0->I2CONSET = 0X10;				/* assert ACK after data is received */
	LPC_I2C0->I2CONCLR = 0X28;
	I2C_end = 1;
	break;

	default:
	break;
  }

}
/*****************************************************************************
** Function name:		I2CInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
** 
*****************************************************************************/
uint32_t I2CInit( uint32_t I2cMode ) 
{
  LPC_SC->PCONP |= (1 << 19);

  /* set PIO0.27 and PIO0.28 to I2C0 SDA and SCK */
  /* function to 01 on both SDA and SCK. */
  LPC_PINCON->PINSEL1 &= ~0x03C00000;
  LPC_PINCON->PINSEL1 |= 0x01400000;	
 
  /*--- Reset registers ---*/
  LPC_I2C0->I2SCLL   = I2SCLL_SCLL;
  LPC_I2C0->I2SCLH   = I2SCLH_SCLH;
  if ( I2cMode == I2CSLAVE )
  {
	LPC_I2C0->I2ADR0 = 0xA0;
  }    

  /* Install interrupt handler */
  NVIC_EnableIRQ(I2C0_IRQn);

  LPC_I2C0->I2CONSET = I2CONSET_I2EN;
  return( 1 );
}

/*
*********************************************************************************************************
** 函数名称 ：I2C_WriteNByte()
** 函数功能 ：向有子地址器件写入N字节数据
** 入口参数 ：	sla			器件从地址
**				suba_type	子地址结构	1－单字节地址	3－8+X结构	2－双字节地址
**			  	suba		器件内部物理地址
**			  	*s			将要写入的数据的指针
**			  	num			将要写入的数据的个数
** 出口参数 ：	1			操作成功
**			  	0			操作失败
*********************************************************************************************************
*/
uint8_t I2C_WriteNByte(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
    if (num > 0)                                                /* 如果读取的个数为0，则返回错  */
	{
		if (suba_type == 1)
		{
		    I2C_sla         = sla;                                      /* 读器件的从地址               */
		    I2C_suba        = suba;                                     /* 器件子地址                   */
		    I2C_suba_num    = 1;                                        /* 器件子地址为1字节			*/
	   	}
		if (suba_type == 2)
		{
		    I2C_sla         = sla;                                      /* 读器件的从地址               */
		    I2C_suba        = suba;                                     /* 器件子地址                   */
		    I2C_suba_num    = 2;                                        /* 器件子地址为1字节			*/
	   	}
		if (suba_type == 3)
		{
		    I2C_sla         = sla + ((suba >> 7 )& 0x0e);                                      /* 读器件的从地址               */
		    I2C_suba        = suba;                                     /* 器件子地址                   */
		    I2C_suba_num    = 1;                                        /* 器件子地址为1字节			*/
	   	}

	    I2C_buf     	= s;                                                /* 数据                         */
	    I2C_num     	= num;                                              /* 数据个数                     */
	    I2C_suba_en 	= 2;                                                /* 有子地址，写操作             */
	    I2C_end     	= 0;
  LPC_I2C0->I2CONCLR = (1 << 2)|
  					   (1 << 3)|
					   (1 << 5);  /* Clear SI flag */

  LPC_I2C0->I2CONSET = (1 << 5)|
  					   (1 << 6);	/* Set Start flag */

	while(I2C_end == 0);
		if(I2C_end)
		{
 			Delay(20);
			return 1;
		}
		else
		{
			Delay(20);
			return 0;
		}
  }
  Delay(20);
  return 0;
}
/*
*********************************************************************************************************
** 函数名称 ：I2C_ReadNByte()
** 函数功能 ：从有子地址器件任意地址开始读取N字节数据
** 入口参数 ：	sla			器件从地址
**				suba_type	子地址结构	1－单字节地址	2－8+X结构	2－双字节地址
**				suba		器件子地址
**				s			数据接收缓冲区指针
**				num			读取的个数
** 出口参数 ：	1			操作成功
**				0			操作失败
*********************************************************************************************************
*/
uint8_t I2C_ReadNByte (uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
	if (num > 0)
	{
		if (suba_type == 1)
		{
			I2C_sla         = sla + 1;                                  /* 读器件的从地址，R=1          */
			I2C_suba        = suba;                                     /* 器件子地址                   */
			I2C_suba_num    = 1;                                        /* 器件子地址为1字节            */
		}
		if (suba_type == 2)
		{
			I2C_sla         = sla + 1;                                  /* 读器件的从地址，R=1          */
			I2C_suba        = suba;                                     /* 器件子地址                   */
			I2C_suba_num    = 2;                                        /* 器件子地址为1字节            */
		}
		if (suba_type == 3)
		{
			I2C_sla         = sla + ((suba >> 7 )& 0x0e) + 1;            /* 读器件的从地址，R=1          */
			I2C_suba        = suba & 0x0ff;                              /* 器件子地址                   */
			I2C_suba_num    = 1;                                        /* 器件子地址为1字节            */
		}



	I2C_buf     	= s;                                                /* 数据接收缓冲区指针           */
	I2C_num     	= num;                                              /* 要读取的个数                 */
	I2C_suba_en	 	= 1;                                                /* 有子地址读                   */
	I2C_end     	= 0;
	
  LPC_I2C0->I2CONCLR = (1 << 2)|
  					   (1 << 3)|
					   (1 << 5);  /* Clear SI flag */

  LPC_I2C0->I2CONSET = (1 << 5)|
  					   (1 << 6);	/* Set Start flag */
	
	while(I2C_end == 0);
		if(I2C_end)
		{
 			Delay(20);
			return 1;
		}
		else
		{
			Delay(20);
			return 0;
		}

	}
	Delay(20);
	return 0;
}
/******************************************************************************
**                            End Of File
******************************************************************************/

