#include "sys.h"
#include "recognize.h"
#include "lcd.h"
#include <LPC17xx.H>
#include "touch.h"
#include "math.h"
#include <stdlib.h>
#include "string.h"
#include "lpc17xx.h"
#include "uart.h"
#include "stdio.h"
#include "RecognizeCode.h"
u8 key;
u8 buf1[32];
u16 rectangle_size = 25; //矩形大小25
u16 start_x0 = 0 ; //矩形起点x坐标x0=0
u16 start_y0 = 20 ; //举行起点y坐眣0=20
int point = 0;
int x[200];
int  y[200];
int future[20][25];//特征值
double  conditionArr[10] = {
    0.000000000000000845939275201984320000000000000000000000000000,
    0.000000000000000000000000000000950620196327242240000000000000,
    0.000000000000000076703947087176432000000000000000000000000000,
    0.000000000000014768066640829700000000000000000000000000000000,
    0.000000000000000000000000922291546853083200000000000000000000,
    0.000000000000000617218701186411760000000000000000000000000000,
    0.000000000000663492179444729200000000000000000000000000000000,
    0.000000000000000000000205333945952220680000000000000000000000,
    0.000000054163512914863128000000000000000000000000000000000000,
    0.000000000000019559505375681924000000000000000000000000000000
};//条件概率
double pSum = 0.00000000541642122744767280;
char clearFlag = 1;
char okFlag = 1;
double pp[20];
double anverage[10];
/*********************************************************************************************************
** Function name:       Delay
** Descriptions:        软件延时
** input parameters:    ulTime
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int8_t byte, a;
uint8_t buffer[];
void DelayUart (uint32_t Time)
{
    uint32_t i;

    i = 0;
    while (Time--) {
        for (i = 0; i < 5000; i++);
    }
}

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
void uart(void)
{
    UART2_Init(115200);
    UART0_Init(115200);

}
/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void LCD_ShowStr(u8 x, u16 y, const u8 *p, u16 color) {
    POINT_COLOR = color;
    while(*p != '\0')
    {
        if(x > MAX_CHAR_POSX) {
            x = 0;
            y += 16;
        }
        if(y > MAX_CHAR_POSY) {
            y = x = 0;
            LCD_Clear(White);
        }
        LCD_ShowChar(x, y, *p, 16, 0);
        x += 8;
        p++;
    }
}
/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void init_Color_Area(u16 x0, u16 y0, u16 color, u16 size) {

    u8 x1, y1;

    if(size + x0 < 240 && size + y0 < 320) {
        x1 = x0 + size;
        y1 = y0 + size;
    }
    POINT_COLOR = color;
    LCD_DrawRectangle(x0, y0, x1, y1, color);

}

/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void init_Color_Area_Diy(u16 start_x0, u16 start_y0, u16 size) {
    init_Color_Area(start_x0, start_y0, Blue, size);
    start_x0 = start_x0 + size + 2;
    init_Color_Area(start_x0, start_y0, Red, size);
    start_x0 = start_x0 + size + 2;
    init_Color_Area(start_x0, start_y0, Black, size);
    start_x0 = start_x0 + size + 2;
    init_Color_Area(start_x0, start_y0, Green, size);
}

/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void load_Drow_Dialog(void)
{
    LCD_Clear(WHITE);
    POINT_COLOR = Black;						//Setting font colors
    LCD_ShowString(216, 0, "RST");			//Displaying screen clear area
    POINT_COLOR = Blue;						//Setting brush color
}
/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void delay1ms(unsigned int delay1msdata)
{
    unsigned int i, j, k;
    for(i = 0; i < delay1msdata; i++)
        for(j = 0; j < 1000; j++)
            for(k = 0; k < 100; k++)
                ;
}
/*******************************************************************************
*        延时函数
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int delayms(unsigned int delay1msdata, int delayms)
{
    unsigned int i, j, k;
    for(i = 0; i < delay1msdata; i++)
        for(j = 0; j < 1000; j++)
            for(k = 0; k < 100; k++)
                return delayms;             ;
}
/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
u8 KEY_Scan(void)
{
    if(!SW1)
        delay1ms(2);
    if(!SW1)
        return 1;
    else
        return 0;
}


/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void choose_Pen_Color(int flag) {
    if(Blue_Pen) {
        POINT_COLOR = Blue;

    }
    if(Red_Pen) {
        POINT_COLOR = Red;

    }
    if(Black_Pen) {
        POINT_COLOR = Black;

    }
    if(Green_Pen) {
        POINT_COLOR = Green;
    }
    if(Clear_Area) {
            LCD_Clear_Area(White, 238, 219, 1, 101);
            clearFlag = 0;
				point = 0;
    }
		//识别
    if(OK_Area) {
        if(point > 0 ) {
            int resultChar ;
            if(flag == 0) {
//                     resultNum = getNum(x, y, point);
//                     printf("\n%d", resultNum);
                resultChar = getCharacter(x, y, point);
                printf("\n %c", resultChar);
								LCD_ShowChar(0,70,resultChar,16,0);
                point = 0;
                LCD_Clear_Area(White, 219, 238, 1, 101);
                flag = 1;
            }
        }


    }

}
/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void getXY(void) {
    if(point < 200) {
        if(point) {
            if(x[point - 1] != Pen_Point.X0 && y[point - 1] != Pen_Point.Y0) {
                x[point] = Pen_Point.X0;
                y[point] = Pen_Point.Y0;
                point++;
            }

        }
        else {
            x[point] = Pen_Point.X0;
            y[point] = Pen_Point.Y0;
            point++;
        }

    }
}
/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int maxArr(int a[200], int size) {

    int  i;
    int  max = 0;
    for(i = 2; i < size; i++) {
        if(a[i] > max) {
            max = a[i];
        }
    }
    return max;
}
/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int minArr(int a[200], int size) {
    int  i, min;
    min = a[2];
    for(i = 2; i < size; i++) {
        if(a[i] < min) {
            min = a[i];
        }
    }
    return min;

}



/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void touchMain() {
    int flag = 1, time, b = 0, resultNum, delayCount = 0;
    char resultChar;
    //获取点数point属于0~200

    while(1) { 			//检测屏幕是否有操作
        if(key == 1) 															//KEY0按下,则执行校准程序
        {
            LCD_Clear(WHITE);											//清屏
            Touch_Adjust();  												//屏幕校准
            Save_Adjdata();
            load_Drow_Dialog();
        }
        choose_Pen_Color(flag);
        key =  KEY_Scan();

        if(Pen_Point.Key_Sta == Key_Down)									//触摸屏被按下
        {
            flag = 0;
            Pen_Int_Set(0);														//关闭中断
            Convert_Pos();

            Pen_Point.Key_Sta = Key_Up;												//初始化手写笔，初始化手写区域
            if(Touch_Area) {
                Draw_Big_Point(Pen_Point.X0, Pen_Point.Y0);
                getXY();															//获取屏幕的坐标点

            }
            Pen_Int_Set(1);																		//开中断

        }
        else		///按键松开
        {

            delay1ms(20);				//停笔延时

        }

    }
}


/*******************************************************************************
*        function
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
void initGUI() {

    uart();
    SystemInit();
    EINTInit();   	//Touch screen External interrupt initialize function.
    LCD_Init();

    /*GPIO Configuration*/
    LPC_GPIO0->FIODIR |= 0x00002C0;  		//P0.6 0.7 0.9 Output
    LPC_GPIO2->FIODIR |= 0x00000FF;  	//P2.13 input P2.0...P2.7 Output

    LCD_ShowString(60, 50, "Adjust The LCD");		 //按rst键屏幕显示的文字
    Touch_Init();
    LCD_ShowStr(0, 0, "Pen Color:", Blue);
    init_Color_Area_Diy(start_x0, start_y0, 25);	 //画颜色选择块
    LCD_ShowStr(0, 45 + 5, "Recognized Result:", Blue);
    LCD_ShowStr(80, 320 - 16, "Wrint Area", Red);
    LCD_ShowStr(180, 25, "Clear", Red);
    LCD_ShowStr(180, 60, "OK", Red);
    LCD_DrawRectangle(0, 100, 239, 319, Red);	 //画手写区域
}
