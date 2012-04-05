#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void getSelectedDetector(int index);

private:
     void getImage();
     void drawTemplate();

     void getXML();
     void showTemplates();

     void headCenter();
     void headCenter(double x, double y);

     void funkciaPrislusnosti(double *hodnoty, double hodnota);
     void funkciaBlizsko(double *hodnoty, double hodnota);
     void funkciaStredne(double *hodnoty, double hodnota);
     void funkciaDaleko(double *hodnoty, double hodnota);
     double vyhodnoteniePravidiel(const double *hodnotyx);

private:
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
    cv::Mat image;
    CvSeq* object;
    CvMemStorage* storage;

    int totalObjects;
    int choosenObjectIndex;

    AL::ALTextToSpeechProxy *textToSpeech;
    AL::ALVideoDeviceProxy *camProxy;
    AL::ALValue captureRobotCam;
    AL::ALMotionProxy *motionProxy;
    AL::ALBehaviorManagerProxy *behaviorProxy;

    std::vector<std::string> behaviourNames;

    float speed, theta;

    bool templateStatus;
    bool brokerStatus;

    ObjectDetection objectDetection;
};

#endif // MAINWINDOW_H
