//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;


Mat fgMaskMOG1; 
Mat fgMaskMOG2; 
Mat fgMaskMOG3; 
Mat fgMaskMOG4; 
Ptr<BackgroundSubtractor> pMOG1; 
Ptr<BackgroundSubtractor> pMOG2; 
Ptr<BackgroundSubtractor> pMOG3; 
Ptr<BackgroundSubtractor> pMOG4; 
int keyboard;

CvCapture* capture1 = 0;
CvCapture* capture2 = 0;
CvCapture* capture3 = 0;
CvCapture* capture4 = 0;
Mat frame1, frame2,frame3, frame4;
int thresh = 100;
RNG rng(12345);

void processVideo();

int main(int argc, char* argv[])
{
    capture1 = cvCaptureFromCAM( 0 );
    //capture2 = cvCaptureFromCAM( 1 );
    //capture3 = cvCaptureFromCAM( 2 );
    //capture4 = cvCaptureFromCAM( 3 );
    namedWindow("FG Mask MOG 1");
    namedWindow("FG Mask MOG 2");
    cvNamedWindow( "cam1", 1 );
    cvNamedWindow( "cam2", 1 );
    cvNamedWindow( "cam3", 1 );
    cvNamedWindow( "cam4", 1 );
    cvNamedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    pMOG1= new BackgroundSubtractorMOG2(); 
    pMOG2 = new BackgroundSubtractorMOG2();
    pMOG3= new BackgroundSubtractorMOG2(); 
    pMOG4 = new BackgroundSubtractorMOG2();        
    processVideo();
    destroyAllWindows();
    return EXIT_SUCCESS;
}

void processVideo() {
  while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        frame1 = cvQueryFrame( capture1 );
	//frame2 = cvQueryFrame( capture2);
 	//frame3 = cvQueryFrame( capture3);
	//frame4 = cvQueryFrame( capture4);
	if( !frame1.empty()/*&&!frame2.empty()&&!frame3.empty()&!frame4.empty()*/ ) {
	    	pMOG1->operator()(frame1, fgMaskMOG1);
        	pMOG2->operator()(frame1, fgMaskMOG2);
		pMOG3->operator()(frame1, fgMaskMOG3);
		pMOG4->operator()(frame1, fgMaskMOG4);

		Mat canny_output;
  		vector<vector<Point> > contours;
  		vector<Vec4i> hierarchy;
		Mat image;
		//GaussianBlur( fgMaskMOG1, fgMaskMOG1, Size( 25, 25 ),0,0 );
		cv::GaussianBlur(fgMaskMOG1, image, cv::Size(25, 25), 3);
		cv::addWeighted(fgMaskMOG1, 1.5, image, -0.5, 0, fgMaskMOG1);		
		Canny( fgMaskMOG1, canny_output, thresh, thresh*2, 3 );
  		findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		vector<vector<Point> >hull( contours.size() );
		for( int i = 0; i < contours.size(); i++ )
		{  convexHull( Mat(contours[i]), hull[i], false ); }
  		vector<Moments> mu(contours.size() );
  		for( int i = 0; i < contours.size(); i++ )
     		{ mu[i] = moments( contours[i], false ); }
  		vector<Point2f> mc( contours.size() );
  		for( int i = 0; i < contours.size(); i++ )
     		{ mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }

  		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		double max = 0 ;  
		int maxi = 0;		
		for( int i = 0; i< contours.size(); i++ )
     		{				
			double t_len = arcLength(hull[i],0); 			
			if(t_len > max) {max= t_len;    maxi=i;} 
			
			
		} 
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) ); 		
		if(contours.size()){
			//drawContours( drawing, hull, maxi, color, 1, 8, vector<Vec4i>(), 0, Point() );	
       			drawContours( drawing, contours, maxi, color, 2, 8, hierarchy, 0, Point() );
       			circle( drawing, mc[maxi], 4, color, -1, 8, 0 );
			cout<<(float)mc[maxi].x<<endl;   		
			Size size((int)mc[maxi].x,(int)mc[maxi].y);
			resize(frame1,frame1,size);
		}			
		
  		imshow( "Contours", drawing );
        	imshow("cam1", frame1);
		imshow("cam2", frame1);
		imshow("cam3", frame1);
		imshow("cam4", frame1);
        	imshow("FG Mask MOG 1", fgMaskMOG1);
        	imshow("FG Mask MOG 2", fgMaskMOG2);
	}        
        keyboard = waitKey( 1 );
    }
}
