#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <time.h>
#include <omp.h>
#include <math.h>

using namespace cv;
using namespace std;

#define PI 3.1459265358
#define e  2.7182818284
#define sigma 1.9

IplImage *imageIn;
IplImage *imageOut;
float **gauss;

struct params{
    int ***image;    
    int ini;
    int end;
    int filter;
    int dimx;
    int dimy;
};


void *kernel(struct params *p){    
    int i, j, k, l, m;                   // Indices para los ciclos
    
    // Separa los atributos que llegan por parámetro
    int y1      = p->ini;
    int y2      = p->end;
    int dimy    = p->dimy;
    int dimx    = p->dimx;
    int filter  = p->filter;
    
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

    cout << "Dimension: " << dimx << "x"  << dimy << endl;

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
    
    // La imagen se divide, según la cantidad de filas que tenga
    // entre la cantidad de hilos con los que se va a trabajar.
    int distr = dimy/NUMTHREADS;                            

    // Se toma el tiempo de inicio de la ejecución principal del programa
    clock_gettime(CLOCK_REALTIME, &ts1);                    
   	#pragma omp parallel num_threads(NUMTHREADS)                
    {
        int th = omp_get_thread_num();          // ID del hilo en ejecución

        // Se agrupan los parámetros que se necesitan para procesar la imagen
        // para luego enviarlos al kernel de ejecución
        struct params *sendParams;
        sendParams = (struct params *) malloc(sizeof(params));
        // Se indica el inicio y final de la porción de imagen con la que va a trabajar el hilo
    
        sendParams->ini     = th * distr;                        
		sendParams->end     = sendParams->ini + distr -1;        
        sendParams->filter  = filter;           
        sendParams->dimx    = dimx;
        sendParams->dimy    = dimy;

        // Luego de agrupar los parámetros de ejecución del kernel en "sendParams"
        // se envían y se guarda el resultado en el primer arreglo de "inter"
		kernel( sendParams );   
    }

    #pragma omp barrier

    
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
