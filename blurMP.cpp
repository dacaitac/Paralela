#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <time.h>
#include <omp.h>

using namespace cv;
using namespace std;

typedef struct params{
    int **imgMatrix;
    int ini;
    int end;
    int filter;
    int dimx;
    int dimy;
} params;

struct params *kernel(struct params *p){    
    int i, j, k, l;
    
    Mat img;
    img = Mat(p->dimx, p->dimy, CV_8UC(3), p->imgMatrix);
    int y1 = p->ini;
    int y2 = p->end;
    int filter = p->filter;
    
    int rad = filter/2; 
    int y = y2 - y1;

    int dimy = img.cols;
    int dimx = img.rows;

    if(y1 == 0){ y1 += rad; }    
    if(y2 == dimy){ y2 -= rad; }    

    Mat ret(dimy, dimx, CV_8UC(3), Scalar::all(0));

    for(i = y1; i < y2; i++){
        for(j = rad; j < dimx-rad; j++){
            Vec3b pixel = 0;
            for(k = i-rad; k < i+rad; k++ ){
                for(l = j-rad; l < j+rad; l++){
                    pixel.val[0] += img.at<Vec3b>(k,l)[0];
                    pixel.val[1] += img.at<Vec3b>(k,l)[1];
                    pixel.val[2] += img.at<Vec3b>(k,l)[2];
                }
            }
            img.at<Vec3b>(i,j) = pixel;
        }   
    }

    for(i = rad; i < y-rad; i++){
        for(j = 0; j < dimx; j++){
            ret.at<Vec3b>(i,j) = img.at<Vec3b>(i+y1,j);
        }
    }
    
    struct params *retParams;
    retParams = (struct params *)malloc(sizeof(params));
    // retParams->matrix = ret;
    retParams->ini = y1;
    retParams->end = y2;
        
    return retParams;
}

int main(int argc, char** argv )
{   
    int i, j, k, l;
    struct timespec ts1, ts2;

    int NUMTHREADS = atoi( argv[2] );
    int filter = atoi( argv[3] );        

    Mat imageIn;
    Mat imageOut;

    imageIn = imread( argv[1], CV_LOAD_IMAGE_COLOR );       
    imageOut = imread( argv[1], CV_LOAD_IMAGE_COLOR );

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

    clock_gettime(CLOCK_REALTIME, &ts1);
   	#pragma omp parallel num_threads(NUMTHREADS)
    {
        int th = omp_get_thread_num();
        struct params *sendParams;
        sendParams = (struct params *) malloc(sizeof(params));
        sendParams->ini = th * distr;
		sendParams->end = sendParams->ini + distr;
        sendParams->imgMatrix = imgMatrix;
        sendParams->filter = filter;
        sendParams->dimx = dimx;
        sendParams->dimy = dimy;

		inter[th][0] = *kernel(sendParams);        
    }

    // for(k = 0; k < NUMTHREADS; k++){
    //     struct params tmp = inter[k][0];
    //     for ( i = 0; i < distr; i++ ) {	
    //         for(j = 0 ; j < dimx ; j++){
    //             for(l = 0; l < imageIn.channels(); l++){
    //                 imageOut.at<Vec3b>(k*distr + i, j)[l] = tmp.matrix.at<Vec3b>(i,j)[l];                
    //             }
    //         }
    //     }
    // }

    clock_gettime(CLOCK_REALTIME, &ts2);
   // double secs = (double)(t_fin-t_ini)*1000/CLOCKS_PER_SEC;
    printf("%ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec),
         abs(ts2.tv_nsec - ts1.tv_nsec));   
    imwrite( "out.jpg", imageOut );    
    
    return 0;
}
