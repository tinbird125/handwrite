#include "lpc17xx.h"
#include "stdint.h"
#include "touch.h" 
#include "lcd.h"
#include "stdlib.h"
#include "math.h"
#include "sys.h"
#include "exti.h"
#include "i2c.h"

Pen_Holder Pen_Point;
typedef signed long  s32;



void Save_Adjdata(void)
{
	s32 temp_data;
	u8  temp[8];
	temp_data=Pen_Point.xfac*100000000;//保存x校正因素
	temp[0]=(u8)(temp_data & 0xff);//保存x校正因素
	temp[1]=(u8)((temp_data >> 8) & 0xff);//保存x校正因素	
	temp[2]=(u8)((temp_data >> 16) & 0xff);//保存x校正因素	
	temp[3]=(u8)((temp_data >> 24) & 0xff);//保存x校正因素			      
    I2C_WriteNByte(0xA0,1,0X00,temp,8);
	 
	temp_data=Pen_Point.yfac*100000000;//保存y校正因素
	temp[0]=(u8)(temp_data & 0xff);//保存x校正因素
	temp[1]=(u8)((temp_data >> 8) & 0xff);//保存x校正因素	
	temp[2]=(u8)((temp_data >> 16) & 0xff);//保存x校正因素	
	temp[3]=(u8)((temp_data >> 24) & 0xff);//保存x校正因素			       	      
    I2C_WriteNByte(0xA0,1,0X08,temp,8);

	temp_data=Pen_Point.xoff;
	temp[0]=(u8)(temp_data &0xff);
	temp[1]=(u8)((temp_data >>8)&0xff);
    I2C_WriteNByte(0xA0,1,0X10,temp,8);

	temp_data=Pen_Point.yoff;
	temp[0]=(u8)(temp_data &0xff);
	temp[1]=(u8)((temp_data >>8)&0xff);
    I2C_WriteNByte(0xA0,1,0X18,temp,8);

	temp[0]=0xff;
    I2C_WriteNByte(0xA0,1,0X20,temp,1);//校准过了
}

u8 Get_Adjdata(void)
{
	s32 temp_data;
	u8  temp[8],i;
	temp_data = I2C_ReadNByte(0xA0,1,0X20,temp,8); 
   if(temp[0] == 0xff)
   {
		for(i=0;i<8;i++)
		{
			temp[i] = 0;
		}
	   I2C_ReadNByte(0xA0,1,0X00,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.xfac = (float)temp_data /100000000;
	
	   I2C_ReadNByte(0xA0,1,0X08,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.yfac = (float)temp_data /100000000;
	
	   I2C_ReadNByte(0xA0,1,0X10,temp,8);
	   temp_data = (s32)((temp[1]<<8)|temp[0]);
	   Pen_Point.xoff = temp_data;
	
	   I2C_ReadNByte(0xA0,1,0X18,temp,8);
	   temp_data = (s32)((temp[1]<<8)|temp[0]);
	   Pen_Point.yoff = temp_data;
	   return 1;
   }

   return 0;

} 






















/*
*********************************************************************************************************
* Description: 	Dealy function of us.
* Arguments  : 	nus : the data of us
* Returns    : 	None
*********************************************************************************************************
*/
void delay_us(u32 nus)
{
	while(nus--);
}
/*
*********************************************************************************************************
* Description: 	Dealy function of ms.
* Arguments  : 	nms : the data of ms
* Returns    : 	None
*********************************************************************************************************
*/
void delay_ms(u32 nms)
{
	while(nms--)
	delay_us(245);
}
/*
*********************************************************************************************************
* Description: 	Write data.
* Arguments  : 	num : the data value
* Returns    : 	None
*********************************************************************************************************
*/	   
void ADS_Write_Byte(u8 num)    
{  
	u8 count=0;
	
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)
		{
			TDIN(1);
		}  
		else 
		{
			TDIN(0);
		}   
		num<<=1;    
		TCLK(0);
		delay_us(1);	   	 
		TCLK(1);	
	} 			    
} 
/*
*********************************************************************************************************
* Description: 	Reading the value of ADC from 7846/7843/XPT2046/UH7843/UH7846.
* Arguments  : 	CMD  the Command value
* Returns    : 	None
*********************************************************************************************************
*/		   
u16 ADS_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0;
 
	TCLK(0);	 
	TCS(0); 
	TCLK(0);
	delay_us(10);		 
	ADS_Write_Byte(CMD);
	delay_us(13);		   	    
	TCLK(0);
	delay_us(1);		 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK(0);
		delay_us(1);  	    	   
		TCLK(1);
		if(DOUT)
		Num++; 		 
	}
		 	
	Num>>=4;
	TCS(1);	 
	return(Num);   
}
/*
*********************************************************************************************************
* Description: 	Take an order for the value value.
* Arguments  : 	xy  the Command value
* Returns    : 	None
*********************************************************************************************************
*/	 
#define READ_TIMES 15 
#define LOST_VAL 5	 
u16 ADS_Read_XY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
/*
*********************************************************************************************************
* Description: 	Getting the real value of AD function.
* Arguments  : 	*x ,*y   the cale of AD value
* Returns    : 	None
*********************************************************************************************************
*/	
u8 Read_ADS(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;							//fail
	*x=xtemp;
	*y=ytemp;
	return 1;													//success
}

/*
*********************************************************************************************************
* Description: 	Getting the real value of AD function.
* Arguments  : 	x ,y  the coordinate value
* Returns    : 	None
*********************************************************************************************************
*/	
#define ERR_RANGE 50 				
u8 Read_ADS2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
/*
*********************************************************************************************************
* Description: 	Getting the touch value of coordinate function.
* Arguments  :  None
* Returns    : 	None
*********************************************************************************************************
*/				   
u8 Read_TP_Once(void)
{
	u8 t=0;	    
	Pen_Int_Set(0);
	Pen_Point.Key_Sta=Key_Up;
	Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
	while(PEN==0&&t<=250)
	{
		t++;
		delay_ms(10);
	};
	Pen_Int_Set(1);		 
	if(t>=250)return 0;
	else return 1;	
}

/*
*********************************************************************************************************
* Description: 	Draw a touch point function.
* Arguments  : 	x ,y  the coordinate value
* Returns    : 	None
*********************************************************************************************************
*/
void Drow_Touch_Point(u8 x,u16 y)
{
	LCD_DrawLine(x-12,y,x+13,y);
	LCD_DrawLine(x,y-12,x,y+13);
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6);
}
/*
*********************************************************************************************************
* Description: 	Draw a big point function.
* Arguments  : 	x ,y  the coordinate value
* Returns    : 	None
*********************************************************************************************************
*/			   
void Draw_Big_Point(u8 x,u16 y)
{	    
	LCD_DrawPoint(x,y);
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}
/*
*********************************************************************************************************
* Description: 	Getting the value of coordinate function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void Convert_Pos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
 
	}
}
/*
*********************************************************************************************************
* Description: 	Getting the value of coordinate function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void gerPos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
 
	}
}
/*
*********************************************************************************************************
* Description: 	Touch screen External interrupt initialize function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
uint32_t EINTInit( void )
{

  LPC_PINCON->PINSEL4 = 1 << 26;	// set P2.13 as EINT0 and
									// P2.0~7 GPIO output

  LPC_SC->EXTMODE = EINT3_EDGE;		// INT3 edge trigger
  LPC_SC->EXTPOLAR = 0;				// INT3 is falling edge by default

  NVIC_EnableIRQ(EINT3_IRQn);
  return( 1 );
}
/*
*********************************************************************************************************
* Description: 	Touch screen External interrupt function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void EINT3_IRQHandler (void) 
{
	delay_us(1);
	if(!PEN)
	{
 	    Pen_Point.Key_Sta=Key_Down;
	}	

    LPC_SC->EXTINT = EINT3;				// clear interrupt
}
/*
*********************************************************************************************************
* Description: 	Touch screen External interrput Enable function.
* Arguments  : 	1 Enable
*				0 Disable
* Returns    : 	None
*********************************************************************************************************
*/	 
void Pen_Int_Set(u8 en)
{
	if(en)
	NVIC_EnableIRQ(EINT3_IRQn);   //Enable		  	
	else 
	NVIC_DisableIRQ(EINT3_IRQn); //Disable 	   
}
/*
*********************************************************************************************************
* Description: 	Touch screen adjust function.Getting four adjust parameter!
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void Touch_Adjust(void)
{								 
	u16 pos_temp[4][2];									//Coordinate cache value
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	   
	cnt=0;				
	POINT_COLOR=Red;
	BACK_COLOR =Red;
	LCD_Clear(White);   
	POINT_COLOR=Red;      							   //Brush color
	LCD_Clear(White);	   
	Drow_Touch_Point(20,20);						   //Draw the first point 
	Pen_Point.Key_Sta=Key_Up;						   //Clear the signal 
	Pen_Point.xfac=0;								   //Record whether adjust 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)				   //Is touched
		{
			if(Read_TP_Once())						   //got the coordinate value
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(White); 
					Drow_Touch_Point(220,20);			//Draw the second point
					break;
				case 2:
					LCD_Clear(White);
					Drow_Touch_Point(20,300);			//Draw the third point
					break;
				case 3:
					LCD_Clear(White); 
					Drow_Touch_Point(220,300);			//Draw the fouth point
					break;
				case 4:	 								//Have gotten all the value
	    		    /*对边相等*/
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)	//Unqualified
					{
						cnt=0;
						LCD_Clear(White); 
						Drow_Touch_Point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)					//Unqualified
					{
						cnt=0;
						LCD_Clear(White); 
						Drow_Touch_Point(20,20);
						continue;
					}
								   
					/*对角线相等*/
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)													//Unqualified
					{
						cnt=0;
						LCD_Clear(White);
						Drow_Touch_Point(20,20);
						continue;
					}
					/*正确了,计算结果 */
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);				// got xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;	//got xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);				//got yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;	//got yoff  
					POINT_COLOR=Blue;
					LCD_Clear(White);
					LCD_ShowString(35,110,"Touch Screen Adjust OK!");					    //Adjust is OK
					delay_ms(1000);
					LCD_Clear(White);   
					return;
				 
			}
		}
	} 
}		    

/*
*********************************************************************************************************
* Description: 	Touch screen initialize function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void Touch_Init(void)
{			    		   
	LPC_GPIO0->FIODIR |= 0x00002C0;  		//P0.6 0.7 0.9 Output
	LPC_GPIO2->FIODIR |= 0x0000000;  		//P2.13 input P2.0...P2.7 Output
 	Read_ADS(&Pen_Point.X,&Pen_Point.Y);	//the first Read initialize			 
	EINTInit();	 							//Setting External interrupt
	LCD_Clear(WHITE);
  	if ( I2CInit( (uint32_t)I2CMASTER ) == 0 )		/* initialize I2c */
	{
		while ( 1 );								/* Fatal error */
	}
	if(Get_Adjdata())return;//已经校准
	else			   //未校准?
	{ 										    
		LCD_Clear(WHITE);//清屏
	    Touch_Adjust();  //屏幕校准 
		Save_Adjdata();	 
	}			
	Get_Adjdata();
}











