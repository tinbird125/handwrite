#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "SYS.H"

#define Key_Down 0x01
#define Key_Up   0x00 
/*�ʸ˽ṹ��*/
typedef struct 
{
	u16 X0;						/*ԭʼ����		*/
	u16 Y0;
	u16 X; 						/*����/�ݴ����� */
	u16 Y;						   	    
	u8  Key_Sta;			    /*�ʵ�״̬      */			  
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;

extern Pen_Holder Pen_Point;

#define TDIN(x) GPIO0_9(x)  
#define TCLK(x) GPIO0_7(x)  
#define TCS(x)  GPIO0_6(x) 
#define DOUT    GPIO0_8  
#define PEN     GPIO2_13  

/*ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ָ�*/
#define CMD_RDY 0X90  /*0B10010000���ò�ַ�ʽ��X���� */
#define CMD_RDX	0XD0  /*0B11010000���ò�ַ�ʽ��Y���� */  											 
#define TEMP_RD	0XF0  /*0B11110000���ò�ַ�ʽ��Y���� */



void Touch_Init(void);		   
u8 Read_ADS(u16 *x,u16 *y);
u8 Read_ADS2(u16 *x,u16 *y); 
u16 ADS_Read_XY(u8 xy);		
u16 ADS_Read_AD(u8 CMD);	 
void ADS_Write_Byte(u8 num); 
void Drow_Touch_Point(u8 x,u16 y);
void Draw_Big_Point(u8 x,u16 y);  
void Touch_Adjust(void);         
void Save_Adjdata(void);		 
u8 Get_Adjdata(void); 		
void Pen_Int_Set(u8 en); 		 
void Convert_Pos(void); 
void gerPos(void);
void  InitializeSPI(void);
void delay1ms(unsigned int delay1msdata);
void SpiDelay(unsigned int DelayCnt);
uint32_t EINTInit( void );
#endif

