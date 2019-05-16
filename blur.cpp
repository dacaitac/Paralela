#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <pthread.h>

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

void *kernel(void *pArg){    
    int i, j, k, l;
    struct params *p;
	p = (struct params *)pArg;
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

    struct params *retParams = (struct params *)malloc(sizeof(params));
    retParams->matrix = ret;
    retParams->ini = y1;
    retParams->end = y2;
    
    pthread_exit( (void*)retParams ) ;   
    return 0;
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

    pthread_t tHandles[NUMTHREADS];    
    struct params inter[NUMTHREADS][NUMTHREADS];
    int distr = dimy/NUMTHREADS;
	for ( k = 0; k < NUMTHREADS; k++ ) {
        inter[k][0].dimx = dimx;
        inter[k][0].dimy = dimy;
        inter[k][0].ini = k * distr;
		inter[k][0].end = inter[k][0].ini + distr;
        inter[k][0].matrix = imgMatrix;
        inter[k][0].filter = filter;
		int err = pthread_create(&tHandles[k], NULL, kernel, (void*)&inter[k][0]);
        if(err != 0){ cout << "Error in pthread"; }
	}   

    for ( i = 0; i < NUMTHREADS; i++ ) {
        struct params *getParams;
		pthread_join(tHandles[i], (void **) &getParams);
        inter[i][0] = *getParams;
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
