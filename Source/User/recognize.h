/****************************************
*我自己定义的一些东西*
*****************************************
*/


#include "lcd.h"
#include "sys.h"               /* Screen Hight (in pixels)           */
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304

#define Blue_Pen Pen_Point.X0>0&&Pen_Point.X0<25&&Pen_Point.Y0>20&&Pen_Point.Y0<45
#define Red_Pen Pen_Point.X0>25+2&&Pen_Point.X0<27+25&&Pen_Point.Y0>20&&Pen_Point.Y0<45
#define Black_Pen Pen_Point.X0>52+2&&Pen_Point.X0<25+54&&Pen_Point.Y0>20&&Pen_Point.Y0<45
#define Green_Pen Pen_Point.X0>79+2&&Pen_Point.X0<81+25&&Pen_Point.Y0>20&&Pen_Point.Y0<45
#define Touch_Area Pen_Point.Y0>=100&&Pen_Point.X0>0
#define Clear_Area Pen_Point.X0>180&&Pen_Point.X0<240&&Pen_Point.Y0>20&&Pen_Point.Y0<45
#define OK_Area Pen_Point.X0>180&&Pen_Point.X0<240&&Pen_Point.Y0>60&&Pen_Point.Y0<80
/**********************************
****************变量定义***********
************************************/


/*****函数区*****/ void touchMain(void);
// extern void LCD_ShowStr(u8 x, u16 y, const u8 *p, u16 color);
// extern void Init_Color_Area(u16 x0, u16 y0, u16 color, u16 size);
// extern void Init_Color_Area_Diy(u16 start_x0, u16 start_y0, u16 size);
// extern void delay1ms(unsigned int delay1msdata);
// extern u8 KEY_Scan(void);
// extern void Choose_Pen_Color(void);
 void initGUI(void);
int minArr(int a[200],int endPoint);
int maxArr(int a[200],int endPoint);
// extern void Load_Drow_Dialog(void);

