#include <cmath>
#include <iostream>
#include <cuda_runtime_api.h>
#include <cuda.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/gpu/gpu.hpp"

using namespace std;
using namespace cv;
using namespace cv::gpu;


static cv::Mat drawHoughLinesOnMat (cv::gpu::GpuMat hough_Mat, cv::gpu::GpuMat houghLines)
{
    cv::Mat output_Mat;
    cv::cvtColor(cv::Mat(hough_Mat), output_Mat, CV_GRAY2BGR);

    std::vector<cv::Vec4i> lines_vector;
    if (!houghLines.empty())
    {
        lines_vector.resize(houghLines.cols);
	cv::Mat temp_Mat(1, houghLines.cols, CV_32SC4, &lines_vector[0]);
	houghLines.download(temp_Mat);
    }

    for (size_t i=0; i<lines_vector.size(); ++i)
    {
        cv::Vec4i l = lines_vector[i];
        cv::line(output_Mat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, 8);
    }

    return output_Mat;
}


int main()
{
    cv::Mat input = cv::imread("out.png", CV_LOAD_IMAGE_GRAYSCALE);
    //std::string imageType = getImgType(input.type());

    cv::gpu::GpuMat mat_input(input), bil_out, mat_thresh, hough_lines;
    cv::gpu::HoughLinesBuf hough_buffer;

    int bilateral_thresh = 15; // 5 == 0.085s; 15 == 0.467s at run-time
    //cv::gpu::bilateralFilter(mat_input, bil_out, bilateral_thresh, bilateral_thresh*2, bilateral_thresh/2);
    //cv::gpu::threshold(bil_out, mat_thresh, 10, 255, CV_THRESH_BINARY);
    //cv::gpu::Canny(bil_out, mat_thresh, 10, 60, 5);

    cv::gpu::HoughLinesP(mat_input, hough_lines, hough_buffer, 1.0f, (float)(CV_PI/180.0f), 5, 1);
    //cv::Mat test_hough(hough_lines);
    cv::Mat hough_Mat = drawHoughLinesOnMat(mat_input, hough_lines);
    //cv::gpu::HoughLines(mat_thresh, hough_lines,  1.0f, (float)(CV_PI/180.0f),   1, true);
    //   /*cv::Mat */hough_Mat = drawHoughLinesOnMat(mat_input, hough_lines);
	imshow("res",hough_Mat);
	waitKey(0);
    return 0;
}