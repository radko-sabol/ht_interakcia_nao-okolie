#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <conio.h>
#include <Windows.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

#include <QMainWindow>
#include <QMessageBox>
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
#include "behaviorthread.h"
#include "motion.hpp"

namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
    
public:
  explicit MainWindow(QWidget *parent = 0);
  int getIpAndPort(QString& IP, QString& port);
  ~MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event);

private slots:
  void imageProcessing();
  void templateProcessing();

  void getItem(int row);

  void behaviorProcessing(QListWidgetItem *item);
  void behaviorProcessing(std::string selectedMotion);

  void on_chooseDetectionComboBox_activated(int index);

  void on_stiffnessToggleButton_clicked();

  void setStiffnessToggleButtonText();

private:
  void getImage();
  void showImage();

  void findObjectsInImage();
  void markObjectsInImage();

  void drawTemplate();

  void getConfig();
  void showTemplates();

  Ui::MainWindow *ui;
  std::string robotIP;
  int robotPort;

  std::string clientName;
  IplImage* imageMain;
  QTimer *m_timer;
  QString m_fileName;
  QFile RNNSFile;
  std::vector<QString> XMLName;
  std::vector<QString> XMLPath;

  CvHaarClassifierCascade* cascade;
  cv::Mat imageMat;
  CvSeq* object;
  CvMemStorage* storage;

  int totalObjects;

  AL::ALTextToSpeechProxy *textToSpeech;
  AL::ALVideoDeviceProxy *camProxy;
  AL::ALValue captureRobotCam;
  AL::ALMotionProxy *motionProxy;
  AL::ALBehaviorManagerProxy *behaviorProxy;

  cv::VideoCapture cap;

  std::vector<std::string> behaviourNames;

  float speed, theta;

  bool m_haarCascadeLoaded;
  bool m_isBrokerConnection;
  short int m_selectedDetection;

  enum detections
  {
    none,
    haarDetection,
    circleDetection,
    squareDetection
  };

  ObjectDetection objectDetection;
  BehaviorThread behaviorThread;
  Motion motion;

  signals:
  void getSpeed(QString);
  void getAngle(QString);
};

#endif // MAINWINDOW_H
