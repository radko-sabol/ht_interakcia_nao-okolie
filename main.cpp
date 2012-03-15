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

#include <alproxies/almotionproxy.h>
#include <alproxies/albehaviormanagerproxy.h>

#include <alerror/alerror.h>


// OpenCV hlavickove subory (v Aldebaran Nao SDK)
#include <opencv/cv.h>
#include <opencv/highgui.h>

// Ostatne hlavickove subory
#include "object_detection.hpp"
#include "motion.hpp"

// Definica flagov
bool USE_HAAR_CASCADE_DETECTION(true);
bool USE_CIRCLE_DETECTION(true);
bool USE_SQUARE_DETECTION(true);

bool DEBUG_MODE(false);

std::string haarCascadeFileName = "..\\..\\..\\files\\haar_cascades\\ball.xml";



void debugModeToggle()
{
  DEBUG_MODE = ! DEBUG_MODE;
}

int objectDetection(const std::string& robotIP, const int& robotPORT)
{
  const int imgWidth = 320,
            imgHeight = 240,
            imgDepth = 8,
            imgChannels = 3;
  ObjectDetection objectDetection;
  Motion motion;
  
  /** Ak chces pouzit obraz z kamery robota */
  AL::ALVideoDeviceProxy camProxy(robotIP, robotPORT); // Vytvorenie proxy na ALVideoDevice robota
  camProxy.setParam(AL::kCameraSelectID, 1); // prepnutie aktivnej kamery; 0 = horna, 1 = dolna
  const std::string clientName = camProxy.subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30); // nastavenie parametrov odoberaneho obrazu
  AL::ALValue captureRobotCam = 0; // inicializacia premennej, v ktorej sa uklada stream z robota

  AL::ALMotionProxy mp(robotIP, robotPORT);
  mp.setStiffnesses("Body",1);
  AL::ALBehaviorManagerProxy behavior(robotIP, robotPORT);
  behavior.runBehavior("standUp");
  behavior.runBehavior("Init");
  

  /** Ak chces pouzit obraz z kamery v PC *
  CvCapture* captureCam = 0; // inicializacia premennej, v ktorej sa uklada stream z kamery (PC,...)
  captureCam = cvCaptureFromCAM(0);
  if (! captureCam) { std::cerr << "Chyba pri inicializacii kamery..." << std::endl; return -1; }
  */

  
  /** Snaha o preberanie obrazu cez C++ (nedoriesena konverzia z cv::Mat do IplImage, tak to zatial bezi cez C-cko) */
  /*
  cv::VideoCapture captureCam = cv::VideoCapture::VideoCapture();
  captureCam.open(0);
  cv::Mat imageMat;
  //IplImage* imageIplImage = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  */
  
  

  /** Inicializacia premennej s obrazom a vytvorenie okna s hlavnym neupravovanym obrazom */
  IplImage* imageMain = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  cvNamedWindow("Video raw", CV_WINDOW_AUTOSIZE);
  

  
  /** Inicializacia premennej s obrazom a vytvorenie okna s Haar cascade detekciou objektov */
  //IplImage* imageHaar = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);

  /** Haar cascade klasifikator */
  //CvHaarClassifierCascade* cascade = loadObjectDetector("..\\..\\..\\files\\haar_cascades\\ball.xml");
  //if (! cascade) { std::cout << "Chyba pri nacitavani kaskady" << std::endl; return -1; }
  cv::CascadeClassifier haarCascade;
  if (! haarCascade.load(haarCascadeFileName)) { std::cerr << "Chyba pri nacitani haar kaskady" << std::endl; return -1; }


  /** Detekcia obdlznikovych predmetov */
  //std::vector<std::vector<cv::Point>> squares;
  //CvMat *imageMat = cvCreateMat(imgHeight, imgWidth, CV_8UC3);


  /** Cyklus bezi pokial pouzivatel nestlaci klavesu Esc */
  while ((char) cvWaitKey(30) != 27)
  {
    objectDetection.objects.clear();
    objectDetection.objects.resize(3);

    /** C++ (snaha o konverziu z cv::Mat do IplImage) */
    //captureCam >> imageMat; // prijatie frame-u z kamery

    captureRobotCam = camProxy.getImageRemote(clientName); cvSetData(imageMain, (char*)captureRobotCam[6].GetBinary(), 960); // prijatie obrazku z kamery robota a nasledne zapisanie do OpenCV datoveho typu IplImage
    //imageMain = cvQueryFrame(captureCam); // prijatie obrazku z kamery (PC,...)
    //cvShowImage("Video main", imageMain); // vykreslenie neupravovaneho vystupu z kamery
    cv::Mat imageMat(imageMain);
    cv::imshow("Video raw", imageMat);
    
    
    if (USE_HAAR_CASCADE_DETECTION)
    {
      //haarDetectAndDrawObjectsC(imageMain, cascade, 0, objects);
      objectDetection.haarDetectObjects(imageMat.clone(), haarCascade, objectDetection.objects);
    }
    
    if (USE_CIRCLE_DETECTION)
    {
      objectDetection.circleDetectObjects(imageMat.clone(), objectDetection.objects);
      //circleDetectAndDrawObjectsC(imageMain);
      //circleDetectAndDrawObjects(imageMat.clone(), objects);
    }

    if (USE_SQUARE_DETECTION)
    {
      objectDetection.squareDetectObjects(imageMat.clone(), objectDetection.objects);
      //squareDrawObjects(imageMat, squares);
    }
    
    // Vykreslenie obrazu s detegovanymi objektmi
    objectDetection.drawDetectedObjects(imageMat, objectDetection.objects);
  }

  /** Cleanup */
  behavior.runBehavior("sitDown");
  mp.setStiffnesses("Body", 0);
  camProxy.unsubscribe(clientName);
  

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
