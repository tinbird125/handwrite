
#include <LPC17xx.H>                    				//NXP LPC17xx definitions 
#include "LCD.h"
#include "string.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "exti.h"
#include "I2C.H"
#include "recognize.h"
#include "uart.h"
#include "stdio.h"
int main (void)
{		
    initGUI();
    touchMain();//³õÊ¼»¯´¥Ãþ
}


