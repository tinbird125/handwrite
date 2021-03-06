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
int arrLength;//��¼���µĵ���
int hasIntersection(int a[200], int j, int pos) {

    if(a[j] >= pos && a[j + 1] < pos || a[j] < pos && a[j + 1] >= pos) {
        if(abs(a[j] - a[j + 1]) < getDelta(a, arrLength) / 3) {
            return 1;
        }

    }
    return 0;
}
int getTemp(int start, int end, int a[200], int b[200], int pos) {
    int j;
    for(j = start; j < end; j++) {
        if(hasIntersection(a, j, pos)) {
            return  b[j];

        }

    }

}
/*******************************************************************************
*    ��ȡdelta
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int getDelta(int a[200], int end) {
    int min, max, delta;
    min = minArr(a, end) ;
    max = maxArr(a, end) ;
    delta =  max - min;
    return delta;

}
/*******************************************************************************
*    ��ȡ��ֵ
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
int getMid(int a[200], int end) {
    return getDelta(a, end) / 2;
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
*        ��ȡ����ľ���
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
float getDis(int start, int end,  int a[200], int b[200], int pos) {
    int dis[2], j, count = 0;
    float delta;
    for(j = start; j < end; j++) {
        if(hasIntersection(a, j, pos)) {
            dis[count] = b[j];
            count++;
        }

    }
    if(count > 1) {
        delta = (float)abs(dis[0] - dis[1]);
        return delta;
    }
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
    int start, end, deltaX, deltaY, mX, mY, j, xCount , yCount , minX, maxX, minY, maxY, yPos, xPos , temp;
    arrLength = point;
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

    xCount = getCount(start, end, mX, x);//����mX������
    yCount = getCount(start, end, mY, y);//�����ύ����
    printf("\nxCount:%d,yCount:%d", xCount, yCount);

    if(xCount == 1 && yCount == 1) {
        if(deltaX < deltaY / 3) {
            return 1;
        }

    }
    else if(xCount == 1 && yCount == 2) {
        return 4;
    }
    else if(  xCount == 2 && yCount == 1) {
        yCount  = 0;
        yPos = minY + deltaY / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            return 9;

        }
        else {
            yPos = minY + deltaY * 3 / 4;
            yCount = getCount(start, end, yPos, y);
            if(yCount > 1) {
                return 6;
            } else {
                if(x[end] > mX) {
                    return 2;
                } else {
                    return 7;
                }

            }
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
                    xPos = minX + deltaX * 3 / 4;
                    xCount = getCount(start, end, xPos, x);
                    if(xCount > 2) {
                        return 3;
                    }
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
        yPos = minY + deltaY / 4;
        xCount = getCount(start, end, yPos, y);
        if(xCount == 1) {
            return 3;
        }
        else {
            return 9;
        }

    }
    else if( xCount >= 4 && yCount >= 2 ) {
        return 8;

    }
    return  ;
}


/*******************************************************************************
*        ��ȡСд��ĸ
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
char getLowerCase(int x[200], int y[200], int point) {
    int start, end, deltaX, deltaY, mX, mY, j, xCount, yCount, minX, maxX, minY, maxY, yPos, xPos , temp;
    start = 2;
    end = point - 2;
    arrLength = point;
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
    printf("\nmX:%d,mY:%d", mX, mY);
    printf("\nstart:%d,end:%d", start, end);
    printf("\nxCount:%d,yCount:%d", xCount, yCount);
    if(deltaX < deltaY / 5) {
        return 'i';
    }
/////////////////////////////////		�ж�j��L		///////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 1) {
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
            return 'l';
        }



    }
    ////////////////////////		�жϣ�u,v,n,r	/////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 2) {
        float dis;
        int count;
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        dis = getDis(start, end, y, x, yPos);

        if(yCount == 1) {
            return 'r';
        }
        else  if(dis / deltaX > 0.5) {
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
            if(count > 3) {
                return 'r';
            }
            else {
                return 'v';
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
        if(y[0] < mY) {
            return 'w';
        }
        return 'm';
    }

///////////////�ж�m/////////////////////////////////////////////////////////////////
    else if(xCount == 1 && yCount == 5) {
        return 'w';

    }

///////////////�ж�		b,c,p,q	,t,d,t///////////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 1) {
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 1) {
            yPos = minY + deltaY * 1 / 4;
            yCount = getCount(start, end, yPos, y);
            temp = getTemp(start, end, y, x, yPos);
            if(yCount == 1) {
                if(temp > mX) {
                    return 'd';

                }
                else {
                    return 'b';
                }

            }
            else {
                return 'y';
            }
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

///////////////�ж�		o	,p,q,x	/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 2) {
        float dis;
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount < 2) {
            yPos = minY + deltaY / 4;
            yCount = getCount(start, end, yPos, y);
            temp = getTemp(start, end, y, x, yPos);
            if(temp < mX) {
                return 'p';
            }
            else {
                return 'q';
            }

        } else {
            if(yCount == 1) {

                return 'b';
            }
            else {
                dis = getDis(start, end, y, x, mY);
                if(dis / deltaX > 0.5) {
                    return 'o';
                } else {
                    return 'x';
                }

            }
        }

    }

///////////////�ж�		a,b,d,x,		/////////////////////////////////////////////////////////////////
    else if(xCount == 2 && yCount == 3) {

        yPos = minY + deltaY / 4;
        yCount = getCount(start, end, yPos, y);

        if(yCount == 1) {
            temp = getTemp(start, end, y, x, yPos);
            if(temp > mX) {
                return 'd';

            }
            return 'b';
        }

        else if(yCount == 3) {
            yPos = minY + deltaY * 3 / 4;
            yCount = getCount(start, end, yPos, y);

            if(yCount == 3) {
                return 'a';
            } else if(yCount == 3) {
                return 'x';
            }
            else if(yCount == 1) {}

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

            if(x[end] > mX) {
                return 'z';
            }
            else {
                return 's';
            }


        }
    }

///////////////�ж�		e,y		/////////////////////////////////////////////////////////////////
    else if(xCount == 3 && yCount == 2) {
        if(x[end] > mX) {
            return 'e';
        }
        else {
            return 'y';
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

/*******************************************************************************
*        ��ȡ��д��ĸ
*   name:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Parameter:    cnt:
*   Return:         cnt:
*******************************************************************************/
char getUperCase(int x[200], int y[200], int point) {

    int start, end, deltaX, deltaY, mX, mY, j, xCount , yCount , minX, maxX, minY, maxY, yPos, xPos , temp;
    arrLength = point;
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

    //////////////////�ж�L,J,P,T,Y,F////////////////////////////////////
    if(xCount == 1 && yCount == 1) {
        float dis;
        xPos = minX + deltaX * 3 / 4;

        yCount  = getCount(start, end , xPos, x);
        if(xCount > 1) {
            return 'P';
        }
        else {
            temp = getTemp(start, end, x , y, xPos);
            if(temp > mY && x[end] > xPos) {
                return 'L';
            }
            else {
                xPos = minX + deltaX / 4;
                temp = getTemp(start, end, x, y, xPos);

                if(temp > mY) {

                    return 'J';
                }
                else {
                    yPos = minY + deltaY / 4;
                    yCount = getCount(start, end, yPos, y);
                    if(yCount > 1) {
                        return 'Y';
                    }
                    else {
                        yPos = minY + deltaY * 3 / 4;
                        dis = getTemp(start, end, y, x, yPos);
                        xPos = minX + deltaX / 4;
                        if(temp < xPos) {
                            return 'F';
                        }
                        else {
                            return 'T';
                        }

                    }
                }

            }

        }
    }
    //////////////////�ж�H,U,V,Y////////////////////////////////////
    else if(xCount == 1 && yCount == 2) {
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount == 1) {
            return 'Y';

        }
        else {
            temp = getTemp(start, end, x, y, mX);
            if(temp < maxY - 20) {
                yPos = minY + deltaY / 4;
                if(getDis(start, end, y, x, yPos) / deltaX > 0.400) {

                    return 'H';
                }
                else {
                    return 'A';
                }

            }
            else {
                yPos = minY + deltaY * 3 / 4;
                if(getDis(start, end, y, x, yPos) / deltaX > 0.400) {
                    return 'U' ;
                }
                else {
                    return 'V';
                }


            }

        }


    }

    //////////////////�ж�N,////////////////////////////////////
    else if(xCount == 1 && yCount == 3) {
        return 'N';

    }

    //////////////////�ж�M,W////////////////////////////////////
    else if(xCount == 1 && yCount == 4) {

        if(y[0] < mY) {
            return 'W';
        }
        return 'M';
    }

    //////////////////�ж�C,F,I,J,P,Z////////////////////////////////////
    else if(xCount == 2 && yCount == 1) {

        float dis;

        yPos = minY + deltaY / 4;
        yCount = getCount(start, end, yPos, y);
        dis = deltaX;
        if(x[end] > mX) {
            if(dis / deltaY > 0.6) {
                if(yCount == 1) {
                    if(x[0] < mX) {
                        return 'Z';
                    } else {
                        return 'C';
                    }

                }
                return 'K';
            }
            else {
                xPos = minX + deltaX * 3 / 4;
                dis =  getDis(start, end, x, y, xPos);
                if(dis / deltaY > 0.7) {
                    return 'I';
                }
                else {
                    yPos = minY + deltaY * 3 / 4;
                    yCount = getCount(start, end, yPos, y);
                    if(yCount == 1) {
                        return 'F';
                    }
                }
            }
        }
        else {
            return 'P';

        }
    }

    //////////////////�ж�A,D,K,O,X,P////////////////////////////////////
    else if(xCount == 2 && yCount == 2) {
        float dis;
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        printf("\nyCount:%d", yCount);
        if(yCount < 2) {
            return 'P';


        }
        else {
            if(y[end] < mY) {
                return 'O';
            }
            else {
                if(x[end] < mX) {//�ж�D,O,X
                    dis  = getDis(start, end, x, y, mX - 10);
                    if(dis / mY > 0.4) {
                        return 'D';
                    }
                    else {
                        return 'X';
                    }

                } else {
                    if(y[end] > yPos) {
                        return 'K';

                    } else {
                        return 'A';
                    }

                }
            }


        }
    }

    //////////////////�ж�E,G,S,Z,////////////////////////////////////
    else if(xCount == 3 && yCount == 1) {

        yPos = minY + deltaY / 4;
        for(j = start; j < end; j++) {
            if(hasIntersection(y, j, yPos)) {
                temp = x[j];

            }
        }
        if(temp > mX) {
            return 'Z';

        }
        else {
            yPos = minY + deltaY * 3 / 4;
            yCount = getCount(start, end, yPos, y);
            if(yCount > 1) {
                return 'G';

            }
            else {
                for(j = start; j < end; j++) {
                    if(hasIntersection(y, j, yPos)) {
                        temp = x[j];
                    }
                }
                if(temp > mX) {
                    return 'S';

                }
                else {
                    return 'E';
                }

            }
        }
    }

    //////////////////�ж�R,B////////////////////////////////////
    else if(xCount == 3 && yCount == 2) {

        xPos = minX + deltaX * 3 / 4;
        xCount = getCount(start, end, xPos, x);
        yPos = minY + deltaY * 4 / 5;
        yCount = getCount(start, end, yPos, y);
        if(xCount > 3) {
            return 'B';

        }
        else { //�ж�Q,R
            if(yCount > 2) {
                return 'Q';
            }
            else {
                return 'R';
            }
        }
    }
    //////////////////�ж�R,B////////////////////////////////////
    else if(xCount == 3 || xCount == 4 && yCount == 3) {
        return 'Q';
    }
    //////////////////�ж�B,////////////////////////////////////
    else if(xCount == 4 && yCount == 1) {

        return 'B';
    }

    //////////////////�ж�B,Q,////////////////////////////////////
    else if(xCount == 4 && yCount == 2) {
        yPos = minY + deltaY * 3 / 4;
        yCount = getCount(start, end, yPos, y);
        if(yCount > 2 && x[end] > mX) {
            return 'Q';
        }
        return 'B';
    }

    return ' ';


}


