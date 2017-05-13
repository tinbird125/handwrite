/***��Ҷ˹****/

#include "bayes.h"
#include "lpc17xx.h"
#include "uart.h"
#include "stdio.h"
#include "recognize.h"
/*******************************************************************************
*       ���������     				                          
*   name: p1                                             		    	          
*   Parameter:i    cnt: ����    			                         
*   Parameter:N    cnt: ����i����������   				          
*   Parameter:Ni   cnt: ��������            				          
*   Return: p1     cnt: ����������� p1                                                            	          
*******************************************************************************/

float p1(int i, int Ni, int N) { 
    float p1;
    p1 = Ni / N;
    return p1;
}

/*******************************************************************************
*        x�ĵ�j������Ϊ1�ĸ���     				                          
*   name:                                        		    	          
*   Parameter:    cnt:    			                         
*   Parameter:    cnt:     				          
*   Parameter:    cnt:               				          
*   Return:         cnt:                                                            	          
*******************************************************************************/

double p2(int i, int Ni, int j,int future[20][25]) { 					
    int k;									//��Ʒ����
    long xkj = 0;
    double  p2;
	
    for(k = 0; k < Ni; k++) {
        xkj += future[k][j];
    }
		
    p2 = (float)(xkj + 1) / (Ni + 2);
    return p2;
}

/*******************************************************************************
*        ����������     				                          
*   name:                                        		    	          
*   Parameter:    cnt:    			                         
*   Parameter:    cnt:     				          
*   Parameter:    cnt:               				          
*   Return:         cnt:                                                            	          
*******************************************************************************/
double p3(int i,int Ni,int future[20][25]) { 								
    int j;
    double mul=1;
    double p3;
    for(j = 0; j < 25; j++) {
            mul *= p2(i,Ni,j,future);
            p3 = mul;
       
    }
		
    return p3;
}
/*******************************************************************************
*        �������			                          
*   name:                                        		    	          
*   Parameter:    cnt:    			                         
*   Parameter:    cnt:     				          
*   Parameter:    cnt:               				          
*   Return:         cnt:                                                            	          
*******************************************************************************/
double  p4(int i	,int Ni,int N,int future[20][25]) { 												
    int k;
    double  temp = 0;
    double  p4;
	
    for(k = 0; k < 10; k++) {
        temp +=	p1(k,Ni,N)*p3(k,Ni,future);
    }
		
    p4 =  p1(i,Ni,N) * p3(i,Ni,future)/ temp;
		
    return p4;

}
