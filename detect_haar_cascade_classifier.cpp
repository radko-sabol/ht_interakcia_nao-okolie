// Standardne hlavickove subory
#include <iostream>
#include <math.h>
#include <string.h>

// OpenCV hlavickove subory
#include <opencv/cv.h>
#include <opencv/highgui.h>

// Ostatne hlavickove subory
#include "detect_haar_cascade_classifier.hpp"


CvHaarClassifierCascade* loadObjectDetector(const char* cascadePath)
{
  return (CvHaarClassifierCascade*)cvLoad(cascadePath);
}

void haarDetectAndDrawObjects(IplImage* image, CvHaarClassifierCascade* cascade, int do_pyramids)
{
  CvSize size = cvGetSize(image);
  IplImage* imageCircle = cvCreateImage(size, image->depth, image->nChannels);
  cvCopy(image, imageCircle, NULL);
  IplImage* smallImage = imageCircle;
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* objects;
  int i,
      scale = 1;

  /* if the flag is specified, down-scale the input image to get a
      performance boost w/o loosing quality (perhaps) */
  if(do_pyramids)
  {
    smallImage = cvCreateImage(cvSize(imageCircle->width/2, imageCircle->height/2), IPL_DEPTH_8U, 3);
    cvPyrDown(image, smallImage, CV_GAUSSIAN_5x5);
    scale = 2;
  }

  /* use the fastest variant */
  objects = cvHaarDetectObjects(smallImage, cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(0, 0));
  //objects = cvHaarDetectObjects(smallImage, cascade, storage, 1.2, 2, CV_HAAR_DO_ROUGH_SEARCH, cvSize(0, 0));
  
  /* draw all the rectangles */
  for(i = 0; i < objects->total; i++)
  {
    /* extract the rectanlges only */
    CvRect objectRect = *(CvRect*)cvGetSeqElem(objects, i);
    if ((objectRect.width > 20) && (objectRect.width < 40))
    {
      std::cout << "Haar object detected" << std::endl;
      std::cout << "Objekt cislo: " << (i + 1) << " - X: " << objectRect.x << "; Y: " << objectRect.y << "; height: " << objectRect.height << "; width: " << objectRect.width << std::endl << std::endl;;
      //cvRectangle(image, cvPoint(face_rect.x*scale, face_rect.y*scale), cvPoint((face_rect.x+face_rect.width)*scale, (face_rect.y+face_rect.height)*scale), CV_RGB(255,0,0), 3);

      cv::Point center(objectRect.x + objectRect.width*0.5, objectRect.y + objectRect.height*0.5);
      cvEllipse(imageCircle, center, cvSize(objectRect.width*0.5, objectRect.height*0.5), 0, 0, 360, cvScalar(255, 0, 0), 2, 8, 0 );
    }
  }

  cvShowImage("Video haar", imageCircle);
  if (smallImage != imageCircle) { cvReleaseImage(&smallImage); }
  cvReleaseImage(&imageCircle);
  cvReleaseMemStorage(&storage);
}
