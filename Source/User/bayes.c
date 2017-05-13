/***贝叶斯****/

#include "bayes.h"
#include "lpc17xx.h"
#include "uart.h"
#include "stdio.h"
#include "recognize.h"
/*******************************************************************************
*       求先验概率     				                          
*   name: p1                                             		    	          
*   Parameter:i    cnt: 种类    			                         
*   Parameter:N    cnt: 种类i的样本个数   				          
*   Parameter:Ni   cnt: 样本总数            				          
*   Return: p1     cnt: 返回先验概率 p1                                                            	          
*******************************************************************************/

float p1(int i, int Ni, int N) { 
    float p1;
    p1 = Ni / N;
    return p1;
}

/*******************************************************************************
*        x的第j个分量为1的概率     				                          
*   name:                                        		    	          
*   Parameter:    cnt:    			                         
*   Parameter:    cnt:     				          
*   Parameter:    cnt:               				          
*   Return:         cnt:                                                            	          
*******************************************************************************/

double p2(int i, int Ni, int j,int future[20][25]) { 					
    int k;									//样品总数
    long xkj = 0;
    double  p2;
	
    for(k = 0; k < Ni; k++) {
        xkj += future[k][j];
    }
		
    p2 = (float)(xkj + 1) / (Ni + 2);
    return p2;
}

/*******************************************************************************
*        类条件概率     				                          
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
*        后验概率			                          
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
