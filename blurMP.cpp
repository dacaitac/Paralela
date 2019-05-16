#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <omp.h>

using namespace cv;
using namespace std;

typedef struct params{
	int** matrix;	
    int ini;
    int end;
    int filter;
    int dimx;
    int dimy;
} params;

struct params *kernel(struct params *p){    
    int i, j, k, l;
    
    int** img = p->matrix;
    int y1 = p->ini;
    int y2 = p->end;
    int filter = p->filter;
    
    int rad = filter/2; 
    int y = y2 - y1;

    int dimy = p->dimy;
    int dimx = p->dimx;

    if(y1 == 0){ y1 += rad; }    
    if(y2 == dimy){ y2 -= rad; }    

    int** ret = 0;
    ret = new int*[ y ];    
    for ( i = 0; i < y; i++){
        ret[i] = new int[ dimx ];
        for (j = 0; j < dimx; j++){
            ret[i][j] = 0;
        }
    }

    for(i = y1; i < y2; i++){
        for(j = rad; j < dimx-rad; j++){
            int val = 0;
            for(k = i-rad; k < i+rad; k++ ){
                for(l = j-rad; l < j+rad; l++){
                    val += img[k][l];
                }
            }
            img[i][j] = val/(rad*rad);    
        }   
    }

    for(i = rad; i < y-rad; i++){
        for(j = 0; j < dimx; j++){
            ret[i][j] = img[i+y1][j];
        }
    }

    struct params *retParams;
    retParams = (struct params *)malloc(sizeof(params));
    retParams->matrix = ret;
    retParams->ini = y1;
    retParams->end = y2;
        
    return retParams;
}

int main(int argc, char** argv )
{   
    int i, j, k;
    int NUMTHREADS = atoi( argv[2] );
    int filter = atoi( argv[3] );        

    Mat imageIn;
    Mat imageOut;

    imageIn = imread( argv[1], IMREAD_COLOR );       
    imageOut = imread( argv[1], IMREAD_COLOR );

    int dimy = imageIn.rows;
    int dimx = imageIn.cols;

    if ( !imageIn.data ){
        printf("No image data \n");
        return -1;
    }
    
    int** imgMatrix = 0;
    imgMatrix = new int*[dimy];    
    for ( i = 0; i < dimy; i++){
        imgMatrix[i] = new int[dimx];
        for (j = 0; j < dimx; j++){
            imgMatrix[i][j] = imageIn.at<Vec3b>(i, j)[0];  
        }
    }

    struct params inter[NUMTHREADS][NUMTHREADS];
    int distr = dimy/NUMTHREADS;

   	#pragma omp parallel num_threads(NUMTHREADS)
    {
        int th = omp_get_thread_num();
        struct params *sendParams;
        sendParams = (struct params *)malloc(sizeof(params));
        sendParams->dimx = dimx;
        sendParams->dimy = dimy;
        sendParams->ini = th * distr;
		sendParams->end = sendParams->ini + distr;
        sendParams->matrix = imgMatrix;
        sendParams->filter = filter;

		inter[th][0] = *kernel(sendParams);        
    }

    for(k = 0; k < NUMTHREADS; k++){
        struct params tmp = inter[k][0];
        for ( i = 0; i < distr; i++ ) {	
            for(j = 0 ; j < dimx ; j++){
                imageOut.at<Vec3b>(k*distr + i, j)[0] = tmp.matrix[i][j];                
            }
        }
    }
       
    imwrite( "out.jpg", imageOut );    
    
    return 0;
}
