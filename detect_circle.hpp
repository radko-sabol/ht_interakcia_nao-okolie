/**
* DEV
*/


// Standardne hlavickove subory
#include <iostream>
#include <math.h>
#include <string.h>

// OpenCV hlavickove subory
#include <opencv/cv.h>
#include <opencv/highgui.h>

void onMouseClick(int event, int x, int y, int flags, void* image);

void cvOpen(const CvArr *src, CvArr *dst, IplConvKernel *element);

void cvClose(const CvArr *src, CvArr *dst, IplConvKernel *element);

void circleDetectAndDrawObjectsC(const IplImage* imageMain);

void circleDetectAndDrawObjects(cv::Mat& img);