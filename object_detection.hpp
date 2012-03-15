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

class ObjectDetection
{
  public:
    /** Suradnice najdenych objektov */
    std::vector<std::vector<std::vector<cv::Scalar>>> objects;

    // circle
    void onMouseClick(int event, int x, int y, int flags, void* image);
    void cvOpen(const CvArr *src, CvArr *dst, IplConvKernel *element);
    void cvClose(const CvArr *src, CvArr *dst, IplConvKernel *element);
    void circleDetectAndDrawObjectsC(const IplImage* imageMain);
    void circleDetectObjects(cv::Mat& img, std::vector<std::vector<std::vector<cv::Scalar>>>& objects);

    // square
    double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0 );
    void squareDetectObjects(const cv::Mat& image, std::vector<std::vector<std::vector<cv::Scalar>>>& objects);
    void squareDrawObjects(cv::Mat& image, const std::vector<std::vector<cv::Point>>& squares);

    // haar
    CvHaarClassifierCascade* loadObjectDetector(const char* cascadePath);
    void haarDetectAndDrawObjectsC(IplImage* image, CvHaarClassifierCascade* cascade, int do_pyramids, std::vector<std::vector<std::vector<cv::Scalar>>>& objects);
    void haarDetectObjects(cv::Mat image, cv::CascadeClassifier cascade, std::vector<std::vector<std::vector<cv::Scalar>>>& objects);
};