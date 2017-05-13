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

                                                                        /* �������ں�I2C�жϴ�����Ϣ��  */
                                                                        /* ȫ�ֱ���                     */
volatile uint8_t     I2C_sla;                                             /* I2C�����ӵ�ַ                */
volatile uint32_t    I2C_suba;                                            /* I2C�����ڲ��ӵ�ַ            */
volatile uint8_t     I2C_suba_num;                                        /* I2C�ӵ�ַ�ֽ���              */
volatile uint8_t     *I2C_buf;                                            /* ���ݻ�����ָ��               */
volatile uint32_t    I2C_num;                                             /* Ҫ��ȡ/д������ݸ���        */
volatile uint8_t     I2C_end;                                             /* I2C���߽�����־������������  */
                                                                        /* ��1                          */
volatile uint8_t     I2C_suba_en;         /*  �ӵ�ַ���ơ�
                                               0--�ӵ�ַ�Ѿ�������߲���Ҫ�ӵ�ַ
                                               1--��ȡ����
                                               2--д����
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
	case 0x08:		 										/* �ѷ�����ʼ���� */
	if (I2C_suba_en == 1)
	{	LPC_I2C0->I2DAT = I2C_sla &0xfe;	
	}
	else
	{	LPC_I2C0->I2DAT = I2C_sla;	
	}	
	LPC_I2C0->I2CONCLR = (1 << 3)|(1 << 5);
	break;
	
	case 0x10:												/*�ѷ����ظ���ʼ���� */ 
	LPC_I2C0->I2DAT = I2C_sla;
	LPC_I2C0->I2CONCLR = 0x28;
	break;
	
	case 0x18:											/*  �ѽ���ACK ��������*/
	case 0x28:											/* �ѷ���I2DAT�е����ݣ��ѽ���ACK */
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
	
	case 0x40:									 /* �ѷ���SLA+R,�ѽ���ACK */
	if (I2C_num <= 1)
	{	LPC_I2C0->I2CONCLR = 1 << 2;
	}
	else
	{
		LPC_I2C0->I2CONSET = 1 << 2;			/* assert ACK after data is received */
	 }
	LPC_I2C0->I2CONCLR = 0x28;
	break;

	case 0x20:								/* regardless, it's a NACK *//* �ѷ���SLA+W,�ѽ��շ�Ӧ�� */
	case 0x30:	 							/* �ѷ���I2DAT�е����ݣ��ѽ��շ�Ӧ��     */	
	case 0x38:		
	case 0x48:								/* �ѷ���SLA+R,�ѽ��շ�Ӧ��              */
	LPC_I2C0->I2CONCLR = 0x28;
	I2C_end = 0xFF;
	break;
		
	case 0x50:								/* �ѽ��������ֽڣ��ѷ���ACK */
	*I2C_buf++ = LPC_I2C0->I2DAT;
	I2C_num--;
	if (I2C_num ==1)					    /* �������һ���ֽ�             */
	{	LPC_I2C0->I2CONCLR = 0x2c;	   		/* STA,SI,AA = 0                */
	}
	else
	{	LPC_I2C0->I2CONSET = 0x04;			
		LPC_I2C0->I2CONCLR = 0x28;	   
	}
	break;

	case 0x58: 								/* �ѽ��������ֽڣ��ѷ��ط�Ӧ�� */	
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
** �������� ��I2C_WriteNByte()
** �������� �������ӵ�ַ����д��N�ֽ�����
** ��ڲ��� ��	sla			�����ӵ�ַ
**				suba_type	�ӵ�ַ�ṹ	1�����ֽڵ�ַ	3��8+X�ṹ	2��˫�ֽڵ�ַ
**			  	suba		�����ڲ������ַ
**			  	*s			��Ҫд������ݵ�ָ��
**			  	num			��Ҫд������ݵĸ���
** ���ڲ��� ��	1			�����ɹ�
**			  	0			����ʧ��
*********************************************************************************************************
*/
uint8_t I2C_WriteNByte(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
    if (num > 0)                                                /* �����ȡ�ĸ���Ϊ0���򷵻ش�  */
	{
		if (suba_type == 1)
		{
		    I2C_sla         = sla;                                      /* �������Ĵӵ�ַ               */
		    I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
		    I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�			*/
	   	}
		if (suba_type == 2)
		{
		    I2C_sla         = sla;                                      /* �������Ĵӵ�ַ               */
		    I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
		    I2C_suba_num    = 2;                                        /* �����ӵ�ַΪ1�ֽ�			*/
	   	}
		if (suba_type == 3)
		{
		    I2C_sla         = sla + ((suba >> 7 )& 0x0e);                                      /* �������Ĵӵ�ַ               */
		    I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
		    I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�			*/
	   	}

	    I2C_buf     	= s;                                                /* ����                         */
	    I2C_num     	= num;                                              /* ���ݸ���                     */
	    I2C_suba_en 	= 2;                                                /* ���ӵ�ַ��д����             */
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
** �������� ��I2C_ReadNByte()
** �������� �������ӵ�ַ���������ַ��ʼ��ȡN�ֽ�����
** ��ڲ��� ��	sla			�����ӵ�ַ
**				suba_type	�ӵ�ַ�ṹ	1�����ֽڵ�ַ	2��8+X�ṹ	2��˫�ֽڵ�ַ
**				suba		�����ӵ�ַ
**				s			���ݽ��ջ�����ָ��
**				num			��ȡ�ĸ���
** ���ڲ��� ��	1			�����ɹ�
**				0			����ʧ��
*********************************************************************************************************
*/
uint8_t I2C_ReadNByte (uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
	if (num > 0)
	{
		if (suba_type == 1)
		{
			I2C_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
			I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
			I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
		}
		if (suba_type == 2)
		{
			I2C_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
			I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
			I2C_suba_num    = 2;                                        /* �����ӵ�ַΪ1�ֽ�            */
		}
		if (suba_type == 3)
		{
			I2C_sla         = sla + ((suba >> 7 )& 0x0e) + 1;            /* �������Ĵӵ�ַ��R=1          */
			I2C_suba        = suba & 0x0ff;                              /* �����ӵ�ַ                   */
			I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
		}



	I2C_buf     	= s;                                                /* ���ݽ��ջ�����ָ��           */
	I2C_num     	= num;                                              /* Ҫ��ȡ�ĸ���                 */
	I2C_suba_en	 	= 1;                                                /* ���ӵ�ַ��                   */
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

