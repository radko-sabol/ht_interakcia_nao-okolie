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

// Ostatne hlavickove subory
#include "detect_circle.hpp"

void onMouseClick(int event, int x, int y, int flags, void* image)
{
  if (event == CV_EVENT_LBUTTONDOWN) 
  {
    CvScalar s = cvGet2D(image, x, y);
    //std::cout << "H:" << s.val[0] << "; S:" << s.val[1] << "; V:" << s.val[2] << std::endl;
    /*
    std::cout << "H:" << hsv_min_h << "; S:" << hsv_min_s << "; V:" << hsv_min_v << std::endl;
    hsv_min_h = min(hsv_min_h, s.val[0]);
    hsv_min_s = min(hsv_min_s, s.val[1]);
    hsv_min_v = min(hsv_min_v, s.val[2]);
    */
  }
  else if (event == CV_EVENT_RBUTTONDOWN) 
  {
    CvScalar s = cvGet2D(image, x, y);
    //std::cout << "H:" << s.val[0] << "; S:" << s.val[1] << "; V:" << s.val[2] << std::endl;
    /*
    std::cout << "H:" << hsv_max_h << "; S:" << hsv_max_s << "; V:" << hsv_max_v << std::endl;
    hsv_max_h = max(hsv_max_h, s.val[0]);
    hsv_max_s = max(hsv_max_s, s.val[1]);
    hsv_max_v = max(hsv_max_v, s.val[2]);
    */
  }
}

void cvOpen(const CvArr *src, CvArr *dst, IplConvKernel *element)
{
	cvErode (src, dst, element, 1);
	cvDilate(src, dst, element, 1);
}

void cvClose(const CvArr *src, CvArr *dst, IplConvKernel *element)
{
	cvDilate(src, dst, element, 1);
	cvErode (src, dst, element, 1);
}

void circleDetectAndDrawObjects(cv::Mat& img) // vela falosnych detekcii
{
  cv::Mat gray, gray2, imageHSV;
  //cvtColor(img, gray, CV_BGR2GRAY);
  cv::cvtColor(img, imageHSV, CV_BGR2HSV);
  cv::inRange(imageHSV, cv::Scalar(0, 50, 170, 0), cv::Scalar(10, 180, 256, 0), gray);
  cv::inRange(imageHSV, cv::Scalar(170, 50, 170, 0), cv::Scalar(256, 180, 256, 0), gray2);
  cv::bitwise_or(gray, gray2, gray);
  // smooth it, otherwise a lot of false circles may be detected
  cv::GaussianBlur(gray, gray, cv::Size(9, 9), 1, 1);
  cv::Canny(gray, gray, 1, 3);
  std::vector<cv::Vec3f> circles;
  //HoughCircles(gray, circles, CV_HOUGH_GRADIENT,
  //             2, gray.rows/4, 200, 100 );

  cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, gray.rows/4, 200, 100, 5, 50);
  for (size_t i = 0; i < circles.size(); i++)
  {
    std::cout << circles[i][2] << std::endl; // radius
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    // draw the circle center
    cv::circle(img, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
    // draw the circle outline
    cv::circle(img, center, radius, cv::Scalar(0,0,255), 3, 8, 0);
  }

  cv::imshow( "circless", img );
  cv::imshow( "circles", gray );
}

void circleDetectAndDrawObjectsC(const IplImage* imageMain) // nefunguje podla mojich predstav
{
  CvSize imageSize = cvGetSize(imageMain);
  int imageDepth = imageMain->depth,
      imageChannels = imageMain->nChannels;

  IplImage* imageCircle = cvCreateImage(imageSize, imageDepth, imageChannels); // klon vstupneho obrazka
  cvCopy(imageMain, imageCircle, NULL); // skopirovanie obrazka do novej premennej

  IplImage* imageHSV = cvCreateImage(imageSize, imageDepth, imageChannels); // farbeny obrazok v HSV
  //IplImage* imageTresholded1 = cvCreateImage(imageSize, imageDepth, 1); // ciernobiely obrazok
  //IplImage* imageTresholded2 = cvCreateImage(imageSize, imageDepth, 1); // ciernobiely obrazok

  /** Vytvorenie masky */
  CvMat *mask = cvCreateMat(imageSize.height, imageSize.width, CV_8UC1);
  CvMat *imageTresholded1 = cvCreateMat(imageSize.height, imageSize.width, CV_8UC1);
  CvMat *imageTresholded2 = cvCreateMat(imageSize.height, imageSize.width, CV_8UC1);

  /** Konverzia obrazu do HSV*/
  cvCvtColor(imageCircle, imageHSV, CV_BGR2HSV);

  /** Prahovanie farieb na Aibo-vu lopticku */
  CvScalar hsv_min = cvScalar(0, 50, 170, 0);
  CvScalar hsv_max = cvScalar(10, 180, 256, 0);
  CvScalar hsv_min2 = cvScalar(170, 50, 170, 0);
  CvScalar hsv_max2 = cvScalar(256, 180, 256, 0);

  cvInRangeS(imageHSV, hsv_min, hsv_max, imageTresholded1);
  cvInRangeS(imageHSV, hsv_min2, hsv_max2, imageTresholded2);
  cvOr(imageTresholded1, imageTresholded2, imageTresholded1);

  /** Prahovanie farby tenisovej lopticky 
  cvInRangeS(imageHSV, cvScalar(0.11*256, 0.60*256, 0.20*256, 0),
	                     cvScalar(0.14*256, 1.00*256, 1.00*256, 0), mask);
  */
  cvReleaseImage(&imageHSV);

  /* Perform morphological ops */
	IplConvKernel *se21 = cvCreateStructuringElementEx(21, 21, 10, 10, CV_SHAPE_RECT, NULL);
	IplConvKernel *se11 = cvCreateStructuringElementEx(11, 11, 5,  5,  CV_SHAPE_RECT, NULL);
	cvClose(imageTresholded1, imageTresholded1, se21); // See completed example for cvClose definition
	cvOpen(imageTresholded1, imageTresholded1, se11);  // See completed example for cvOpen  definition
	cvReleaseStructuringElement(&se21);
	cvReleaseStructuringElement(&se11);

  /* Copy mask into a grayscale image */
  /*
	IplImage *hough_in = cvCreateImage(size, 8, 1);
	cvCopy(mask, hough_in, NULL);
  cvSmooth(hough_in, hough_in, CV_GAUSSIAN, 15, 15, 0, 0);
  */
  //cvSmooth(imageTresholded1, imageTresholded1, CV_GAUSSIAN, 9, 9, 0, 0);
  cvCanny(imageTresholded1, imageTresholded1, (float)1, (float)1*3, 3);
  

  //cvSmooth(imageTresholded1, imageTresholded1, CV_GAUSSIAN, 15, 15, 0, 0);

	/* Run the Hough function */
	CvMemStorage *storage = cvCreateMemStorage(0);
  CvSeq *circles = cvHoughCircles(imageTresholded1, storage, CV_HOUGH_GRADIENT, 1, imageTresholded1->height/10, 100, 40, 0, 0);
	cvReleaseMemStorage(&storage);

  cvShowImage("Video ball tresholded #n", imageTresholded1);
  //std::cout << circles->total << std::endl;

	for (int i = 0; i < circles->total; i++) 
  {
    std::cout << "Circle detected" << std::endl;
    float *p = (float*)cvGetSeqElem(circles, i);
    CvPoint center = cvPoint(cvRound(p[0]),cvRound(p[1]));
    CvScalar val = cvGet2D(mask, center.y, center.x);
    if (val.val[0] < 1) continue;
    //cvCircle(imageCircle,      center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
    //cvCircle(imageCircle,      center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
    cvCircle(imageTresholded1, center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
    cvCircle(imageTresholded1, center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
	}

    

  //cvSetMouseCallback("Video ball tresholded #1", onMouseClick, imageMain);
  //cvShowImage("Video circle test", imageCircle);
  //cvShowImage("Video circle tresholded #1", imageTresholded1);
  //cvShowImage("Video ball tresholded #2", imageBallTresholded2);
  
}