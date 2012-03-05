/**
* Copyright 2012
*
* Nazov projektu: Humanoidne technologie - interakcia humanoidneho robota Nao s okolim
*
* Autori projektu:
*  - Roman Michna (manazer)
*  - Peter Brudnak
*  - Jan Marcin
*  - Radko Sabol
* (Technicka Univerzita Kosice - Katedra kybernetiky a umelej inteligencie, odbor: Umela inteligencia)
*
* Popis projektu:
*  TODO
*
*/

// Nacitanie standardnych hlavickovych suborov
#include <iostream>
#include <string>
#include <algorithm>

// Nacitanie Aldebaran Nao hlavickovych suborov
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

// Definica flagov
#define useHaarCascadeDetection true
#define useCircleDetection false

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

void circleDetectAndDrawObjects(IplImage* imageMain)
{
  CvSize size = cvGetSize(imageMain);
  IplImage* imageCircle = cvCreateImage(size, imageMain->depth, imageMain->nChannels);
  cvCopy(imageMain, imageCircle, NULL);
  IplImage* imageHSV = cvCreateImage(size, imageCircle->depth, imageCircle->nChannels);
  IplImage* imageTresholded1 = cvCreateImage(size, imageCircle->depth, 1);
  IplImage* imageTresholded2 = cvCreateImage(size, imageCircle->depth, 1);

  CvScalar hsv_min = cvScalar(0, 50, 170, 0);
  CvScalar hsv_max = cvScalar(10, 180, 256, 0);
  CvScalar hsv_min2 = cvScalar(170, 50, 170, 0);
  CvScalar hsv_max2 = cvScalar(256, 180, 256, 0);

  cvCvtColor(imageCircle, imageHSV, CV_BGR2HSV);

  CvMat *mask = cvCreateMat(size.height, size.width, CV_8UC1);
  cvInRangeS(imageHSV, hsv_min, hsv_max, imageTresholded1);
  cvInRangeS(imageHSV, hsv_min2, hsv_max2, imageTresholded2);
  cvOr(imageTresholded1, imageTresholded2, imageTresholded1);
  /*
  cvInRangeS(imageHSV, cvScalar(0.11*256, 0.60*256, 0.20*256, 0),
	                     cvScalar(0.14*256, 1.00*256, 1.00*256, 0), mask);
  */
  /*
  cvInRangeS(imageHSV, cvScalar(0.11*256, 0.16*256, 0.66*256, 0),
	                         cvScalar(1.00*256, 0.70*256, 1.00*256, 0), mask);
  */
  /*
  cvInRangeS(imageHSV, cvScalar(150, 40, 170, 0),
	                         cvScalar(256, 180, 256, 0), mask);
  */
  //cvShowImage("Video ball tresholded #n", imageHSV);
  cvReleaseImage(&imageHSV);

  /*
	IplConvKernel *se21 = cvCreateStructuringElementEx(11, 11, 10, 10, CV_SHAPE_RECT, NULL);
	IplConvKernel *se11 = cvCreateStructuringElementEx(11, 11, 5,  5,  CV_SHAPE_RECT, NULL);
	cvClose(mask, mask, se21); // See completed example for cvClose definition
	cvOpen(mask, mask, se11);  // See completed example for cvOpen  definition
	cvReleaseStructuringElement(&se21);
	cvReleaseStructuringElement(&se11);
  */
	
  /* Copy mask into a grayscale image */
  /*
	IplImage *hough_in = cvCreateImage(size, 8, 1);
	cvCopy(mask, hough_in, NULL);
  cvSmooth(hough_in, hough_in, CV_GAUSSIAN, 15, 15, 0, 0);
  //cvSmooth(hough_in, hough_in, CV_GAUSSIAN, 9, 9);
  //cvCanny(hough_in, hough_in, (float)1, (float)1*3, 5);
  */

  cvSmooth(imageTresholded1, imageTresholded1, CV_GAUSSIAN, 9, 9);

	/* Run the Hough function */
	CvMemStorage *storage = cvCreateMemStorage(0);
  CvSeq *circles = cvHoughCircles(imageTresholded1, storage, CV_HOUGH_GRADIENT, 2, imageTresholded1->height/4, 100, 50, 20, 200);
	cvReleaseMemStorage(&storage);

  //std::cout << circles->total << std::endl;

	for (int i = 0; i < circles->total; i++) 
  {
    std::cout << "Circle detected" << std::endl;
    float *p = (float*)cvGetSeqElem(circles, i);
    CvPoint center = cvPoint(cvRound(p[0]),cvRound(p[1]));
    CvScalar val = cvGet2D(mask, center.y, center.x);
    if (val.val[0] < 1) continue;
    cvCircle(imageCircle,      center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
    cvCircle(imageCircle,      center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
    cvCircle(imageTresholded1, center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
    cvCircle(imageTresholded1, center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
	}

  //cvSetMouseCallback("Video ball tresholded #1", onMouseClick, imageMain);
  cvShowImage("Video circle test", imageCircle);
  cvShowImage("Video circle tresholded #1", imageTresholded1);
  //cvShowImage("Video ball tresholded #2", imageBallTresholded2);
  
}


int objectDetection(const std::string& robotIP, const int& robotPORT)
{
  const int imgWidth = 320,
            imgHeight = 240,
            imgDepth = 8,
            imgChannels = 3;
  
  /** Ak chces pouzit obraz z kamery robota
  AL::ALVideoDeviceProxy camProxy(robotIP, robotPORT); // Vytvorenie proxy na ALVideoDevice robota
  camProxy.setParam(AL::kCameraSelectID, 0); // prepnutie aktivnej kamery; 0 = horna, 1 = dolna
  const std::string clientName = camProxy.subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30); // nastavenie parametrov odoberaneho obrazu
  AL::ALValue captureRobotCam = 0; // inicializacia premennej, v ktorej sa uklada stream z robota
  */

  /** Ak chces pouzit obraz z kamery v PC */
  CvCapture* captureCam = 0; // inicializacia premennej, v ktorej sa uklada stream z kamery (PC,...)
  captureCam = cvCaptureFromCAM(0);
  if (! captureCam) { std::cout << "Chyba pri inicializacii kamery..." << std::endl; return -1; }
  

  /** Inicializacia premennej s obrazom a vytvorenie okna s hlavnym neupravovanym obrazom */
  IplImage* imageMain = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  //IplImage* imageMain = cvLoadImage("C:\\Users\\Radko Sabol\\Desktop\\tennis\\images\\1-10.jpg");
  //cvNamedWindow("Video main", CV_WINDOW_AUTOSIZE);



  if (useHaarCascadeDetection)
  {
    /** Vytvorenie okna s Haar cascade detekciou objektov */
    cvNamedWindow("Video haar", CV_WINDOW_AUTOSIZE);
  }

  /** Inicializacia premennej s obrazom a vytvorenie okna s Haar cascade detekciou objektov */
  IplImage* imageHaar = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);

  /** Haar cascade klasifikator */
  CvHaarClassifierCascade* cascade = loadObjectDetector("..\\..\\..\\files\\haar_cascades\\ball.xml");
  if (! cascade) { std::cout << "Chyba pri nacitavani kaskady" << std::endl; return -1; }



  /** Detekcia kruhovych predmetov */
  IplImage* imageHSV = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  if (useCircleDetection)
  {
    //cvNamedWindow("Video ball tresholded #1", CV_WINDOW_AUTOSIZE);
    //cvNamedWindow("Video ball tresholded #2", CV_WINDOW_AUTOSIZE);
  }



  /** Cyklus bezi pokial pouzivatel nestlaci klavesu Esc */
  while ((char) cvWaitKey(30) != 27)
  {
    //captureRobotCam = camProxy.getImageRemote(clientName); cvSetData(imageMain, (char*)captureRobotCam[6].GetBinary(), 960); // prijatie obrazku z kamery robota a nasledne zapisanie do OpenCV datoveho typu IplImage
    imageMain = cvQueryFrame(captureCam); // prijatie obrazku z kamery (PC,...)
    cvShowImage("Video main", imageMain); // vykreslenie neupravovaneho vystupu z kamery

    
    if (useHaarCascadeDetection)
    {
      //cvSetData(imageHaar, imageMain->imageData, imageMain->width*imageMain->nChannels);
      haarDetectAndDrawObjects(imageMain, cascade, 0);
      //cvShowImage("Video haar", imageHaar);
    }

    
    if (useCircleDetection)
    {
      circleDetectAndDrawObjects(imageMain);
    }
    

  }

  return 0;
}

// argumenty: [1] - IP adresa robota, [2] - port robota
int main(int argc, const char *argv[])
{
  std::string robotIP = "";
  int robotPORT = 0;
  
  /** Spracovanie zadanych argumentov */
  if (argc < 2)
  {
    robotIP = "";
    robotPORT = 0;
  }
  else if (argc < 3)
  {
    robotIP = argv[1];
    robotPORT = 9559;
  }
  else
  {
    robotIP = argv[1];
    robotPORT = atoi(argv[2]);
  }

  try
  {
    return objectDetection(robotIP, robotPORT);
  }
  catch (const AL::ALError& error)
  {
    std::cerr << "Chyba: " << error.what() << std::endl;
  }

  return 0;
}
