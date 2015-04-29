#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "egbis/image.h"
#include "egbis/misc.h"
#include "egbis/pnmfile.h"
#include "egbis/segment-image.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

/****
 * OpenCV C++ Wrapper using the Mat class
 * TODO: Move this to separate file.
 ***/
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

/****
 * END OF: OpenCV C++ Wrapper using the Mat class
 ***/

Mat egbisImage;
Mat img;
char* imageName;
int num_ccs;

//egbisImage = runEgbisOnMat(&img, sigma_value, (float)k_switch_value, (float)c_switch_value, &num_ccs);