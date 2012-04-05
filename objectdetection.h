#ifndef OBJECTDETECTION_H
#define OBJECTDETECTION_H

#include <iostream>
#include <math.h>
#include <string.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>


class ObjectDetection
{
private:
    CvHaarClassifierCascade* m_cascade;
    cv::Mat m_image;
    CvSeq* m_object;
    CvMemStorage* m_storage;
public:
    ObjectDetection();

    //haar
    void loadObjectDetector(const char* cascadePath);
    void drawTemplate(IplImage* image);
    CvSeq* detectTemplate(IplImage* image);

};

#endif // OBJECTDETECTION_H
