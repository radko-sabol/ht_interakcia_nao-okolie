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
  
  /** Ak chces pouzit obraz z kamery robota *
  AL::ALVideoDeviceProxy camProxy(robotIP, robotPORT); // Vytvorenie proxy na ALVideoDevice robota
  camProxy.setParam(AL::kCameraSelectID, 1); // prepnutie aktivnej kamery; 0 = horna, 1 = dolna
  const std::string clientName = camProxy.subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30); // nastavenie parametrov odoberaneho obrazu
  AL::ALValue captureRobotCam = 0; // inicializacia premennej, v ktorej sa uklada stream z robota

  AL::ALMotionProxy mp(robotIP, robotPORT);
  mp.setStiffnesses("Body",1);
  AL::ALBehaviorManagerProxy behavior(robotIP, robotPORT);
  behavior.runBehavior("standUp");
  behavior.runBehavior("Init");
  */

  /** Ak chces pouzit obraz z kamery v PC */
  CvCapture* captureCam = 0; // inicializacia premennej, v ktorej sa uklada stream z kamery (PC,...)
  captureCam = cvCaptureFromCAM(0);
  if (! captureCam) { std::cerr << "Chyba pri inicializacii kamery..." << std::endl; return -1; }
  

  
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

    //captureRobotCam = camProxy.getImageRemote(clientName); cvSetData(imageMain, (char*)captureRobotCam[6].GetBinary(), 960); // prijatie obrazku z kamery robota a nasledne zapisanie do OpenCV datoveho typu IplImage
    imageMain = cvQueryFrame(captureCam); // prijatie obrazku z kamery (PC,...)
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
    
    for (size_t i = 0; i < objectDetection.objects.size(); i++) // ID identifikatora
    {
      for (size_t j = 0; j < objectDetection.objects[i].size(); j++) // najdene objekty
      {
        if (i == 0) // detegovany haar objekt
        {
          cv::rectangle(imageMat, cv::Point(objectDetection.objects[i][j][0][0], objectDetection.objects[i][j][0][1]), cv::Point(objectDetection.objects[i][j][0][0] + objectDetection.objects[i][j][0][2], objectDetection.objects[i][j][0][1] + objectDetection.objects[i][j][0][3]), cv::Scalar(0, 255, 0), 3, 8, 0);
          cv::Point center(objectDetection.objects[i][j][0][0] + objectDetection.objects[i][j][0][2]*0.5, objectDetection.objects[i][j][0][1] + objectDetection.objects[i][j][0][3]*0.5);
          cv::circle(imageMat, center, 1, cv::Scalar(255, 0, 0), 3, 8, 0);
          std::cout << "X: " << center.x << "Y: " << center.y << std::endl;
          
          /*
	        if ((center.x > 0) && (center.y > 0))
          {
            motion.center(center.x, center.y, mp);
          }
          */
          
        }

        if (i == 1) // detegovany kruh
        {
          cv::Point circleCenter(cvRound(objectDetection.objects[i][j][0][0]), cvRound(objectDetection.objects[i][j][0][1]));
          int circleRadius = cvRound(objectDetection.objects[i][j][0][2]);

          cv::circle(imageMat, circleCenter, circleRadius, cv::Scalar(0, 0, 255), 3, 8, 0);
        }

        if (i == 2) // detegovany stvorec
        {
          const cv::Scalar p1 = objectDetection.objects[i][j][0];
          const cv::Scalar p2 = objectDetection.objects[i][j][1];
          const cv::Scalar p3 = objectDetection.objects[i][j][2];
          const cv::Scalar p4 = objectDetection.objects[i][j][3];

          /*
          std::cout << "P1 - X: " << p1[0] << " ;Y: " << p1[1] << " ;?: " << p1[2] << " ;?: " << p1[3] << std::endl;
          std::cout << "P2 - X: " << p2[0] << " ;Y: " << p2[1] << " ;?: " << p2[2] << " ;?: " << p2[3] << std::endl;
          std::cout << "P3 - X: " << p3[0] << " ;Y: " << p3[1] << " ;?: " << p3[2] << " ;?: " << p3[3] << std::endl;
          std::cout << "P4 - X: " << p4[0] << " ;Y: " << p4[1] << " ;?: " << p4[2] << " ;?: " << p4[3] << std::endl;
          */

          /*
          // Vypocet stredoveho bodu
          int x = 0,
              y = 0;

          x = ((squares[i][0].x + squares[i][2].x)/2 + (squares[i][1].x + squares[i][3].x)/2) / 2;
          y = ((squares[i][0].y + squares[i][2].y)/2 + (squares[i][1].y + squares[i][3].y)/2) / 2;

          cv::circle(image, cv::Point(squares[i][0].x, squares[i][0].y), 2, cv::Scalar(0, 0, 255), 1);
          //cv::circle(image, cv::Point(squares[i][1].x, squares[i][1].y), 2, cv::Scalar(255, 0, 0), 1);
          cv::circle(image, cv::Point(squares[i][2].x, squares[i][2].y), 2, cv::Scalar(255, 0, 255), 1);
          cv::circle(image, cv::Point(x, y), 2, cv::Scalar(255, 0, 0), 1);
          */

          cv::line(imageMat, cv::Point(p1[0], p1[1]), cv::Point(p2[0], p2[1]), cv::Scalar(255, 0, 0), 2);
          cv::line(imageMat, cv::Point(p2[0], p2[1]), cv::Point(p3[0], p3[1]), cv::Scalar(255, 0, 0), 2);
          cv::line(imageMat, cv::Point(p3[0], p3[1]), cv::Point(p4[0], p4[1]), cv::Scalar(255, 0, 0), 2);
          cv::line(imageMat, cv::Point(p4[0], p4[1]), cv::Point(p1[0], p1[1]), cv::Scalar(255, 0, 0), 2);
        }
      }
    }

    cv::imshow("Video detected", imageMat);
  }

  /** Cleanup *
  behavior.runBehavior("sitDown");
  mp.setStiffnesses("Body", 0);
  camProxy.unsubscribe(clientName);
  */
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
