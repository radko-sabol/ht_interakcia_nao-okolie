// Standardne hlavickove subory
#include <iostream>
#include <math.h>
#include <string.h>

// OpenCV hlavickove subory
#include <opencv/cv.h>
#include <opencv/highgui.h>


CvHaarClassifierCascade* loadObjectDetector(const char* cascadePath);

void haarDetectAndDrawObjects(IplImage* image, CvHaarClassifierCascade* cascade, int do_pyramids);