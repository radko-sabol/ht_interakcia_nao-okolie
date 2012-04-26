#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * Konstruktor
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->haarCascadesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->haarCascadesListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  ui->foundObjectListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->foundObjectListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->foundObjectListWidget->setSortingEnabled(true);

  ui->chooseDetectionComboBox->insertItem(none, QString("..."));
  ui->chooseDetectionComboBox->insertItem(haarDetection, QString("Haar"));
  ui->chooseDetectionComboBox->insertItem(circleDetection, QString("Circle"));
  ui->chooseDetectionComboBox->insertItem(squareDetection, QString("Square"));

  m_timer = new QTimer();
  m_haarCascadeLoaded = false;
  m_brokerConnection = false;
  m_selectedDetection = none;
  choosenObjectIndex = -1;
  captureRobotCam = 0;
  speed = theta = 0;
}

/**
 * Destruktor
 */
MainWindow::~MainWindow()
{
  delete ui;
  delete m_timer;

  if (m_brokerConnection) // kontrola, ci som bol pripojeny na robota
  {
    delete textToSpeech;
    camProxy->unsubscribe(clientName);
    delete camProxy;
    if (behaviorProxy->isBehaviorInstalled("sitDown"))
    {
      behaviorProxy->runBehavior("sitDown"); // predtym to bolo mimo podmienky a sposobovalo to "Assertion failed" pri zatvoreni okna s nastavenim IP adresy a portu
      motionProxy->setStiffnesses("Body", 0);
    }
    else
    {
      QMessageBox::warning(this, "Missing behavior", "The behavior \"sitDown\" is missing.\nStiffness will be turned off - hold the robot.", QMessageBox::Ok, QMessageBox::Ok);
      motionProxy->setStiffnesses("Body", 0);
    }
    delete behaviorProxy;
    delete motionProxy;
  }
}

/**
 * Pripojenie na hardware robota, ktoreho NaoQi bezi na danej IP adrese a porte
 *
 * @param QString &IP - IP adresa robota
 * @param QString &port - port
 */
void MainWindow::getIpAndPort(QString &IP, QString &port)
{
  robotIP = IP.toStdString();
  robotPort = port.toInt();

  if (robotIP == "127.0.0.1") // pripajam sa na lokalnu kameru v PC
  {
    m_brokerConnection = false; // nastavenie flagu informujuceho, ze sa nepripajam na robota
    cap.open(0);
    if (! cap.isOpened()) { /*return -1;*/ }
  }
  else
  {
    m_brokerConnection = true; // nastavenie flagu informujuceho, ze sa pripajam na robota

    qDebug() << "IP: " << IP << ", port: " << port;
    textToSpeech = new AL::ALTextToSpeechProxy(robotIP, robotPort);
    camProxy = new AL::ALVideoDeviceProxy(robotIP, robotPort);
    motionProxy = new AL::ALMotionProxy(robotIP, robotPort);
    behaviorProxy = new AL::ALBehaviorManagerProxy(robotIP, robotPort);

    ui->videoStreamLabel->getMotionProxy(IP, port);

    motionProxy->setStiffnesses("Body", 1);

    behaviorProcessing("Init");

    camProxy->setParam(AL::kCameraSelectID, 1); // 0 - horna kamera; 1 - dolna kamera
    clientName = camProxy->subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30);

    behaviourNames = behaviorProxy->getInstalledBehaviors();

    for (unsigned int i = 0; i < behaviourNames.size(); i++) { ui->behaviorsListWidget->addItem(QString(behaviourNames[i].c_str())); }

    imageMain = cvCreateImage(cvSize(320, 240), 8, 3);
  }

  connect(m_timer, SIGNAL(timeout()), this, SLOT(imageProcessing()));
  connect(ui->loadHaarCascadeButton, SIGNAL(clicked()), this, SLOT(templateProcessing()));
  connect(ui->foundObjectListWidget,SIGNAL(currentRowChanged(int)),this,SLOT(getChoosenObjectIndex(int)));
  //connect(ui->listWidget_2,SIGNAL(itemSelectionChanged()),this,SLOT(motionProcessing()));
  connect(ui->behaviorsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(behaviorProcessing(QListWidgetItem*)));

  m_timer->start(33);
}

/**
 * Funkcia obsluhujuca ziskavanie obrazu, detegovanie a vykreslovanie objektov a vykreslovanie obrazu do GUI
 */
void MainWindow::imageProcessing()
{
  QString foundObjects;
  cv::Point center;
  ui->foundObjectListWidget->clear();

  getImage(); // ziskanie obrazu z kamery do premennej imageMat

  findObjectsInImage(); // vykonanie pozadovaneho sposobu detekcie objektov v obraze imageMat

  std::vector< std::vector<cv::Scalar> > detectedObjects = objectDetection.getObjects(m_selectedDetection);

  for (unsigned int i = 0; i < detectedObjects.size(); i++)
  {
    if (m_selectedDetection == haarDetection)        { center = cv::Point(detectedObjects[i][0][0] + detectedObjects[i][0][2]*0.5, detectedObjects[i][0][1] + detectedObjects[i][0][3]*0.5); }
    else if (m_selectedDetection == circleDetection) { center = cv::Point(detectedObjects[i][0][0], detectedObjects[i][0][1]); }
    else if (m_selectedDetection == squareDetection) { center = cv::Point((((detectedObjects[i][0][0] + detectedObjects[i][2][0])/2 + (detectedObjects[i][1][0] + detectedObjects[i][3][0])/2) / 2), (((detectedObjects[i][0][1] + detectedObjects[i][2][1])/2 + (detectedObjects[i][1][1] + detectedObjects[i][3][1])/2) / 2)); }
    foundObjects = QString("[%1] X: %2; Y: %3").arg(i+1).arg(center.x).arg(center.y);
    ui->foundObjectListWidget->addItem(foundObjects);
  }

  // vycentrovanie hlavy na stred objektu ak robot vidi len jeden objekt (zatial len testovanie na haar detekcii)
  if (detectedObjects.size() == 1)
  {
    if (m_selectedDetection == haarDetection) // haar
    {
      int x = (detectedObjects[0][0][0] + (detectedObjects[0][0][2] * 0.5)),
          y = (detectedObjects[0][0][1] + (detectedObjects[0][0][3] * 0.5));
      motion.headCenter(x, y, *motionProxy);
      motion.walkToObject(*motionProxy);
      //detectedObjects[0][0][0] // x
      //detectedObjects[0][0][1] // y
      //detectedObjects[0][0][2] // width
      //detectedObjects[0][0][3] // height
    }
    else if (m_selectedDetection == circleDetection) // circle
    {
      int x = (detectedObjects[0][0][0]),
          y = (detectedObjects[0][0][1]);
      motion.headCenter(x, y, *motionProxy);
      motion.walkToObject(*motionProxy);
    }
  }
  else if (detectedObjects.size() == 0)
  {
    motion.stopWalking(*motionProxy);
  }

  //std::cout << "[" << m_selectedDetection << "] najdenych objektov: " << (detectedObjects.size()) << std::endl;
  markObjectsInImage(); // zakreslenie najdenych objektov v obraze imageMat

  showImage(); // vykreslenie obrazu do GUI
}

/**
 * Ziskanie frame-u z kamery robota/PC a zapisanie ho do premennej imageMat
 */
void MainWindow::getImage()
{
  if (m_brokerConnection) // kontrola, ci som pripojeny na robota
  {
    captureRobotCam = camProxy->getImageRemote(clientName);
    cvSetData(imageMain, (char*)captureRobotCam[6].GetBinary(), 960);

    imageMat = cv::Mat(imageMain);
  }
  else
  {
    cv::Mat image; // pomocna premenna z dovodu, ze resize obrazu jednej premennej (ako vstupnej aj vystupnej) nefunguje korektne
    cap >> image;
    cv::resize(image, imageMat, cv::Size(320, 240), 0, 0, CV_INTER_LINEAR); // zmen velkosti obrazu na 320x240
  }
}

/**
 * Vykreslenie obrazu do gui (z premennej imageMat)
 */
void MainWindow::showImage()
{
  cv::cvtColor(imageMat, imageMat, CV_BGR2RGB); // zmena z BGR na RGB farebny priestor
  QImage img = QImage((const unsigned char*)(imageMat.data), imageMat.cols, imageMat.rows, QImage::Format_RGB888);
  ui->videoStreamLabel->setPixmap(QPixmap::fromImage(img));
  ui->videoStreamLabel->resize(ui->videoStreamLabel->pixmap()->size());
}

/**
 * Vykonanie pozadovanej detekcie objektov v obraze imageMat
 */
void MainWindow::findObjectsInImage()
{
  objectDetection.clearObjects(); // vyprazdnenie premennej, do ktorej sa zapisuju pozicie najdenych objektov

  if ((m_selectedDetection == haarDetection) && (m_haarCascadeLoaded == true))
  {
    objectDetection.haarDetectObjects(imageMat.clone());
  }

  if (m_selectedDetection == circleDetection)
  {
    objectDetection.circleDetectObjects(imageMat.clone());
  }

  if (m_selectedDetection == squareDetection)
  {
    objectDetection.squareDetectObjects(imageMat.clone());
  }
}

/**
 * Zaznacenie najdenych objektov do premennej imageMat
 */
void MainWindow::markObjectsInImage()
{
  objectDetection.drawDetectedObjects(imageMat);
}

/*
void MainWindow::drawTemplate()
{
  ui->listWidget_2->clear();

  objectDetection.clearObjects();
  objectDetection.haarDetectObjects(imageMat);
  objectDetection.drawDetectedObjects(imageMat);

  QImage img = QImage((const unsigned char*)(imageMat.data), imageMat.cols,imageMat.rows, QImage::Format_RGB888);
  ui->label->setPixmap(QPixmap::fromImage(img));
  ui->label->resize(ui->label->pixmap()->size());
}
*/

/**
 *
 */
void MainWindow::templateProcessing() // TODO: mne osobne tento nazov funkcie na prvy pohlad nehovori nic
{
  getXML();
  showTemplates();
  connect(ui->haarCascadesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(getItem(int)));
}

/**
 *
 */
void MainWindow::getXML()
{
  ui->haarCascadesListWidget->clear();

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

  for(unsigned int i = 0; i < line.size(); i++)
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

/**
 *
 */
void MainWindow::showTemplates()
{
  for (unsigned int i = 0; i < XMLName.size(); i++)
  {
    ui->haarCascadesListWidget->addItem(XMLName[i]);
  }
}

/**
 * Nacitanie haar kaskad
 */
void MainWindow::getItem(int row)
{
  std::string cesticka = XMLPath[row].toStdString();
  std::cout << cesticka << std::endl;

  if (cesticka.size() > 0)
  {
    m_haarCascadeLoaded = objectDetection.loadHaarObjectDetector(cesticka);
    //templateStatus = true;
  }
  else { m_haarCascadeLoaded = false; }
}

/**
 *
 */
void MainWindow::motionProcessing()
{
  //headCenter();
}

/**
 *
 */
void MainWindow::behaviorProcessing(QListWidgetItem *item)
{
    QString behaviourName = item->text();
    behaviorThread.setBehaviorProxy(robotIP, robotPort);
    behaviorThread.setSelectedBehavior(behaviourName.toStdString());
    behaviorThread.start(QThread::HighPriority);
    std::vector<std::string> runBehavior = behaviorProxy->getRunningBehaviors();
    if(runBehavior.empty() == true)
        behaviorThread.quit();
    //behaviorProxy->runBehavior(behaviourName.toStdString());

}

void MainWindow::behaviorProcessing(std::string selectedMotion)
{
    behaviorThread.setBehaviorProxy(robotIP, robotPort);
    behaviorThread.setSelectedBehavior(selectedMotion);
    behaviorThread.start(QThread::HighPriority);
    std::vector<std::string> runBehavior = behaviorProxy->getRunningBehaviors();
    if(runBehavior.empty() == true)
        behaviorThread.quit();
    //behaviorProxy->runBehavior(behaviourName.toStdString());

}

/**
 * Event handler zachytavajuci zmenu hodnoty v combo boxe
 *
 * @paramt int index - index zvolenej detekcie
 */
void MainWindow::on_chooseDetectionComboBox_activated(int index)
{
  qDebug() << index;
  m_selectedDetection = index;
}

/*
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
*/

void MainWindow::getChoosenObjectIndex(int row)
{
    if(row >= 0)
        choosenObjectIndex = row;
    qDebug() << choosenObjectIndex;
}

/**
 * Ovladanie pohybu robota Nao pomocou sipiek (hlava) a klaves WASD (chodza)
 *
 * @param QKeyEvent *event - TODO
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
  setFocus();
  qDebug() << event->key();

  //hlava

  switch (event->key())
  {
    case Qt::Key_Up:
      motionProxy->post.changeAngles("HeadPitch", -0.1, 0.05);
      break;

    case Qt::Key_Down:
      motionProxy->post.changeAngles("HeadPitch", 0.1, 0.05);
      break;

    case Qt::Key_Left:
      motionProxy->post.changeAngles("HeadYaw", 0.1, 0.05);
      break;

    case Qt::Key_Right:
      motionProxy->post.changeAngles("HeadYaw", -0.1, 0.05);
      break;

    case Qt::Key_Q:
      speed = 0;
      theta = 0;
      break;

    case Qt::Key_W:
      if (speed < 1) { speed += 0.05; }
      break;

    case Qt::Key_S:
      if (speed >- 1) { speed -= 0.05; }
      break;

    case Qt::Key_A:
      if (theta < 1) { theta += 0.05; }
      break;

    case Qt::Key_D:
      if (theta >- 1) { theta -= 0.05; }
      break;
  }

  std::cout << "theta: " << theta << "; speed: " << speed << std::endl;
  motionProxy->post.setWalkTargetVelocity(speed, 0, theta, 0.5);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        choosenObjectIndex = -1;
}
