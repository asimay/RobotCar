#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

int thresh = 100;
RNG rng(12345);
int main(int argc, char** argv)
{
	cv::Mat bak_img,src_img,orig;
	cv::VideoCapture capture;
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	capture.open(0);
	int i=30;	
	while(--i>0){
	capture >> bak_img;
	}	
	cv::cvtColor(bak_img,bak_img, CV_BGR2GRAY);
	GaussianBlur( bak_img, bak_img, Size( 31, 31 ),0,0 ); //31 31
	//cv::threshold(bak_img, bak_img,15,255,CV_THRESH_BINARY); 
	Mat canny_output;
  	vector<vector<Point> > contours;
  	vector<Vec4i> hierarchy;
	Mat image;
	while(true) {
		capture >> src_img;
		orig = src_img;
		cv::cvtColor(src_img,src_img, CV_BGR2GRAY);
		GaussianBlur( src_img, src_img, Size( 31, 31 ),0,0 );
		
		cv::absdiff(bak_img,src_img,src_img); 
		cv::threshold(src_img,src_img,35,255,CV_THRESH_BINARY); //35
		//cv::threshold(src_img,src_img,70.0f,255,CV_THRESH_BINARY);
		//GaussianBlur( src_img, src_img, Size( 5, 5 ),0,0 );
		//cv::GaussianBlur(src_img, image, cv::Size(25, 25), 3);
		//cv::addWeighted(src_img, 1.5, image, -0.5, 0, src_img);		
		//Canny( src_img, src_img, thresh, thresh*2, 3 );
		

		cv::Mat elementCLOSE(5,5,CV_8U,cv::Scalar(255,255,255));
    		cv::morphologyEx(src_img,src_img,cv::MORPH_CLOSE,elementCLOSE);
		
    		cv::findContours(src_img,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);		
		

		//findContours(src_img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  		vector<Moments> mu(contours.size() );
  		for( int i = 0; i < contours.size(); i++ )
     		{ mu[i] = moments( contours[i], false ); }
  		vector<Point2f> mc( contours.size() );
  		for( int i = 0; i < contours.size(); i++ )
     		{ mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }
		Mat drawing = Mat::zeros( src_img.size(), CV_8UC3 );
		double max = 0 ;  
		int maxi = 0;	
		Scalar color = Scalar( 200,200,200 ); 	

		
		for( int i = 0; i< contours.size(); i++ )
     		{				
			//double t_len = arcLength(contours[i],0); 			
			double t_len = contourArea( contours[i],0);			
			if(t_len > max) {max= t_len;    maxi=i;} 

			
		} 
		vector<vector<Point> > contours_poly( contours.size() );
  		vector<Rect> boundRect( contours.size() );
  		vector<Point2f>center( contours.size() );
  		vector<float>radius( contours.size() );
		if(contours.size()){
		approxPolyDP( Mat(contours[maxi]), contours_poly[0], 3, true );      		
		boundRect[0] = boundingRect( Mat(contours_poly[0]) );
       		minEnclosingCircle( (Mat)contours_poly[0], center[0], radius[0] );
		

		drawContours( drawing, contours, maxi, color, 2, 8, hierarchy, 0, Point() );
		//drawContours( drawing, contours_poly, 0, color, 1, 8, vector<Vec4i>(), 0, Point() );
       		rectangle( drawing, boundRect[0].tl(), boundRect[0].br(), color, 2, 8, 0 );
       		//circle( drawing, center[0], (int)radius[0], color, 2, 8, 0 );
		if(boundRect[0].tl().x>0)
		{Size size(200+boundRect[0].tl().x,boundRect[0].tl().y+100);
		resize(orig,orig,size);}

		}
		imshow( "Contours", drawing );
		imshow( "OriginalScaled", orig );
		int c = cv::waitKey(1);
		if( c == 27) break;
	}
	return 0;
}
