/**
* DEV
*/

// Standardne hlavickove subory
#include <iostream>
#include <math.h>
#include <string.h>
#include <QDebug>
// OpenCV hlavickove subory
#include <opencv/cv.h>
#include <opencv/highgui.h>

class ObjectDetection
{
  private:
    /** Suradnice najdenych objektov */
    std::vector<std::vector<std::vector<cv::Scalar>>> m_objects;
    cv::CascadeClassifier m_haarCascade;

    // circle
    //void cvOpen(const CvArr *src, CvArr *dst, IplConvKernel *element);
    //void cvClose(const CvArr *src, CvArr *dst, IplConvKernel *element);

    // square
    double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0 );

  public:
    // circle
    //void circleDetectAndDrawObjectsC(const IplImage* imageMain);
    void circleDetectObjects(cv::Mat& img);

    // square
    void squareDetectObjects(const cv::Mat& image);
    //void squareDrawObjects(cv::Mat& image, const std::vector<std::vector<cv::Point>>& squares);

    // haar
    void loadHaarObjectDetector(std::string cascadePath);
    //void haarDetectAndDrawObjectsC(IplImage* image, CvHaarClassifierCascade* cascade, int do_pyramids, std::vector<std::vector<std::vector<cv::Scalar>>>& objects);
    void haarDetectObjects(cv::Mat image);

    // vykreslenie najdenych objektov
    void ObjectDetection::drawDetectedObjects(cv::Mat &image);

    std::vector<std::vector<cv::Scalar>> ObjectDetection::getObjects(int index);
};
