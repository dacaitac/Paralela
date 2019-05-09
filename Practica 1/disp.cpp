#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


const int dimx = 810 , dimy = 540;
const int cx = 3;

int mult(int ma1[cx][cx],int ma2[cx][cx]){
    int i , j , tot=0, dem=0;
    for(j=0;j<cx;j++){
        for(i=0;i<cx;i++){
            tot+=ma1[i][j]*ma2[i][j];
            dem+=ma2[i][j];
        }
    }
    return tot;
}

int convi(int ma1[cx][cx],int ma2[dimx][dimy],int ax,int ay){
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
            tot=mult(part,ma1);
        }
    }else{
        tot=ma2[ax][ay];
    }
    return tot;
}


int main(int argc, char** argv )
{
    
    int resp[dimx][dimy];
    int i , j;
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
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    //IplImage* img = cvLoadImage("greatwave.png", 1);
    Mat mtx(image); // convert IplImage* -> Mat

    //mtx.create(81,81, CV_8UC(1));
    cout << "M = "<< endl << " "  << mtx << endl << endl;

    int color[dimx][dimy];
    for (j = 0; j < dimy; j++){
        for (i = 0; i < dimx; i++){
            color[i][j]=image.at<uchar>(Point(i, j));
        }
    }

    for(j = 0 ; j < dimy ; j++){
        for(i = 0 ; i < dimx ; i++){
            resp[i][j]=convi(conv,color,i,j);
            imagin.at<uchar>(Point(i, j))=resp[i][j];
        }
    }
    
    imwrite(argv[2], imagin);

    return 0;
}