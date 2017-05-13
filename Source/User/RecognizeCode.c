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
/*******************************************************************************
*        �ж��Ƿ��н���
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int hasIntersection(int a[200], int j, int pos) {
    if(a[j] > pos && a[j + 1] <= pos || a[j] < pos && a[j + 1] >= pos) {
        return 1;
    }
    return 0;
}

/*******************************************************************************
*        ��ȡ������
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int getCount(int start, int end, int pos, int a[200]) {
    int j, count = 0;
    for(j = start; j < end; j++) {
        if(hasIntersection(a, j, pos)) {
            count++;

        }
    }
    return count;
}
/*******************************************************************************
*        ��ȡ����ʶ��
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int getNum(int x[200], int y[200], int point) {
    int start, end, deltaX, deltaY, mX, mY, j, xCount = 0, yCount = 0, minX, maxX, minY, maxY, yPos, xPos , temp;
    start = 2;
    end = point - 2;
    minX = minArr(x, end) ;
    maxX = maxArr(x, end) ;
    minY = minArr(y, end);
    maxY = maxArr(y, end);
    deltaX = maxX - minX;
    deltaY = maxY - minY;
    mX = minX  + deltaX / 2;
    mY = minY  + deltaY / 2;

    xCount = getCount(start, end, mX, x);//x�ύ����
    yCount = getCount(start, end, mY, y);//y�ύ����
    printf("\nxCount:%d,yCount:%d", xCount, yCount);

    if(xCount == 1 && yCount == 1) {
        if(deltaX < deltaY / 3) {
            return 1;
        }

    }
    else if(  xCount == 2 && yCount == 1) {
        yCount  = 0;
        yPos = minY + deltaY % 1 / 6;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            return 9;

        }
        else {
            return 7;
        }
    }
////////////////////�ж�0��4////////////////////////////////////////////
    else if(xCount == 2 && yCount == 2) {
        yCount = 0;
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            return 0;
        }
        else {
            return 4;
        }

    }
////////////////////�ж�9��6��2��5////////////////////////////////////////////
    else if(xCount == 3 && yCount == 1) {
        xCount = 0;
        yCount = 0;
        yPos = minY + deltaY * 4 / 5;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) { ////6,5
            yCount = 0;
            yPos = minY + deltaY * 3 / 5;
            yCount = getCount(start, end, yPos, y);
            if(yCount > 1) {
                return 6;

            }
            return 5;

        }
        else {
            yCount = 0;
            yPos = minY + deltaY * 1 / 5;
            yCount = getCount(start, end, yPos, y);
            if(yCount > 1) {
                xCount = 0;
                for(j = start; j < end; j++) {
                    if(y[j] < mY && hasIntersection(x, j, mX)) {
                        xCount++;

                    }

                }
                if(xCount > 1) {
                    return 9;
                }
                else {
                    return 2;
                }

            }
            else {
                yPos = minY + deltaY / 4;
                for(j = start; j < end; j++) {
                    if(hasIntersection(y, j, yPos)) {
                        temp = x[j];
                    }
                }
                if(temp < mX) {

                    return 5;
                }

                return 2;
            }
        }

    }


////////////////////�ж�6��5��////////////////////////////////////////////
    else if( xCount == 3 && yCount == 2) {
        yCount = 0;
        yPos = minY + deltaY * 4 / 5;
        yCount = getCount(start, end, yPos, y);

        if(yCount > 1) {
            return 6;
        }
        else {
            return 5;
        }
    }
    else if( xCount == 3 && yCount == 3) {
        return 5;
    }

    else if (xCount == 4 && yCount == 1) {
        return 3;

    }
    else if( xCount >= 4 && yCount >= 2 ) {
        return 8;

    }

}


/*******************************************************************************
*        ��ȡСд��ĸ
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
char getCharacter(int x[200], int y[200], int point) {
    int start, end, deltaX, deltaY, mX, mY, j, xCount = 0, yCount = 0, minX, maxX, minY, maxY, yPos, xPos , temp;
    start = 2;
    end = point - 2;
    minX = minArr(x, end) ;
    maxX = maxArr(x, end) ;
    minY = minArr(y, end);
    maxY = maxArr(y, end);
    deltaX = maxX - minX;
    deltaY = maxY - minY;
    mX = minX  + deltaX / 2;
    mY = minY  + deltaY / 2;

    xCount = getCount(start, end, mX, x);

    yCount = getCount(start, end, mY, y);
    printf("\nxCount:%d,yCount:%d", xCount, yCount);
    if(deltaX < deltaY / 5) {
        return 'i';
    }
/////////////////////////////////		�ж�j��L		///////////////////////////////////////////////////
    if(xCount == 1 && yCount == 1) {
        if(deltaX < deltaY / 5) {
            return 'i';
        }

        for(j = start; j < end; j++) {

            if(hasIntersection(y, j, mY)) {
                temp = x[j];
            }
        }
        printf("\n%d", temp);
        printf("\n%d,%d", mX, mY);
        if(temp > mX) {
            return 'j';

        }
        else {
            return 'L';
        }



    }
    ////////////////////////		�жϣ�u,v,n,r	/////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 2) {
        int xValue[5], count = 0, absX, i;
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);

        for(j = start; j < end; j++) {
            if(hasIntersection(y, j, yPos)) {

                xValue[count]	= x[j]	;
                count++;

            }
        }
        absX = abs(xValue[0] - xValue[1]);
        if(yCount == 1) {
            return 'r';
        }
        else  if(count >= 2) {

            if((float)absX / abs(minX - maxX) > 0.5) {
                yPos = minY + deltaY * 1 / 4;
                yCount = getCount(start, end, yPos, y);
                if(yCount > 2) {
                    return 'n';
                }
                else {
                    return 'u';
                }



            }

            else {
                count == 0;
                for(j = start; j < end; j++) {
                    for(i = j + 1; i < end; i++) {
                        if(x[j] == x[i]) {
                            count++;
                        }

                    }
                }
                printf("\n%d", count);
                if(count > 3) {
                    return 'r';
                }
                else {
                    return 'v';
                }

            }


        }

    }




///////////////�ж� h,n/////////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 3) {
        yPos = minY + deltaY * 2 / 7;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            return 'n';

        }
        else {
            return 'h';
        }


    }
///////////////�ж� w/////////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 4) {
        return 'w';
    }

///////////////�ж�m/////////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 5) {
        return 'm';

    }


///////////////�ж�		b,c,p,q	,t,d,t///////////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 1) {
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            yPos = minY + deltaY * 1 / 4;
            yCount = getCount(start, end, yPos, y);
            for(j = start; j < end; j++) {
                if(hasIntersection(y, j, yPos)) {

                    temp = x[j];
                }

            }
            if(temp > mX) {
                return 'd';

            }
            return 'b';

        }
        yPos = minY + deltaY * 1 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount < 2) {
            xPos = minX + deltaX * 1 / 4;
            xCount = getCount(start, end, xPos, x);
            if(xCount > 1) {
                return 'c';
            }
            else {


                return 'f';

            }

        }
        else {
            yPos = minY + deltaY * 3 / 4;
            yCount = getCount(start, end, yPos, y);

            for(j = start; j < end; j++) {
                if(hasIntersection(y, j, yPos)) {
                    temp = x[j];

                }

            }
            if(temp < mX) {
                return 'p';
            }
            else {
                return 'q';
            }


        }

    }

///////////////�ж�		o	,p,q		/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 2) {
        yCount = 0;
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount < 2) {
            yPos = minY + deltaY * 3 / 4;
            yCount = getCount(start, end, yPos, y);

            for(j = start; j < end; j++) {
                if(hasIntersection(y, j, yPos)) {
                    temp = x[j];

                }

            }
            if(temp < mX) {
                return 'p';
            }
            else {


                return 'q';
            }

        } else {

            return 'o';
        }

    }

///////////////�ж�		a,b,d,x,		/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 3) {
        int absX, xValue, count = 0,
                          yPos = minY + deltaY / 4;
        yCount = getCount(start, end, yPos, y);
			   for(j = start; j < end; j++) {
                if(hasIntersection(y, j, yPos)) {
                    xValue[count]	= x[j];
                    count++;

                }
            }
						//////////TODO
            absX = abs(xValue[0] - xValue[1]);
        if(yCount == 1) {
            for(j = start; j < end; j++) {
                if(hasIntersection(y, j, yPos)) {
                    temp = x[j];
                }

            }
            if(temp > mX) {
                return 'd';

            }
            return 'b';
        }

        else if(yCount == 3) {
         
            if(count >= 2) {

                if((float)absX / abs(minX - maxX) > 0.5) {
                    return 'a';

                }
                else {
                    return 'x';
                }



            }
        }
    }

///////////////�ж�		d,		/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 4) {
        return 'd';


    }
///////////////�ж�	g,z,s	,t		/////////////////////////////////////////////////////////////////
    else if(xCount == 3 && yCount == 1) {
        yPos = minY + deltaY * 1 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            yPos = minY + deltaY * 3 / 4;
            yCount = getCount(start, end, yPos, y);
            for(j = start; j < end; j++) {
                if(hasIntersection(y, j, mY)) {
                    temp = x[j];

                }


            }
            if(temp > mX) {
                return 'g';

            }
            else {
                return 't';
            }

        }
        else {
            yPos = minY + deltaY * 3 / 4;
            xPos = minX + deltaX * 1 / 4;

            xCount = getCount(start, end, xPos, x);


            if(xCount == 1) {
                return 't';
            }
            if(temp < mX) {
                return 'z';
            }
            else {
                return 's';
            }


        }
    }

///////////////�ж�		e,y		/////////////////////////////////////////////////////////////////
    else if(xCount == 3 && yCount == 2) {
        xCount = 0;
        xPos = minX + deltaY * 3 / 4;
        for(j = start; j < end; j++) {
            if(hasIntersection(x, j, xPos)) {
                temp = y[j];
            }
            if(temp > mY + mY  / 2) {
                return 'e';

            }
            else {
                return 'y';

            }
        }


    }

///////////////�ж�	k			/////////////////////////////////////////////////////////////////
    else if(xCount == 3 && yCount == 3) {

        return 'k';

    }
///////////////�ж�	g			/////////////////////////////////////////////////////////////////
    else if(xCount == 4 && yCount == 2) {
        return 'g';
    }


    return ' ';
}



