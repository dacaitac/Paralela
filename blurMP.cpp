#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <time.h>
#include <omp.h>
#include <math.h>

using namespace cv;
using namespace std;

struct params{
    int ***image;    
    int ini;
    int end;
    int filter;
    int dimx;
    int dimy;
    int ch;
};

struct params *kernel(struct params *p){    
    int i, j, k, l, m;                   // Indices para los ciclos
    
    // Separa los atributos que llegan por parámetro
    int ***img  = p->image;    
    int y1      = p->ini;
    int y2      = p->end;
    int dimy    = p->dimy;
    int dimx    = p->dimx;
    int ch      = p->ch;
    int filter  = p->filter;
    
    int rad = filter/2;                 // El "radio" de la matriz de convolcion
    int y   = y2 - y1;                  // La dimensión en Y de la "subimagen" 
                                        // con la que va a trabajar este hilo

    int cant    = filter * filter;      // Cantidad de elementos operados en la matriz de convolución 
    int upY     = rad;                  // Alcance hacia arriba de la matriz de convolución
    int downY   = rad;                  // Alcance hacia abajo de la matriz de convolución
    int leftX   = rad;                  // Alcance hacia la izquierda de la matriz de convolución
    int rightX  = rad;                  // Alcance hacia la derecha de la matriz de convolución

    // Si el inicio de la convolución es en Y=0, desplaza el 
    // inicio o el final de la convolución para evitar que se desborde la imagen
    if(y1 == 0){ y1 += rad; }                                                   
    if(y2 == dimy){ y2 -= rad; }  
    
    // ------------------------------CONVOLUCIÓN--------------------------------------------------
    // Se aplica la convolución para cada canal de colores de la imagen.
    // Desde el Main se envía toda la imagen porque se necesitan pixeles
    // que estan por fuera de los limites de sector de imagen con que se va a trabajar.
    for(k = 0; k < ch; k++){
        for(i = y1; i < y2; i++){                            // Límites en Y del sector a trabajar
            for(j = 0; j < dimx; j++){                       // Límites en X del sector a trabajar
                int pixel = 0;                               
                // Para cada pixel, se trabaja con los vecinos que estan en el radio
                // definido en las variables upY, downY, rightX, leftX
                for(m = i-upY; m < i+downY; m++ ){           
                    for(l = j-leftX; l < j+rightX-1; l++){  
                        // Se suman todos los pixeles en el radio
                        pixel += img[k][m][l];               
                    }
                }                
                // Con el promedio de valores de los pixeles en el radio, se sobreescribe la imagen
                // con el nuevo valor calculado
                img[k][i][j] = pixel / cant;                 
            }
        }
    }
    // Luego de sobreescribir los valores de la imagen con los que se va a trabajar
    // se devuelve solamente el sector de la imagen con la que trabajó este hilo.
    // TODO: Esta puede ser una posible causa de las lineas horizontales    
    if(y1-rad == 0){
        y1 = 0;
    }

    if(y2+rad >= dimy){
        y2 = dimy;
    }
    int*** ret = 0;    
    ret = new int**[ch];
    for(k = 0; k < ch; k++){
        ret[k] = new int*[y];
        for(i = 0; i < y; i++){
            ret[k][i] = new int[dimx];
            for(j = 0; j < dimx; j++){
                ret[k][i][j] = img[k][i+y1][j];
            }
        }
    }

    // Se agrupan los datos de lo procesado y los retorna.
    struct params *retParams;
    retParams = (struct params*)malloc(sizeof(params));
    retParams->image  = ret;
    retParams->ini    = y1;
    retParams->end    = y2;
        
    return retParams;
}

int main(int argc, char** argv ){   
    int i, j, k, l;                                 // Indices para los ciclos
    struct timespec ts1, ts2;                       // Variables para tomar el tiempo

    int NUMTHREADS  = atoi( argv[2] );              // Lee la cantidad de hilos desde la entrada
    int filter      = atoi( argv[3] );              // Lee el tamaño de la matriz de convolucion
	//NUMTHREADS=NUMTHREADS+1;
    Mat imageIn;
    Mat imageOut;
    imageIn  = imread( argv[1], IMREAD_COLOR );     // Lee el nombre de la imagen a trabajar
    imageOut = imread( argv[1], IMREAD_COLOR );     // Para tener las mismas dimensiones se asigna
                                                    // la imagen de entrada a la de salida
    
    int dimy = imageIn.rows;                        // Dimension en Y de la imagen a trabajar
    int dimx = imageIn.cols;                        // Dimension en X de la imagen a trabajar
    int ch   = imageIn.channels();                  // Cantidad de canales que tiene la imagen
    
    if ( !imageIn.data ){                           // Valida la imagen
        printf("No image data \n");
        return -1;
    }

    // Se guarda la imagen en una matriz tridimensional
    // El parámetro "k" indica cada canal de colores de la imagen
    // El parámetro "i" indica la dimensión de la imagen en Y
    // El parámetro "j" indica la dimensión de la imagen en X    
    int*** image = 0;                               
    image = new int**[ch];
    for(k = 0; k < ch; k++){
        image[k] = new int*[dimy];
        for(i = 0; i < dimy; i++){
            image[k][i] = new int[dimx];
            for(j = 0; j < dimx; j++){
                image[k][i][j] = imageIn.at<Vec3b>(i, j)[k];
            }
        }
    }   

    // Se hace un arreglo bidimensional para guardar el resultado
    // de la ejecución de cada hilo. La segunda dimensión se hace
    // para evitar errores de "Flase sharing".
    struct params inter[NUMTHREADS][NUMTHREADS];    
    
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
		sendParams->end     = sendParams->ini + distr;        
        sendParams->image   = image;            
        sendParams->filter  = filter;           
        sendParams->dimx    = dimx;
        sendParams->dimy    = dimy;
        sendParams->ch      = ch;
        
        // Luego de agrupar los parámetros de ejecución del kernel en "sendParams"
        // se envían y se guarda el resultado en el primer arreglo de "inter"
		inter[th][0] = *kernel( sendParams );   
    }


    // Cada sector de la imagen procesado por cada hilo quedó guardado en el arreglo Inter
    // este bloque recorre ese arreglo para luego guardarlo en la nueva imagen.
    for(l = 0; l < NUMTHREADS; l++){
        struct params tmp = inter[l][0];
        for(k = 0; k < ch; k++){
            for(i = 0; i < distr-1; i++){
                for(j = 0; j < dimx; j++){
                    imageOut.at<Vec3b>(l*distr + i, j)[k] = tmp.image[k][i][j];
                }
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &ts2);            // Se toma el tiempo total de ejecución para luego imprimirlo.
    printf("%ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec), abs(ts2.tv_nsec - ts1.tv_nsec));   
    imwrite( "out.jpg", imageOut );    
    
    return 0;
}
