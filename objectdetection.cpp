#include "objectdetection.h"

ObjectDetection::ObjectDetection()
{
    m_cascade = NULL;
    m_image = 0;
    m_object = NULL;
    m_storage = NULL;
}

void ObjectDetection::loadObjectDetector(const char* cascadePath)
{
    m_cascade = (CvHaarClassifierCascade*)cvLoad(cascadePath);
}

CvSeq* ObjectDetection::detectTemplate(IplImage* image)
{
    m_storage = cvCreateMemStorage(0);

    m_object = cvHaarDetectObjects( image, m_cascade, m_storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING );

    return m_object;
}

int scale = 1;

void ObjectDetection::drawTemplate(IplImage* image)
{
    for(int i = 0; i < m_object->total; i++ )
    {
        CvRect face_rect = *(CvRect*)cvGetSeqElem( m_object, i );
        cvRectangle(image, cvPoint(face_rect.x * scale, face_rect.y * scale), cvPoint((face_rect.x + face_rect.width) * scale, (face_rect.y + face_rect.height) * scale), CV_RGB(255,0,0), 3);
    }

    cvReleaseMemStorage(&m_storage);
}
