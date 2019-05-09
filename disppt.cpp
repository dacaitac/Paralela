#include <stdio.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

struct params{
	int ini;
	int fin;
};



const int dimx = 810 , dimy = 540;
const int cx = 3;
int gb=0;


int ma1[cx][cx];
int ma2[dimx][dimy];
int resp[dimx][dimy];


int mult(int ma2[cx][cx]){
    int i , j , tot=0, dem=0;
    for(j=0;j<cx;j++){
        for(i=0;i<cx;i++){
            tot+=ma1[i][j]*ma2[i][j];
        }
    }
    return tot;
}

int convi(int ax,int ay){
    int i , j , tot=0;
    int part[cx][cx];
    if((ax>=(cx/2) && ay>=(cx/2))){
        if(ax>=dimx-(cx/2) || ay>=dimy-(cx/2)){
            tot=ma2[ax][ay];
        }else{
            for(j=(-cx/2);j<=cx/2;j++){
                for(i=(-cx/2);i<=cx/2;i++){
                    //printf("%d %d %d\n",ax+i,ay+j,ma2[ax+i][ay+j]);
                    part[i+(cx/2)][j+(cx/2)]=ma2[ax+i][ay+j];
                }
            }
            tot=mult(part);
        }
    }else{
        tot=ma2[ax][ay];
    }
    return tot;
}

void *cutter( void *pArg){
	struct params *p;
	p = (struct params *)pArg;
	int i,j;
	//printf("%d %d\n",p->ini,p->fin);
    for(j = p->ini ; j < p-> fin ; j++){
        for(i = 0 ; i < dimx ; i++){
            resp[i][j]=convi(i,j);
        }
    }

	return 0;
}


int main(int argc, char** argv )
{
    int i , j , k;
    int conv[cx][cx];
    // if ( argc != 2 )
    // {
    //     printf("usage: DisplayImage.out <Image_Path>\n");
    //     return -1;
    // }

    for (j = 0; j < cx; j++){
        for (i = 0; i < cx; i++){
            conv[i][j]=1;
        }
    }
    
    Mat image;
    Mat imagin;
    image = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );    
    imagin = imread( argv[2], CV_LOAD_IMAGE_GRAYSCALE );
	int NUMTHREADS = atoi(argv[3]);
	int repa = dimy/NUMTHREADS;
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    //IplImage* img = cvLoadImage("greatwave.png", 1);
    Mat mtx(image); // convert IplImage* -> Mat

    //mtx.create(81,81, CV_8UC(1));
    cout << "M = "<< endl << " "  << mtx << endl << endl;
	
	pthread_t tHandles[NUMTHREADS];
	
    int color[dimx][dimy];
    for (j = 0; j < dimy; j++){
        for (i = 0; i < dimx; i++){
            color[i][j]=image.at<uchar>(Point(i, j));
        }
    }
	for(j=0;j<cx;j++){
		for(i=0;i<cx;i++){
			ma1[i][j]=conv[i][j];
		}	
	}
	for(j=0;j<dimy;j++){
		for(i=0;i<dimx;i++){
			ma2[i][j]=color[i][j];
		}	
	}

	struct params inter[NUMTHREADS];

	for ( k = 0; k < NUMTHREADS; k++ ) {
		inter[k].ini=k*repa;
		inter[k].fin=inter[k].ini+repa;
		//printf("%d %d\n\n",inter[i].ini,inter[i].fin);
		pthread_create(&tHandles[i], NULL, cutter, (void*)&inter[i]);
	}

	for ( i = 0; i < NUMTHREADS; ++i ) {
		pthread_join(tHandles[i], NULL);
	}
	
	for(j = 0 ; j < dimy ; j++){
        for(i = 0 ; i < dimx ; i++){
            imagin.at<uchar>(Point(i, j))=resp[i][j];
        }
    }
    
    imwrite(argv[2], imagin);

    return 0;
}
