#include<vector>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/objdetect/objdetect.hpp>

#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;


int main(int argc, char *argv[])
{
    cv::Mat frame;                                              
    cv::Mat fg;     
    cv::Mat blurred;
    cv::Mat thresholded;
    cv::Mat gray;
    cv::Mat blob;
    cv::Mat bgmodel;                                            
    cv::namedWindow("Frame");   
    cv::namedWindow("Background Model");
    cv::namedWindow("Blob");
    cv::VideoCapture cap(0);    
	cv::Mat bak_img,src_img;
    cv::BackgroundSubtractorMOG2 bgs;      

    //bgs.nmixtures = 3;
    //bgs.history = 1000;
    //bgs.varThresholdGen = 15;
    //bgs.bShadowDetection = true;                            
    //bgs.nShadowDetection = 0;                               
    //bgs.fTau = 0.5;                                         

std::vector<std::vector<cv::Point> > contours;  
int i=20;
while(--i>0){             
cap >> bak_img;
}	
cv::GaussianBlur(bak_img,bak_img,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
cv::cvtColor(bak_img,bak_img, CV_BGR2GRAY);
for(;;)
{
    cap >> frame;                                           

    cv::GaussianBlur(frame,blurred,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);

    //bgs.operator()(blurred,fg);                         
    //bgs.getBackgroundImage(bgmodel);

	
	cv::cvtColor(frame,frame, CV_BGR2GRAY);
	
	//GaussianBlur( frame, frame, Size( 5, 5 ),0,0 );
	cv::absdiff(bak_img,frame,frame); 
	                             
	
    //cv::imshow("Frame",fg);
    //cv::imshow("Background Model",bgmodel);

    cv::threshold(/*fg*/frame,thresholded,70.0f,255,CV_THRESH_BINARY);
	

    cv::Mat elementCLOSE(5,5,CV_8U,cv::Scalar(255,255,255));
    cv::morphologyEx(thresholded,thresholded,cv::MORPH_CLOSE,elementCLOSE);
	
    cv::findContours(thresholded,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
    cv::imshow("Frame",thresholded);
cv::cvtColor(thresholded,blob,CV_GRAY2RGB);
    cv::drawContours(blob,contours,-1,cv::Scalar(1),-1,8);
	cv::imshow("Blob",blob);
    
    int keyboard = waitKey( 1 );
	
	

}

}
