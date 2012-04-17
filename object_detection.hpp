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

    // square
    double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0 );

  public:
    // circle
    void circleDetectObjects(cv::Mat& img);

    // square
    void squareDetectObjects(const cv::Mat& image);

    // haar
    boolean loadHaarObjectDetector(std::string cascadePath);
    void haarDetectObjects(cv::Mat image);

    // vykreslenie najdenych objektov
    void drawDetectedObjects(cv::Mat &image);

<<<<<<< HEAD
    std::vector<std::vector<cv::Scalar> > ObjectDetection::getObjects(int index);
=======
    std::vector< std::vector<cv::Scalar> > getObjects(int index);
>>>>>>> 9949da25f1d91a08e386fd1fa7c33e2a321fb44a
    void clearObjects();
};
