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
*        ÅÐ¶ÏÊÇ·ñÓÐ½»µã
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
*        »ñÈ¡¼ÆÊýÆ÷
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
*        »ñÈ¡Êý×ÖÊ¶±ð
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int getNum(int x[200], int y[200], int point) {
    int start, end, deltaX, deltaY, mX, mY, j, xCount = 0, yCount = 0, yPos, temp,
                                               x45, y45, x45count, y45count, y25, y25count,
                                               minX, maxX, minY, maxY;
    start = 2;
    end = point - 2;
    minX = minArr(x, end) ;
    maxX = maxArr(x, end) ;
    minY = minArr(y, end);
    maxY = maxArr(y, end);
    deltaX = maxX - minX;
    deltaY = maxY - minY;
    x45 = deltaX * 7 / 8 + minArr(x, end);
    y25 = deltaY * 3 / 8 + minArr(y, end);
    y45 = deltaY * 4 / 5 + minArr(y, end);
    mX = minX  + deltaX / 2;
    mY = minY  + deltaY / 2;

    for(j = start; j < end; j++) {
        if(x[j] > mX && x[j + 1] <= mX || x[j] < mX && x[j + 1] >= mX) {
            xCount++;
        }
        if(y[j] > mY && y[j + 1] <= mY || y[j] < mY && y[j + 1] >= mY) {

            yCount++;
        }

    }
    printf("\nxCount:%d,yCount:%d", xCount, yCount);

    if(deltaX < deltaY / 3) {
        return 1;
    }
    else if(yCount == 1 && xCount == 2) {
        return 7;

    }
    else if(yCount >= 2 && xCount >= 4) {
        return 8;

    }
    else if(xCount == 2 && yCount == 2) {
        y25count = 0;
        y45count = 0;
        for(j = start; j < end; j++) {
            if(x[j] >= x45 && x[j + 1] < x45 || x[j] <= x45 && x[j + 1] > x45) {
                x45count ++;
            }
            if(y[j] >= y45 && y[j + 1] < y45 || y[j] <= y45 && y[j + 1] > y45) {
                y45count ++;
            }
        }
        printf("\n%d,%d ", x45count, y45count);
        if(x45count > 1 && y45count > 1) {
            return 0;
        }
        else {

            return 4;
        }

    }
    else if (xCount == 4 && yCount == 1) {
        return 3;

    }
    else if(yCount == 2 && xCount == 3) {
        y25count = 0;
        y45count = 0;
        x45count = 0;

        for(j = start; j < end; j++) {
            if(x[j] >= x45 && x[j + 1] < x45 || x[j] <= x45 && x[j + 1] > x45) {
                x45count ++;
            }
            if(y[j] >= y45 && y[j + 1] < y45 || y[j] <= y45 && y[j + 1] > y45) {
                y45count ++;
            }
        }
        if(y45count > 1) {
            return 6;
        }
        else {

            return 5;
        }
    }
    else if(yCount == 1 && xCount == 3) {
        int temp;
        y25count = 0;
        y45count = 0;
        for(j = start; j < end; j++) {
            if(y[j] >= y25 && y[j + 1] < y25 || y[j] <= y25 && y[j + 1] > y25) {
                y25count++;
                temp = x[j];
            }
            if(y[j] >= y45 && y[j + 1] < y45 || y[j] <= y45 && y[j + 1] > y45) {
                y45count++;

            }
        }

        if(y25count > 1 && y45count < 2) {

            return 9;
        }
        if(y25count < 2 && y45count >= 2) {
            return 6;

        }
        else {
            if(temp < mX) {
                return 5;
            } else {
                return 2;
            }
        }
    }
    return 255;
}







/*******************************************************************************
*        »ñÈ¡×ÖÄ¸
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
char getCharacter(int x[200], int y[200], int point) {
    int start, end, deltaX, deltaY, mX, mY, j, xCount = 0, yCount = 0, minX, maxX, minY, maxY, yPos, xPos ,temp;
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

    for(j = start; j < end; j++) {
        if(hasIntersection(x, j, mX)) {
            xCount++;
        }
        if(hasIntersection(y, j, mY)) {

            yCount++;
        }

    }
    printf("\nxCount:%d,yCount:%d", xCount, yCount);

/////////////////////////////////		ÅÐ¶Ïj£¬L		///////////////////////////////////////////////////
    if(xCount == 1 && yCount == 1) {

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
    ////////////////////////		ÅÐ¶Ï£¬u,v,n,r	/////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 2) {
        yPos = minY + deltaY * 5 / 6;
        yCount = getCount(start, end, yPos, y);
        if(yCount == 1) {

            return 'r';
        }
    }
    ///////////////ÅÐ¶Ï h,n/////////////////////////////////////////////////////////////////
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
    ///////////////ÅÐ¶Ï w/////////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 4) {
        return 'w';
    }

    ///////////////ÅÐ¶Ïm/////////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 5) {
        return 'm';

    }


    ///////////////ÅÐ¶Ï		b,c,p,q		///////////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 1) {
        yCount = 0;
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
            return 'c';
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

    ///////////////ÅÐ¶Ï		o	,p,q		/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 2) {
        int yPos, temp;
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

    ///////////////ÅÐ¶Ï		b,d,x,		/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 3) {





    }

    ///////////////ÅÐ¶Ï		d,		/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 4) {
        return 'd';


    }
    ///////////////ÅÐ¶Ï	g,z,s			/////////////////////////////////////////////////////////////////
    else if(xCount == 3 && yCount == 1) {

        yCount = 0;
        yPos = minY + deltaY * 1 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            return 'g';

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
                return 'z';
            }
            else {
                return 's';
            }

        }
    }

    ///////////////ÅÐ¶Ï		e,y		/////////////////////////////////////////////////////////////////
    else if(xCount == 3 && yCount == 2) {
        xCount = 0;
        xPos = minX + deltaY * 3 / 4;
        for(j = start; j < end; j++) {
            if(hasIntersection(x, j, xPos)) {
                temp = y[j];
            }
            if(temp >mY+ mY  /2) {
                return 'e';

            }
            else {
                return 'y';

            }
        }


    }

    ///////////////ÅÐ¶Ï	k			/////////////////////////////////////////////////////////////////
    else if(xCount == 3 && yCount == 3) {

        return 'k';

    }
    ///////////////ÅÐ¶Ï	g			/////////////////////////////////////////////////////////////////
    else if(xCount == 4 && yCount == 2) {
        return 'g';
    }


    return 255;
}



