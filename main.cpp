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

// Nacitanie Aldebaran Nao hlavickovych suborov
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>


int objectDetection(const std::string& robotIP, const int& robotPORT)
{
  const int imgWidth = 320,
            imgHeight = 240,
            imgDepth = 8,
            imgChannels = 3;

  /** Vytvorenie proxy na ALVideoDevice robota */
  //AL::ALVideoDeviceProxy camProxy(robotIP, robotPORT);
  //camProxy.setParam(AL::kCameraSelectID, 0); // prepnutie aktivnej kamery; 0 = horna, 1 = dolna
  //const std::string clientName = camProxy.subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30);
  //AL::ALValue captureRobotCam = 0;

  CvCapture* captureCam = 0;
  captureCam = cvCaptureFromCAM(0);
  if (! captureCam) { std::cout << "Chyba pri inicializacii kamery..." << std::endl; return -1; }


  IplImage* imageMain = cvCreateImage(cvSize(imgWidth, imgHeight), imgDepth, imgChannels);
  cvNamedWindow("Video main");
  

  /** Main loop. Exit when pressing ESC.*/
  while ((char) cvWaitKey(30) != 27)
  {
    //captureRobotCam = camProxy.getImageRemote(clientName); cvSetData(imageMain, (char*)captureRobotCam[6].GetBinary(), 960);
    imageMain = cvQueryFrame(captureCam);

    cvShowImage("Video main", imageMain);
  }

  return 0;
}

// argumenty: [1] - IP adresa robota, [2] - port robota
int main(int argc, const char *argv[])
{
  std::string robotIP = "";
  int robotPORT = 0;
  
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
