#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <ctime>

#include "IPM.h"

using namespace cv;
using namespace std;

int p1,p2,p3,res;

void detectAndDraw( Mat& src );
void laneMarkingsDetector(cv::Mat &srcGRAY, cv::Mat &dstGRAY, int tau);


int main( int _argc, char** _argv )
{
	// Matrix
	Mat inputImg, inputImgGray;
	Mat markings;
	Mat outputImg;
	
	p1=30;
	p2=10;
	p3=200;
	res=20;
	cvNamedWindow( "result", 1 );
	
	if( _argc != 2 )
	{
		cout << "input as argument!" << endl;
		return 1;
	}

	// Video
	string videoFileName = _argv[1];	
	cv::VideoCapture video;
	if( !video.open(videoFileName) )
		return 1;

	// Show video information
	int width = 0, height = 0, fps = 0, fourcc = 0;	
	width = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	height = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));
	fps = static_cast<int>(video.get(CV_CAP_PROP_FPS));
	fourcc = static_cast<int>(video.get(CV_CAP_PROP_FOURCC));

	cout << "Input video: (" << width << "x" << height << ") at " << fps << ", fourcc = " << fourcc << endl;
	
	
	vector<Point2f> origPoints;
	origPoints.push_back( Point2f(0, height-100) );
	origPoints.push_back( Point2f(width, height-100) );
	origPoints.push_back( Point2f(width, height-150) );
	origPoints.push_back( Point2f(0, height-150) );

	//origPoints.push_back( Point2f(width-300, height-150) );
	//origPoints.push_back( Point2f(width, height-70) );
	
	//origPoints.push_back( Point2f(width, height-170) );
	//origPoints.push_back( Point2f(width-300, height-250) );

	

	
	vector<Point2f> dstPoints;
	dstPoints.push_back( Point2f(0, height) );
	dstPoints.push_back( Point2f(width, height) );
	dstPoints.push_back( Point2f(width, 0) );
	dstPoints.push_back( Point2f(0, 0) );
		
	// IPM object
	IPM ipm( Size(width, height), Size(width, height), origPoints, dstPoints );
	
	// Main loop
	int frameNum = 0;
	for( ; ; )
	{
		printf("FRAME #%6d ", frameNum);
		fflush(stdout);
		frameNum++;

		// Get current image		
		video >> inputImg;
		if( inputImg.empty() )
			break;

		 // Color Conversion
		 if(inputImg.channels() == 3)		 
			 cvtColor(inputImg, inputImgGray, CV_BGR2GRAY);				 		 
		 else	 
			 inputImg.copyTo(inputImgGray);			 		 

		 // Process
		 clock_t begin = clock();
		//cv::GaussianBlur( inputImgGray, inputImgGray, cv::Size(1,1),0,0 );
		 ipm.applyHomography( inputImgGray, inputImgGray );		 
		 clock_t end = clock();
		 double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		 printf("%.2f (ms)\r", 1000*elapsed_secs);	
		 ipm.drawPoints(origPoints, inputImg );

		 // View		
		 imshow("Input", inputImg);
		 imshow("Output", inputImgGray);

markings = inputImgGray.clone();
laneMarkingsDetector(inputImgGray, markings, res);
cv::threshold(markings, markings, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);



		detectAndDraw(markings); //inputImgGray
		int key = waitKey( 1 );
            if( key >= 0 )
			{
				cout << key << endl;
				if(key=='q') p1++;
				if(key=='a') p1--;
				if(key=='w') p2++;
				if(key=='s') p2--;
				if(key=='e') p3++;
				if(key=='d') p3--;
				if(key=='r') res++;
				if(key=='f') res--;
			}
	 cout<<p1<<" "<<p2<<" "<<p3<<" "<<res<<endl;

		 
	}

	return 0;	
}	

void detectAndDraw( Mat& src )
{
    Mat  blur,dst,cdst; 
   //cv::GaussianBlur( src, src, cv::Size(1,1),10,0 );
 Canny(src, src, 50, 200, 3);
 cvtColor(src, cdst, CV_GRAY2BGR);

 if (0){
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 100, 50, 50 );

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }
}
 else{
  vector<Vec4i> lines;
   const int64 start = getTickCount();

/*
-dst: Output of the edge detector. It should be a grayscale image (although in fact it is a binary one)
-lines: A vector that will store the parameters (x_{start}, y_{start}, x_{end}, y_{end}) of the detected lines
-rho : The resolution of the parameter r in pixels. We use 1 pixel.
-theta: The resolution of the parameter \theta in radians. We use 1 degree (CV_PI/180)
-threshold: The minimum number of intersections to “detect” a line
-minLinLength: The minimum number of points that can form a line. Lines with less than this number of points are disregarded.
-maxLineGap: The maximum gap between two points to be considered in the same line. //10 7 25 //20 25 40

*/
  HoughLinesP(src, lines, 1, CV_PI/180, p1, p2, p3 );
  const double timeSec = (getTickCount() - start) / getTickFrequency();
  cout<<p1<<" "<<p2<<" "<<p3<<" "<<res<<endl;
  //cout << "CPU Time : " << timeSec * 1000 << " ms" << endl;
  
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
	double Angle = atan2(l[3] - l[1], l[2] - l[0]) * 180.0 / CV_PI;
	if((Angle>30 && Angle< 135) ||(Angle<-30&&Angle>-135) )
	//cout << "Line angle : " << Angle<< " deg" << endl;
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
  }
   
 }
 
 //imshow("source", src);
 //imshow("detected lines", cdst);
 cv::imshow( "result", cdst );
}

void laneMarkingsDetector(cv::Mat &srcGRAY, cv::Mat &dstGRAY, int tau){
	dstGRAY.setTo(0);
	//cv::imshow( "markings", srcGRAY);
	
	int aux = 0;
	for (int j=0; j<srcGRAY.rows; ++j)
		{
		const unsigned char *ptRowSrc = srcGRAY.ptr<uchar>(j);
		unsigned char *ptRowDst = dstGRAY.ptr<uchar>(j);
		
		for (int i=tau; i<srcGRAY.cols-tau; ++i)
			{   
				
				if(ptRowSrc[i] != 0)
				{
						aux = 2 * ptRowSrc[i];
						aux += -ptRowSrc[i-tau];
						aux += -ptRowSrc[i+tau];
						aux += -abs((int)(ptRowSrc[i-tau] - ptRowSrc[i+tau]));
						
						aux = (aux<0)?(0):(aux); 
						aux = (aux>255)?(255):(aux);
						
						
						ptRowDst[i] = (unsigned char)aux;
						
				}
			
			}
		
		
		}
	
	
	
	
}
