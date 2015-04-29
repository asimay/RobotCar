#include <opencv2/core/core.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>

#include <cstdio>
#include <cstdlib>
#include "egbis/image.h"
#include "egbis/misc.h"
#include "egbis/pnmfile.h"
#include "egbis/segment-image.h"





using namespace std;
using namespace cv;
int p1,p2,p3,res;
Mat egbisImage;
Mat img;
char* imageName;
int num_ccs;
void detectAndDraw( Mat& img );
image<rgb>* convertMatToNativeImage(Mat *input);
Mat convertNativeToMat(image<rgb>* input);
Mat runEgbisOnMat(Mat *input, float sigma, float k, int min_size, int *numccs);
void laneMarkingsDetector(cv::Mat &srcGRAY, cv::Mat &dstGRAY, int tau);



int main( int argc, const char** argv )
{
    CvCapture* capture = 0;
    Mat frame, frameCopy, image,markings;
    string inputName;
	res = atoi(argv[1]);
	p1=20;
	p2=50;
	p3=50;
_start_:
	//capture = cvCaptureFromCAM( 0 );
	//image = imread( "road.png" );
	capture = cvCaptureFromAVI( "video.avi" );
            
	cvNamedWindow( "result", 1 );
	//cvNamedWindow( "markings", 1 );

    if( capture )
    {	
		IplImage* iplImg;
        cout << "In capture mode" << endl;
        for(;;)
        {
            frame = cvQueryFrame( capture );
           
            if( frame.empty() ) goto _start_;
			//egbisImage = runEgbisOnMat(&frame, 0.6,1990,1990, &num_ccs);
			//num_ccs=0;
			//cv::imshow( "result", egbisImage );
			//waitKey(1);
            cvtColor(frame, frame, CV_BGR2GRAY);
            markings = frame.clone();
			
			
			const int64 start = getTickCount();
			laneMarkingsDetector(frame, markings, res);
			const double timeSec = (getTickCount() - start) / getTickFrequency();
			cv::threshold(markings, markings, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			//cv::imshow( "result", markings  );
			detectAndDraw( markings );
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

		// goto _cleanup_;
        }

        waitKey(0);

		_cleanup_:
        cvReleaseCapture( &capture );
    }
    else
    {
        cout << "In image read mode" << endl;
        if( !image.empty() )
        {
            //egbisImage = runEgbisOnMat(&image, 0.6,1990,1990, &num_ccs);
			//cv::imshow( "result", egbisImage );
			//cv::imshow( "result", image );
			cvtColor(image, image, CV_BGR2GRAY);
            markings = image.clone();
			int res = atoi(argv[1]);
			cout << atoi(argv[1]) << endl;
			const int64 start = getTickCount();
			laneMarkingsDetector(image, markings, res);
			const double timeSec = (getTickCount() - start) / getTickFrequency();
  
			cout << "CPU Time : " << timeSec * 1000 << " ms" << endl;
			cv::imshow( "markings", markings );
			waitKey(0);
			detectAndDraw( markings );
            

        }
    }

    cvDestroyWindow("result");

    return 0;
}

void detectAndDraw( Mat& src )
{
    Mat  blur,dst,cdst; 
	//cv::blur( src, blur, cv::Size(5,5) );
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







image<rgb>* convertMatToNativeImage(Mat *input){
    int w = input->cols;
    int h = input->rows;
    image<rgb> *im = new image<rgb>(w,h);

    for(int i=0; i<input->rows; i++)
    {
        for(int j=0; j<input->cols; j++)
        {
            rgb curr;
            Vec3b intensity = input->at<Vec3b>(i,j);
            curr.b = intensity.val[0];
            curr.g = intensity.val[1];
            curr.r = intensity.val[2];
            im->data[i+j*w] = curr;
        }
    }
    return im;
}

Mat convertNativeToMat(image<rgb>* input){
    int w = input->width();
    int h = input->height();
    Mat output(Size(w,h),CV_8UC3);

    for(int i =0; i<w; i++){
        for(int j=0; j<h; j++){
            rgb curr = input->data[i+j*w];
            output.at<Vec3b>(i,j)[0] = curr.b;
            output.at<Vec3b>(i,j)[1] = curr.g;
            output.at<Vec3b>(i,j)[2] = curr.r;
        }
    }

    return output;
}

Mat runEgbisOnMat(Mat *input, float sigma, float k, int min_size, int *numccs) {
    int w = input->cols;
    int h = input->rows;
    Mat output(Size(w,h),CV_8UC3);

    // 1. Convert to native format
    image<rgb> *nativeImage = convertMatToNativeImage(input);
    // 2. Run egbis algoritm
    image<rgb> *segmentetImage = segment_image(nativeImage, sigma, k, min_size, numccs);//sigma = 0.6; k = 1990;  min_size=1990;
    // 3. Convert back to Mat format
    output = convertNativeToMat(segmentetImage);

    return output;
}
