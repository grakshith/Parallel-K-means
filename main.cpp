#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;
int main(int argc, char *argv[])
{
    Mat img = imread("lena.jpg");
    //printf("Hi there");
    cout << img.size[1];
    //cout << img;
    double sum=0;
    //cout << img;
    for(int i = 0; i < img.rows; i++)
    {
    for(int j = 0; j < img.cols; j++)
        cout << (int)img.at<Vec3b>(i,j)[0] << " " << (int)img.at<Vec3b>(i,j)[1] << " " << (int)img.at<Vec3b>(i,j)[2] << std::endl;
    break;


    }
    cout << sum;
//    for(int i=0;i<512;i++)
//        for(int j=0;j<512;j++)
//        printf("%d", img[i][j]);
    if(img.empty())
       return -1;
    namedWindow( "lena", 0 );
    imshow("lena", img);

    waitKey(0);
    return 0;
}



