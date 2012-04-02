#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    captureRobotCam = 0;
    m_timer = new QTimer();
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->listWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);

    templateStatus = false;
    brokerStatus = false;
    choosenObjectIndex = -1;

    speed  = theta = 0;
}

MainWindow::~MainWindow()
{
    delete ui;

    behaviorProxy->runBehavior("sitDown");

    if(brokerStatus == true)
    {
        delete textToSpeech;
        camProxy->unsubscribe(clientName);
        delete camProxy;
        motionProxy->setStiffnesses("Body",0);
        delete motionProxy;
        delete m_timer;
        delete behaviorProxy;
    }
}

void MainWindow::getIpAndPort(QString& IP, QString& port)
{
    brokerStatus = true;
	robotIP = IP.toStdString();
	robotPort = port.toInt();

    qDebug() << "IP " << IP << "Port " << port;
    textToSpeech = new AL::ALTextToSpeechProxy(robotIP, robotPort);
    camProxy = new AL::ALVideoDeviceProxy(robotIP, robotPort);
    motionProxy = new AL::ALMotionProxy(robotIP, robotPort);
    behaviorProxy = new AL::ALBehaviorManagerProxy(robotIP, robotPort);

    ui->label->getMotionProxy(IP, port);

    motionProxy->setStiffnesses("Body",1);

    behaviorProxy->runBehavior("standUp");
    behaviorProxy->runBehavior("Init");

    camProxy->setParam(AL::kCameraSelectID, 1); // prepnutie aktivnej kamery; 0 = horna, 1 = dolna
    clientName = camProxy->subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30); // nastavenie parametrov odoberaneho obrazu

    imageMain = cvCreateImage(cvSize(320, 240), 8, 3);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(imageProcessing()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(templateProcessing()));
    connect(ui->listWidget_2,SIGNAL(currentRowChanged(int)),this,SLOT(getChoosenObjectIndex(int)));
    //connect(ui->listWidget_2,SIGNAL(itemSelectionChanged()),this,SLOT(motionProcessing()));
    m_timer->start(33);
}

void MainWindow::imageProcessing()
{
    std::vector<std::string> str;
    str.clear();
    str = behaviorProxy->getInstalledBehaviors();

    //qDebug() << "Behavior" << str.size();

    for(int i = 0; i < str.size(); i++)
        ui->listWidget_2->addItem(QString(str[i].c_str()));

    getImage();

    if(templateStatus == true)
    {
        detectTemplate();
        drawTemplate();
    }

    str.clear();
}

void MainWindow::getImage()
{
    captureRobotCam = camProxy->getImageRemote(clientName);
    cvSetData(imageMain, (char*)captureRobotCam[6].GetBinary(), 960);

    cvCvtColor(imageMain,imageMain,CV_BGR2RGB);
}

void MainWindow::detectTemplate()
{

    IplImage* small_image = imageMain;
    storage = cvCreateMemStorage(0);

    object = cvHaarDetectObjects( small_image, cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING );

    totalObjects = object->total;
}

void MainWindow::drawTemplate()
{
    int scale = 1;


    ui->listWidget_2->clear();

    for(int i = 0; i < totalObjects; i++ )
    {
       CvRect face_rect = *(CvRect*)cvGetSeqElem( object, i );
       cvRectangle(imageMain, cvPoint(face_rect.x * scale, face_rect.y * scale), cvPoint((face_rect.x + face_rect.width) * scale, (face_rect.y + face_rect.height) * scale), CV_RGB(255,0,0), 3);
       ui->listWidget_2->addItem(QString("Objekt %0 na pozicii %1 %2").arg(i + 1).arg(face_rect.x).arg(face_rect.y));
    }

    if(choosenObjectIndex >= 0 && totalObjects > 0 )
    {
        CvRect face_rect = *(CvRect*)cvGetSeqElem( object, choosenObjectIndex );

        qDebug() << choosenObjectIndex << " " <<face_rect.x << " " << face_rect.y;
        headCenter(face_rect.x, face_rect.y);
    }

    image = cv::Mat(imageMain);

    QImage img = QImage((const unsigned char*)(image.data), image.cols,image.rows, QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(img));
    ui->label->resize(ui->label->pixmap()->size());

    cvReleaseMemStorage(&storage);
}

void MainWindow::templateProcessing()
{
    getXML();
    showTemplates();
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(getItem(int)));
}

void MainWindow::getXML()
{

    ui->listWidget->clear();

    XMLName.clear();
    XMLPath.clear();

    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files (*.rnns)"));


    RNNSFile.setFileName(fileName);

    RNNSFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&RNNSFile);

    std::vector<QString> line;

    while (!in.atEnd())
    {
        line.push_back(in.readLine());
    }

    for(int i = 0; i < line.size(); i++)
    {
        if((i % 2) == 0)
        {
            XMLName.push_back(line[i]);
        }
        else
        {
            XMLPath.push_back(line[i]);
        }
    }
}

void MainWindow::showTemplates()
{
    for(int i = 0; i < XMLName.size(); i++)
    {
        ui->listWidget->addItem(XMLName[i]);
    }
}

void MainWindow::getItem(int row)
{
    std::string cesticka = XMLPath[row].toStdString();

    if(cesticka.size() > 0)
    {
        cascade = (CvHaarClassifierCascade*)cvLoad(cesticka.c_str());
        templateStatus = true;
    }
    else
        templateStatus = false;
}

void MainWindow::motionProcessing()
{
    headCenter();
}

void MainWindow::headCenter(double x, double y)
{
    double nchange_x = 0;
    double nchange_y = 0;
    double mnozina_x[3];
    double mnozina_y[3];

    if(x < 160)
         nchange_x = (160 - x) / 160;
    else nchange_x = (x - 160) / 160;

    if(y < 120)
         nchange_y = (120 - y) / 120;
    else nchange_y = (y - 120) / 120;

    funkciaPrislusnosti(mnozina_x, nchange_x);
    funkciaPrislusnosti(mnozina_y, nchange_y);

    double change_x=vyhodnoteniePravidiel(mnozina_x) * 0.1; //change in gradian
    double change_y=vyhodnoteniePravidiel(mnozina_y) * 0.1;

    double distance;

    std::vector<float> position_y = motionProxy->getAngles("HeadPitch",true);

    distance = (pow(1.0+tan(position_y[0]),-1) * 0.465);

    if(x < 160)
    {
         motionProxy->changeAngles("HeadYaw", change_x, 0.05f);
    }
    else
    {
         motionProxy->changeAngles("HeadYaw", -change_x, 0.05f);
    }
    if(y < 120)
        motionProxy->changeAngles("HeadPitch", -change_y,0.05f);
    else motionProxy->changeAngles("HeadPitch", change_y,0.05f);
}

void MainWindow::headCenter()
{
        //float a=0.5;
        //double b=0;
        //double theta=0;
        //double frequency=0.5;

        double x = 0;//face_rect.x;
        double y = 0;// face_rect.y;
        double nchange_x = 0;
        double nchange_y = 0;
        double mnozina_x[3];
        double mnozina_y[3];

        if(x < 160)
             nchange_x = (160 - x) / 160;
        else nchange_x = (x - 160) / 160;

        if(y < 120)
             nchange_y = (120 - y) / 120;
        else nchange_y = (y - 120) / 120;

        funkciaPrislusnosti(mnozina_x, nchange_x);
        funkciaPrislusnosti(mnozina_y, nchange_y);

        double change_x=vyhodnoteniePravidiel(mnozina_x) * 0.1; //change in gradian
        double change_y=vyhodnoteniePravidiel(mnozina_y) * 0.1;

        double distance;

        std::vector<float> position_y = motionProxy->getAngles("HeadPitch",true);

        distance = (pow(1.0+tan(position_y[0]),-1) * 0.465);

        if(x < 160)
        {
             motionProxy->changeAngles("HeadYaw", change_x, 0.05f);
        }
        else
        {
             motionProxy->changeAngles("HeadYaw", -change_x, 0.05f);
        }
        if(y < 120)
            motionProxy->changeAngles("HeadPitch", -change_y,0.05f);
        else motionProxy->changeAngles("HeadPitch", change_y,0.05f);
}

void MainWindow::funkciaPrislusnosti(double *hodnoty, double hodnota)
{
    funkciaBlizsko(hodnoty,hodnota);
    funkciaStredne(hodnoty,hodnota);
    funkciaDaleko(hodnoty,hodnota);
}

void MainWindow::funkciaDaleko(double *hodnoty,double hodnota)
{
    hodnoty[0]=(1-2*hodnota);
}
void MainWindow::funkciaStredne(double *hodnoty,double hodnota)
{
    if(hodnota<0.5)
        hodnoty[1]=2*hodnota;
    else
        hodnoty[1]=2-2*hodnota;
}
void MainWindow::funkciaBlizsko(double *hodnoty,double hodnota)
{
    hodnoty[2]=2*hodnota-1;
}

double MainWindow::vyhodnoteniePravidiel(const double *hodnotyx)
{
    double vystup[3]={0,0,0};

    if(hodnotyx[0]>0)
    {
        //std::cout << "odpal 1";
        vystup[0]=hodnotyx[0]*0.05f;
    }
    if(hodnotyx[1]>0)
    {
        //std::cout << "odpal 2";
        vystup[1]=hodnotyx[1]*0.45f;
    }
    if(hodnotyx[2]>0)
    {
        //std::cout << "odpal 3";
        vystup[2]=hodnotyx[2]*0.95f;
    }
return vystup[0]+vystup[1]+vystup[2];
}

void MainWindow::getChoosenObjectIndex(int row)
{
    if(row >= 0)
        choosenObjectIndex = row;
    qDebug() << choosenObjectIndex;
}



void MainWindow::keyPressEvent(QKeyEvent* event)
{
    setFocus();
    qDebug() << event->key();

    //hlava

    if(event->key() == Qt::Key_Up)
        motionProxy->post.changeAngles("HeadPitch", -0.1, 0.05f);
    if(event->key() == Qt::Key_Down)
        motionProxy->post.changeAngles("HeadPitch", 0.1, 0.05f);
    if(event->key() == Qt::Key_Left)
        motionProxy->post.changeAngles("HeadYaw", 0.1, 0.05f);
    if(event->key() == Qt::Key_Right)
        motionProxy->post.changeAngles("HeadYaw", -0.1, 0.05f);

    if(event->key() == Qt::Key_Q)
        {
            speed = 0;
            theta = 0;
        }
    if(event->key() == Qt::Key_W)
        {
            if (speed < 1)
                speed += 0.01f;
        }
    if(event->key() == Qt::Key_S)
        {
            if (speed >- 1)
                speed -= 0.01f;
        }
   if(event->key() == Qt::Key_A)
        {
            if(theta < 1)
                theta += 0.01f;
        }
   if(event->key() == Qt::Key_D)
        {
            if(theta >- 1)
                theta -= 0.01f;
        }

   motionProxy->post.setWalkTargetVelocity(speed,0,theta,0.5);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        choosenObjectIndex = -1;
}
