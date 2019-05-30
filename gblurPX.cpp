#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <pthread.h>

using namespace cv;
using namespace std;

#define PI 3.1459265358
#define e  2.7182818284
#define sigma 1.9

IplImage *imageIn;
IplImage *imageOut;
float **gauss;

struct params{
    int ini;
    int end;
    int filter;
    int dimx;
    int dimy;
};

void *kernel(void *pArg){    
    int i, j, k, l, m;
    struct params *p;
	p = (struct params *)pArg;

    // Separa los atributos que llegan por parámetro
    int y1 = p->ini;
    int y2 = p->end;
    int dimy = p->dimy;
    int dimx = p->dimx;
    int filter = p->filter;
    
    int rad = filter/2;                 // El "radio" de la matriz de convolcion
    //----------------- CONVOLUCION ----------------------
    //i = Reccorre en y
    //j = Recorre en x
    //m = Extiende x
    //l = Extiende l
    for(i = y1; i < y2; i++){
        for(j = 0; j < dimx; j++){
            CvScalar pixelIn;
            CvScalar pixelOut;

            double blue  = 0.0;
            double green = 0.0;
            double red   = 0.0;

            for (m = -rad; m <= rad; m++){
                for (l = -rad; l <= rad; l++){
                    int posX;
                    int posY;
                    if( j+m < 0) posX = j+m * -1;
                    else if(j+m >= dimx ) posX = j-m;
                    else posX = j+m;

                    if(i+l < 0) posY = i+l * -1;
                    else if(i+l >= dimy ) posY = i-l;
                    else posY = i+l;
                    
                    pixelIn = cvGet2D(imageIn, posY, posX);
                    blue  += pixelIn.val[0] * gauss[m+rad][l+rad];
                    green += pixelIn.val[1] * gauss[m+rad][l+rad];
                    red   += pixelIn.val[2] * gauss[m+rad][l+rad];
                }
            }
            pixelOut = cvGet2D(imageOut, i, j);
            pixelOut.val[0] = blue;
            pixelOut.val[1] = green;
            pixelOut.val[2] = red;

            cvSet2D(imageOut, i, j, pixelOut);
        }
    }
    return 0;
}

int main(int argc, char** argv ){
    int i, j, k, l;                                 // Indices para los ciclos
    struct timespec ts1, ts2;                       // Variables para tomar el tiempo

    int NUMTHREADS  = atoi( argv[2] );              // Lee la cantidad de hilos desde la entrada
    int filter      = atoi( argv[3] );              // Lee el tamaño de la matriz de convolucion
    
    imageIn  = cvLoadImage( argv[1], CV_LOAD_IMAGE_COLOR);  // Carga la imagen a trabajar
    imageOut = cvCloneImage( imageIn );                     // Clona la imagen para imprimir

    int dimy = imageIn->height;
    int dimx = imageIn->width;   

    gauss = (float**) malloc(filter * sizeof(float *));
    for(i = 0; i < filter; i++){
        gauss[i] = (float *) malloc(filter * sizeof(float));
    }

    // Se llena la matriz de convolucion gaussiana
    float sum = 0;
    float v = sigma * sigma;
    for( i = 0; i < filter; i++){
        for( j = 0; j < filter; j++){
            gauss[i][j] = (1/(2*PI*v)) * pow(e, -(pow(floor(filter/2),2)+pow(floor(filter/2),2))) /(2*v);
            sum += gauss[i][j];
        }
    }

    for( i = 0; i < filter; i++){
        for(j = 0; j < filter; j++){
            gauss[i][j] /= sum;
        }
    }

    int distr = dimy/NUMTHREADS;

    pthread_t tHandles[NUMTHREADS];    
    struct params inter[NUMTHREADS][NUMTHREADS];
    clock_gettime(CLOCK_REALTIME, &ts1);                    
	for ( k = 0; k < NUMTHREADS; k++ ) {
        inter[k][0].dimx = dimx;
        inter[k][0].dimy = dimy;
        inter[k][0].ini = k * distr;
		inter[k][0].end = inter[k][0].ini + distr;
        inter[k][0].filter = filter;
		int err = pthread_create(&tHandles[k], NULL, kernel, (void*)&inter[k][0]);
        if(err != 0){ cout << "Error in pthread"; }
	}   

    for ( i = 0; i < NUMTHREADS; i++ ) {
        struct params *getParams;
		pthread_join(tHandles[i], NULL);
        inter[i][0] = *getParams;
	}

    // Se toma el tiempo total de ejecución para luego imprimirlo.
    clock_gettime(CLOCK_REALTIME, &ts2);            
    printf("%ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec), abs(ts2.tv_nsec - ts1.tv_nsec));   
    cvSaveImage( "out.jpg", imageOut );    
    cvWaitKey(0); 


    //Liberamos memoria de la matriz gaussiana
    for( i = 0; i < filter; i++){
        float *cur = gauss[i];
        free(cur);
    }
    free(gauss); 
    
    return 0;
}
