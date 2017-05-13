The LCD_Blinky project is a simple program for the NXP 'LPC1768' 
microcontroller using Keil 'MCB1700' evaluation board and 
demonstrating interrupt functionality and display to graphic LCD.
(compliant to Cortex Microcontroller Software Interface Standard 
(CMSIS v1.0))

Example functionality:                                                   
 - Clock Settings:
   - XTAL  =  12 MHz
   - PLL0  = 400 MHz
   - CCLK  = 100 MHz
 - Displays rotating ARM 3D logo, each picture is displayed every 
   150 ms
 - 8 LEDs blink with speed of 50 ms, determined by SysTick Timer
   working in interrupt mode
 
The Blinky program is available for target:
  LPC1768 Flash: runs from Internal Flash located on chip
                 (used for production or target debugging)

Notes: - In EVAL version of uVision do not rebuild the target as 
         this example's size exceeds EVAL version limitation

Picture conversion procedure:
1. Convert picture to 16 BPP (R - 5bits, G - 6bits, B - 5bits)
2. Remove header from bmp by hex editor, remaining file size should 
   be equal to (width * height * 2) bytes
3. Convert prepared file without header to C array
/**********************************
** 宝马开发板
** 论坛：bbs.openmcu.com
** 旺宝：www.openmcu.com
** 邮箱：support@openmcu.com

** 实验名：【3】触摸屏实验
** KEIL MDK-ARM Standard Version：V4.22A
** 软件库(keil) Version: unused
** 固件库(ST) Version: unused
** 使用外设: 3.2LCD
**********************************/


/*********************************
** 实验说明：
插上3.2寸屏幕
** 实验现象：
按下按键INT0进行校准,校准后能再屏幕上实现画板的功能，如果想从新校准请按下INT0就ok了
**********************************/
