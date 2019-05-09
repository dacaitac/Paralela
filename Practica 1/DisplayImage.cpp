#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    
    Mat image;
    image = imread( argv[1], 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    //IplImage* img = cvLoadImage("greatwave.png", 1);
    Mat mtx(image); // convert IplImage* -> Mat

    //mtx.create(81,81, CV_8UC(1));
    cout << "M = "<< endl << " "  << mtx << endl << endl;
    
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}