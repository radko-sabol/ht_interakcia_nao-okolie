#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <conio.h>
#include <Windows.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QListWidgetItem>
#include <QModelIndexList>
#include <QFileDialog>

#include <alproxies/altexttospeechproxy.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alproxies/almotionproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "label.h"
#include "object_detection.hpp"
<<<<<<< HEAD
#include "behaviorthread.h"
=======
#include "motion.hpp"
>>>>>>> 9949da25f1d91a08e386fd1fa7c33e2a321fb44a

namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
    
public:
  explicit MainWindow(QWidget *parent = 0);
  void getIpAndPort(QString& IP, QString& port);
  ~MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event);
  void mousePressEvent(QMouseEvent *event);

private slots:
  void imageProcessing();
  void templateProcessing();
  void motionProcessing();

  void getItem(int row);
  void getChoosenObjectIndex(int row);

  void behaviorProcessing(QListWidgetItem *item);

  void on_chooseDetectionComboBox_activated(int index);

private:
  void getImage();
  void showImage();

  void findObjectsInImage();
  void markObjectsInImage();

  void drawTemplate();

  void getXML();
  void showTemplates();

  Ui::MainWindow *ui;
  std::string robotIP;
  int robotPort;

  std::string clientName;
  IplImage* imageMain;
  QTimer *m_timer;
  QString fileName;
  QFile RNNSFile;
  std::vector<QString> XMLName;
  std::vector<QString> XMLPath;

  CvHaarClassifierCascade* cascade;
  cv::Mat imageMat;
  CvSeq* object;
  CvMemStorage* storage;

  int totalObjects;
  int choosenObjectIndex;

  AL::ALTextToSpeechProxy *textToSpeech;
  AL::ALVideoDeviceProxy *camProxy;
  AL::ALValue captureRobotCam;
  AL::ALMotionProxy *motionProxy;
  AL::ALBehaviorManagerProxy *behaviorProxy;

  cv::VideoCapture cap;

  std::vector<std::string> behaviourNames;

  float speed, theta;

  bool m_haarCascadeLoaded;
  bool m_brokerConnection;
  short int m_selectedDetection;

  enum detections
  {
    none,
    haarDetection,
    circleDetection,
    squareDetection
  };

  ObjectDetection objectDetection;
<<<<<<< HEAD
  BehaviorThread behaviorThread;
=======
  Motion motion;
>>>>>>> 9949da25f1d91a08e386fd1fa7c33e2a321fb44a
};

#endif // MAINWINDOW_H
