/**
* DEV
*/


#include <iostream>
#include <math.h>
#include <string.h>

#include <QDebug>

#include <opencv/cv.h>
#include <opencv/highgui.h>

class ObjectDetection
{
  private:
    /** Suradnice najdenych objektov */
    std::vector< std::vector< std::vector<cv::Scalar> > > m_objects;
    cv::CascadeClassifier m_haarCascade;

    double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

  public:
    void circleDetectObjects(cv::Mat& img);
    void squareDetectObjects(const cv::Mat& image);
    boolean loadHaarObjectDetector(std::string cascadePath);
    void haarDetectObjects(cv::Mat image);
    void drawDetectedObjects(cv::Mat &image);
    std::vector< std::vector<cv::Scalar> > getObjects(int index);
    void clearObjects();
};
