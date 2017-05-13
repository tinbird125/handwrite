/****************************************Copyright (c)****************************************************
**                                 http://www.OpenMCU.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           uart.c
** Last modified Date:  2014-04-10
** Last Version:        V1.1
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Created by:          OpenMCU
** Created date:        2010-05-10
** Version:             V1.07
** Descriptions:        ��дʾ������
**
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "lpc17xx.h"                                   /* LPC17xx definitions    */
#include "uart.h"

#define UART_LOAD_DLL(div)	((div) & 0xFF)	
#define UART_DLL_MASKBIT	((uint8_t)0xFF)	

#define UART_DLM_MASKBIT	((uint8_t)0xFF)			
#define UART_LOAD_DLM(div)  (((div) >> 8) & 0xFF)	

#define UART_LCR_WLEN5     		((uint8_t)(0))   		
#define UART_LCR_WLEN6     		((uint8_t)(1<<0))   	
#define UART_LCR_WLEN7     		((uint8_t)(2<<0))   	
#define UART_LCR_WLEN8     		((uint8_t)(3<<0))   	
#define UART_LCR_STOPBIT_SEL	((uint8_t)(1<<2))   	
#define UART_LCR_PARITY_EN		((uint8_t)(1<<3))		
#define UART_LCR_PARITY_ODD		((uint8_t)(0<<4))      
#define UART_LCR_PARITY_EVEN	((uint8_t)(1<<4))		
#define UART_LCR_PARITY_F_1		((uint8_t)(2<<4))		
#define UART_LCR_PARITY_F_0		((uint8_t)(3<<4))		
#define UART_LCR_BREAK_EN			((uint8_t)(1<<6))		
#define UART_LCR_DLAB_EN			((uint8_t)(1<<7))    	
#define UART_LCR_BITMASK			((uint8_t)(0xFF))		

#define UART_FDR_DIVADDVAL(n)	((uint32_t)(n&0x0F))		
#define UART_FDR_MULVAL(n)		((uint32_t)((n<<4)&0xF0))	
#define UART_FDR_BITMASK			((uint32_t)(0xFF))			

#define UART_ACCEPTED_BAUDRATE_ERROR	(3)			




void UARTInit( uint32_t PortNum, uint32_t baudrate )
{
  uint32_t Fdiv;

  if ( PortNum == 0 )
  {
    LPC_PINCON->PINSEL0 |= (1 << 4);             /* Pin P0.2 -- TXD0 (Com0) */
    LPC_PINCON->PINSEL0 |= (1 << 6);             /* Pin P0.3 -- RXD0 (Com0) */
            LPC_UART0->LCR  = 0x83;                       /* 8 bits, ��У��, 1 ��ֹͣλ */
    Fdiv = (SystemCoreClock/4/ 16) / baudrate;/* ���ò����� */
    LPC_UART0->DLM  = Fdiv / 256;
    LPC_UART0->DLL  = Fdiv % 256+1;
   LPC_UART0->LCR  = 0x03;                       /* ����������  */
    LPC_UART0->FCR  = 0x07;                       /* �����Ƚ��ȳ����ƼĴ�������*/

  }
  else if ( PortNum == 1 )
  {
               LPC_PINCON->PINSEL4 |= (1<<1);                /* Pin P2.0 -- TXD1 (Com1)*/
          LPC_PINCON->PINSEL4 |= (1<<3);                /* Pin P2.1 -- RXD1 (Com1)*/
   
               LPC_UART1->LCR = 0x83;                                  /* 8 bits, ��У��, 1 ��ֹͣλ */
    Fdiv = (SystemCoreClock/4/16)/ baudrate ; /* ���ò���, Fpclk: 18MHz */
    LPC_UART1->DLM = Fdiv/256;                                                                                             
    LPC_UART1->DLL = (Fdiv%256)+1;
               LPC_UART1->LCR = 0x03;                                   /* DLAB = 0 */
    LPC_UART1->FCR = 0x07;                             /* ʹ��TX�� RX ����ʼ��FIFO. */

  }
}




int uart0_set_divisors(uint8_t uartNum, uint32_t uClk, uint32_t baudrate)
{
	int errorStatus = 0;
	uint32_t calcBaudrate = 0;							
	uint32_t temp = 0;								

	uint32_t mulFracDiv, dividerAddFracDiv;				
	uint32_t diviser = 0 ;								
	uint32_t mulFracDivOptimal = 1;						
	uint32_t dividerAddOptimal = 0;						
	uint32_t diviserOptimal = 0;																				
	uint32_t relativeError = 0;							
	uint32_t relativeOptimalError = 100000;				

	uClk = uClk >> 4; 									/* uClk / 16 */
	for (mulFracDiv = 1 ; mulFracDiv <= 15 ;mulFracDiv++){
		for (dividerAddFracDiv = 0 ; dividerAddFracDiv <= 15 ;dividerAddFracDiv++){
			temp = (mulFracDiv * uClk) / ((mulFracDiv + dividerAddFracDiv));
			diviser = temp / baudrate;									
			if ((temp % baudrate) > (baudrate / 2))					
				diviser++;
			if (diviser > 2 && diviser < 65536){						
				calcBaudrate = temp / diviser;						
				if (calcBaudrate <= baudrate)								
			  		relativeError = baudrate - calcBaudrate;			
				else
			  		relativeError = calcBaudrate - baudrate;
				if ((relativeError < relativeOptimalError)){  			
			  	mulFracDivOptimal = mulFracDiv ;
			  	dividerAddOptimal = dividerAddFracDiv;
			  	diviserOptimal = diviser;
			  	relativeOptimalError = relativeError;
			  	if (relativeError == 0)
					break;
				}
		  	} /* End of if */
		} /* end of inner for loop */
		if (relativeError == 0)
	  		break;
	} /* end of outer for loop  */
	if (relativeOptimalError < ((baudrate * UART_ACCEPTED_BAUDRATE_ERROR)/100)){ 

		 		LPC_UART0->LCR |= UART_LCR_DLAB_EN;														/* DLAB = 1 */
				LPC_UART0->/*DLIER.*/DLM = UART_LOAD_DLM(diviserOptimal);			/* DLM?? */
				LPC_UART0->/*RBTHDLR.*/DLL = UART_LOAD_DLL(diviserOptimal); 	/* DLL?? */
				LPC_UART0->LCR &= (~UART_LCR_DLAB_EN) & UART_LCR_BITMASK;			/* DLAB = 0 */
				LPC_UART0->FDR = (UART_FDR_MULVAL(mulFracDivOptimal) \
						| UART_FDR_DIVADDVAL(dividerAddOptimal)) & UART_FDR_BITMASK;
	
		errorStatus = 1;
	}
	return errorStatus;
}
           
/*********************************************************************************************************
** Function name:       UART0_Init
** Descriptions:        ��Ĭ��ֵ��ʼ������0�����ź�ͨѶ����������Ϊ8λ����λ��1λֹͣλ������żУ��
** input parameters:    ������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UART0_Init (uint32_t UART0_BPS)
{	
    uint32_t pclkdiv,pclk;
    uint32_t lcr_d = 3, lcr_p = 0, lcr_s = 0; 

    LPC_PINCON->PINSEL0 |= (1 << 4);            /* Pin P0.2 used as TXD0 (Com0)*/ 
    LPC_PINCON->PINSEL0 |= (1 << 6);            /* Pin P0.3 used as RXD0 (Com0) */
	
	pclkdiv = (LPC_SC->PCLKSEL0 >>6) & 0x03;
	switch ( pclkdiv ){
		case 0x00:
		default:
			pclk = SystemCoreClock/4;
			break;
		case 0x01:
			pclk = SystemCoreClock;
			break; 
		case 0x02:
			pclk = SystemCoreClock/2;
			break; 
		case 0x03:
			pclk = SystemCoreClock/8;
			break;
	}  
 
	LPC_UART0->FDR = 0;                             // Fractional divider not used
	LPC_UART0->LCR = 0x80 | lcr_d | lcr_p | lcr_s;  // Data bits, Parity,   Stop bit
	LPC_UART0->LCR = 0x00 | lcr_d | lcr_p | lcr_s;  // DLAB = 0
	LPC_UART0->IER = 0x03;                          // Enable TX/RX interrupts
  uart0_set_divisors(0,pclk,UART0_BPS);   
	LPC_UART0->FCR = 0x07;													/* Enable and reset TX and RX FIFO. */	
}

/*********************************************************************************************************
** Function name:       UART0_SendByte
** Descriptions:        ����0��������
** input parameters:    ucdata
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
int UART0_SendByte (int ucData)
{
	while (!(LPC_UART0->LSR & 0x20));           /*��bit5 0 Ϊ�����ݣ�1 Ϊ�������*/
    return (LPC_UART0->THR = ucData);
}


/*********************************************************************************************************
** Function name:      UART0_GetChar
** Descriptions:       �Ӵ���0��ȡ�ַ�
** input parameters:   ��
** output parameters:  ��
** Returned value:     RBR
*********************************************************************************************************/
int UART0_GetChar (void) 
{
  	while (!(LPC_UART0->LSR & 0x01));	       /*��bit0 0 Ϊ�գ�1 Ϊ���յ�����*/
  	return (LPC_UART0->RBR);			    
}

/*********************************************************************************************************
** Function name:       UART0_SendString 
** Descriptions:        �Ӵ���0��������
** input parameters:    s: Ҫ���͵��ַ���ָ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UART0_SendString (unsigned char *s) 
{
  	while (*s != 0) 
	{
   		UART0_SendByte(*s++);
	}
}


int uart2_set_divisors(uint8_t uartNum, uint32_t uClk, uint32_t baudrate)
{
	int errorStatus = 0;
	uint32_t calcBaudrate = 0;							
	uint32_t temp = 0;								

	uint32_t mulFracDiv, dividerAddFracDiv;				
	uint32_t diviser = 0 ;								
	uint32_t mulFracDivOptimal = 1;						
	uint32_t dividerAddOptimal = 0;						
	uint32_t diviserOptimal = 0;																				
	uint32_t relativeError = 0;							
	uint32_t relativeOptimalError = 100000;				

	uClk = uClk >> 4; 									/* uClk / 16 */
	for (mulFracDiv = 1 ; mulFracDiv <= 15 ;mulFracDiv++){
		for (dividerAddFracDiv = 0 ; dividerAddFracDiv <= 15 ;dividerAddFracDiv++){
			temp = (mulFracDiv * uClk) / ((mulFracDiv + dividerAddFracDiv));
			diviser = temp / baudrate;									
			if ((temp % baudrate) > (baudrate / 2))					
				diviser++;
			if (diviser > 2 && diviser < 65536){						
				calcBaudrate = temp / diviser;						
				if (calcBaudrate <= baudrate)								
			  		relativeError = baudrate - calcBaudrate;			
				else
			  		relativeError = calcBaudrate - baudrate;
				if ((relativeError < relativeOptimalError)){  			
			  	mulFracDivOptimal = mulFracDiv ;
			  	dividerAddOptimal = dividerAddFracDiv;
			  	diviserOptimal = diviser;
			  	relativeOptimalError = relativeError;
			  	if (relativeError == 0)
					break;
				}
		  	} /* End of if */
		} /* end of inner for loop */
		if (relativeError == 0)
	  		break;
	} /* end of outer for loop  */
	if (relativeOptimalError < ((baudrate * UART_ACCEPTED_BAUDRATE_ERROR)/100)){ 

		 		LPC_UART2->LCR |= UART_LCR_DLAB_EN;									/* DLAB = 1 */
				LPC_UART2->/*DLIER.*/DLM = UART_LOAD_DLM(diviserOptimal);			/* DLM?? */
				LPC_UART2->/*RBTHDLR.*/DLL = UART_LOAD_DLL(diviserOptimal); 		/* DLL?? */
				LPC_UART2->LCR &= (~UART_LCR_DLAB_EN) & UART_LCR_BITMASK;			/* DLAB = 0 */
				LPC_UART2->FDR = (UART_FDR_MULVAL(mulFracDivOptimal) \
						| UART_FDR_DIVADDVAL(dividerAddOptimal)) & UART_FDR_BITMASK;
	
		errorStatus = 1;
	}
	return errorStatus;
}


/*********************************************************************************************************
** Function name:       UART2_Init
** Descriptions:        ��Ĭ��ֵ��ʼ������2�����ź�ͨѶ����������Ϊ8λ����λ��1λֹͣλ������żУ��
** input parameters:    ������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UART2_Init (uint32_t UART2_BPS)
{	
    uint32_t pclkdiv,pclk;
    uint32_t lcr_d = 3, lcr_p = 0, lcr_s = 0; 
	
    LPC_PINCON->PINSEL0 |= (1 << 20);             /* Pin P0.10 used as TXD2 (Com2)*/ 
    LPC_PINCON->PINSEL0 |= (1 << 22);             /* Pin P0.11 used as RXD2 (Com2)*/
	
	pclkdiv = (LPC_SC->PCLKSEL0 >> 16) & 0x03;
	switch ( pclkdiv ){
		case 0x00:
		default:
			pclk = SystemCoreClock/4;
			break;
		case 0x01:
			pclk = SystemCoreClock;
			break; 
		case 0x02:
			pclk = SystemCoreClock/2;
			break; 
		case 0x03:
			pclk = SystemCoreClock/8;
			break;
	}  
  LPC_SC->PCONP = LPC_SC->PCONP|(1<<24);	    		/*ʹ��UART2										*/	 
	LPC_UART2->FDR = 0;                             // Fractional divider not used
	LPC_UART2->LCR = 0x80 | lcr_d | lcr_p | lcr_s;  // Data bits, Parity,   Stop bit
	LPC_UART2->LCR = 0x00 | lcr_d | lcr_p | lcr_s;  // DLAB = 0
	LPC_UART2->IER = 0x03;                          // Enable TX/RX interrupts
  uart2_set_divisors(2,pclk,UART2_BPS);
    
	LPC_UART2->FCR = 0x07;													/* Enable and reset TX and RX FIFO. */
}

/*********************************************************************************************************
** Function name:       UART2_SendByte
** Descriptions:        �Ӵ���2��������
** input parameters:    data: ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
int UART2_SendByte (int ucData)
{
	while (!(LPC_UART2->LSR & 0x20));
    return (LPC_UART2->THR = ucData);
}

/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
/*********************************************************************************************************
** Function name:       UART2_GetChar
** Descriptions:        �Ӵ���2��ȡ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      RBR
*********************************************************************************************************/
int UART2_GetChar (void) 
{
  	while (!(LPC_UART2->LSR & 0x01));
  	return (LPC_UART2->RBR);
}

/*********************************************************************************************************
** Function name:     UART2_SendString
** Descriptions:	    �򴮿�2�����ַ���
** input parameters:  s:   Ҫ���͵��ַ���ָ��
** output parameters: ��
** Returned value:    ��
*********************************************************************************************************/
void UART2_SendString (unsigned char *s) 
{
  	while (*s != 0) 
	{
   		UART2_SendByte(*s++);
	}
}

void UART0_SendChar(uint16_t disp)
{
	uint16_t dispbuf[4];
	uint8_t i;

	dispbuf[3] = disp%10 + '0';
	dispbuf[2] = disp/10%10 + '0';
	dispbuf[1] = disp/10/10%10 + '0';
	dispbuf[0] = disp/10/10/10%10 + '0';
	for(i=0;i<4;i++)
		UART0_SendByte(dispbuf[i]);	
}

//�������´��룬��Ϊ��ʹ��printf��
/*********************************************************************************************************
** Function name:       sendchar
** Descriptions:        �򴮿ڷ��͵����ַ���Ŀ���Ǻ�stdio����
** input parameters:    ch:   Ҫ���͵��ַ�
** output parameters:   ��
** Returned value:      ���͹����ַ�
*********************************************************************************************************/
int sendchar(int ch)
{
     UART0_SendByte(ch);
     return(ch);
}

/*********************************************************************************************************
** Function name:       getkey
** Descriptions:        �Ӵ��ڻ�ȡ�����ַ���Ŀ���Ǻ�stdio����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��ȡ���ַ�
*********************************************************************************************************/
int getkey(void)
{
    return (int)UART0_GetChar();
}
