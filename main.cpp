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

// Standardne hlavickove subory
#include <iostream>
#include <string>
#include <algorithm>

// Aldebaran Nao hlavickove subory
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>

// OpenCV hlavickove subory (v Aldebaran Nao SDK)
#include <opencv/cv.h>
#include <opencv/highgui.h>

// Ostatne hlavickove subory
#include "detect_circle.hpp"
#include "detect_square.hpp"
#include "detect_haar_cascade_classifier.hpp"

// Definica flagov
const bool USE_HAAR_CASCADE_DETECTION(false);
const bool USE_CIRCLE_DETECTION(true);
const bool USE_SQUARE_DETECTION(false);



int objectDetection(const std::string& robotIP, const int& robotPORT)
{
  const int imgWidth = 320,
            imgHeight = 240,
            imgDepth = 8,
            imgChannels = 3;
  
  /** Ak chces pouzit obraz z kamery robota *
  AL::ALVideoDeviceProxy camProxy(robotIP, robotPORT); // Vytvorenie proxy na ALVideoDevice robota
  camProxy.setParam(AL::kCameraSelectID, 0); // prepnutie aktivnej kamery; 0 = horna, 1 = dolna
  const std::string clientName = camProxy.subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30); // nastavenie parametrov odoberaneho obrazu
  AL::ALValue captureRobotCam = 0; // inicializacia premennej, v ktorej sa uklada stream z robota
  */

  /** Ak chces pouzit obraz z kamery v PC */
  CvCapture* captureCam = 0; // inicializacia premennej, v ktorej sa uklada stream z kamery (PC,...)
  captureCam = cvCaptureFromCAM(0);
  if (! captureCam) { std::cout << "Chyba pri inicializacii kamery..." << std::endl; return -1; }

  /** Snaha o preberanie obrazu cez C++ (nedoriesena konverzia z cv::Mat do IplImage, tak to zatial bezi cez C-cko)
  cv::VideoCapture captureCam2 = cv::VideoCapture::VideoCapture();
  captureCam2.open(0);
  cv::Mat imageMat;
  IplImage* imageIplImage = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  */
  

  /** Inicializacia premennej s obrazom a vytvorenie okna s hlavnym neupravovanym obrazom */
  IplImage* imageMain = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  cvNamedWindow("Video main", CV_WINDOW_AUTOSIZE);
  

  
  if (USE_HAAR_CASCADE_DETECTION)
  {
    /** Vytvorenie okna s Haar cascade detekciou objektov */
    cvNamedWindow("Video haar", CV_WINDOW_AUTOSIZE);
  }

  /** Inicializacia premennej s obrazom a vytvorenie okna s Haar cascade detekciou objektov */
  //IplImage* imageHaar = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);

  /** Haar cascade klasifikator */
  CvHaarClassifierCascade* cascade = loadObjectDetector("..\\..\\..\\files\\haar_cascades\\ball.xml");
  if (! cascade) { std::cout << "Chyba pri nacitavani kaskady" << std::endl; return -1; }



  /** Detekcia kruhovych predmetov */
  //IplImage* imageHSV = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  if (USE_CIRCLE_DETECTION)
  {
    //cvNamedWindow("Video ball tresholded #1", CV_WINDOW_AUTOSIZE);
    //cvNamedWindow("Video ball tresholded #2", CV_WINDOW_AUTOSIZE);
  }


  /** Detekcia obdlznikovych predmetov */
  std::vector<std::vector<cv::Point>> squares;
  //CvMat *imageMat = cvCreateMat(imgHeight, imgWidth, CV_8UC3);


  /** Cyklus bezi pokial pouzivatel nestlaci klavesu Esc */
  while ((char) cvWaitKey(30) != 27)
  {
    /** Snaha o konverziu z cv::Mat do IplImage
    captureCam2 >> frame; // prijatie frame-u z kamery
    IplImage src = frame;
    cvCopy(&src, frame);
    cv::imshow("test", frame);
    cvShowImage("asd", src);
    */

    //captureRobotCam = camProxy.getImageRemote(clientName); cvSetData(imageMain, (char*)captureRobotCam[6].GetBinary(), 960); // prijatie obrazku z kamery robota a nasledne zapisanie do OpenCV datoveho typu IplImage
    imageMain = cvQueryFrame(captureCam); // prijatie obrazku z kamery (PC,...)
    cvShowImage("Video main", imageMain); // vykreslenie neupravovaneho vystupu z kamery
    
    cv::Mat imageMat(imageMain);
    
    if (USE_HAAR_CASCADE_DETECTION)
    {
      haarDetectAndDrawObjects(imageMain, cascade, 0);
    }
    
    if (USE_CIRCLE_DETECTION)
    {
      //circleDetectAndDrawObjectsC(imageMain);
      circleDetectAndDrawObjects(imageMat.clone());
    }

    if (USE_SQUARE_DETECTION)
    {
      squareDetectObjects(imageMat.clone(), squares);
      squareDrawObjects(imageMat, squares);
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
