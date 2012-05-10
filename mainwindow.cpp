/**
 * @file mainwindow.cpp
 *
 * @author Roman Michna
 * @author Radko Sabol
 * @author Peter Brudnak
 */

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
  m_isBrokerConnection = false;
  m_selectedDetection = none;
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

  if (m_isBrokerConnection) // kontrola, ci som pripojeny na robota
  {
    delete textToSpeech;
    camProxy->unsubscribe(clientName);
    delete camProxy;
    if (behaviorProxy->isBehaviorInstalled("sitDown"))
    {
      behaviorProxy->runBehavior("sitDown"); // predtym to bolo mimo podmienky a sposobovalo to "Assertion failed" pri zatvoreni okna s nastavenim IP adresy a portu
      motionProxy->setStiffnesses("Body", 0);
    }
    else if ((behaviorProxy->isBehaviorInstalled("SitDown")))
    {
      behaviorProxy->runBehavior("SitDown"); // predtym to bolo mimo podmienky a sposobovalo to "Assertion failed" pri zatvoreni okna s nastavenim IP adresy a portu
      motionProxy->setStiffnesses("Body", 0);
    }
    else
    {
      QMessageBox::warning(this, "Missing behavior", "The behavior \"sitDown\" or \"SitDown\" is missing.\nStiffness will be turned off - hold the robot.", QMessageBox::Ok, QMessageBox::Ok);
      motionProxy->setStiffnesses("Body", 0);
    }
    delete behaviorProxy;
    delete motionProxy;
  }
}

/**
 * Pripojenie na robota, ktoreho NaoQi bezi na danej IP adrese a porte (v pripade localhost adresy sa program pripoji na kameru v pocitaci)
 *
 * @param QString &IP - IP adresa robota
 * @param QString &port - port
 */
int MainWindow::getIpAndPort(QString &IP, QString &port)
{
  robotIP = IP.toStdString();
  robotPort = port.toInt();

  if (robotIP == "127.0.0.1") // pripajam sa na lokalnu kameru v PC
  {
    m_isBrokerConnection = false; // nastavenie flagu informujuceho, ze sa nepripajam na robota
    cap.open(0);
    if (! cap.isOpened()) // pripojenie na kameru v PC sa nepodarilo
    {
      QMessageBox::critical(this, "Camera connection problem", "Connection to camera failed", QMessageBox::Ok, QMessageBox::Ok);
      this->close();
      return -1;
    }

    // zablokovanie tlacidiel, ktore su pri pripojeni na kameru nepotrebne
    ui->stiffnessToggleButton->setEnabled(false);
    ui->speedTextBrowser->setEnabled(false);
    ui->angleTextBrowser->setEnabled(false);
  }
  else
  {
    m_isBrokerConnection = true; // nastavenie flagu informujuceho, ze sa pripajam na robota

    qDebug() << "IP: " << IP << ", port: " << port;
    textToSpeech = new AL::ALTextToSpeechProxy(robotIP, robotPort);
    camProxy = new AL::ALVideoDeviceProxy(robotIP, robotPort);
    motionProxy = new AL::ALMotionProxy(robotIP, robotPort);
    behaviorProxy = new AL::ALBehaviorManagerProxy(robotIP, robotPort);

    ui->videoStreamLabel->getMotionProxy(IP, port);

    motionProxy->setStiffnesses("Body", 1);

    behaviorProcessing("standUp"); // spustenie behavioru "standUp" cez vlakno

    camProxy->setParam(AL::kCameraSelectID, 1); // 0 - horna kamera; 1 - dolna kamera
    clientName = camProxy->subscribe("getImages", AL::kQVGA, AL::kBGRColorSpace, 30); // pripojenie na kameru robota

    // nacitanie, vypisanie a zoradenie nainstalovanych spravani v robotovi (podla abecedy)
    behaviourNames = behaviorProxy->getInstalledBehaviors();
    for (unsigned int i = 0; i < behaviourNames.size(); i++) { ui->behaviorsListWidget->addItem(QString(behaviourNames[i].c_str())); }
    ui->behaviorsListWidget->setSortingEnabled(true);
    ui->behaviorsListWidget->sortItems(Qt::AscendingOrder);

    imageMain = cvCreateImage(cvSize(320, 240), 8, 3);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(setStiffnessToggleButtonText()));
    connect(ui->behaviorsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(behaviorProcessing(QListWidgetItem*)));
    connect(this,SIGNAL(getSpeed(QString)),ui->speedTextBrowser,SLOT(setText(QString)));
    connect(this,SIGNAL(getAngle(QString)),ui->angleTextBrowser,SLOT(setText(QString)));
  }

  connect(m_timer, SIGNAL(timeout()), this, SLOT(imageProcessing()));
  connect(ui->loadHaarCascadeButton, SIGNAL(clicked()), this, SLOT(templateProcessing()));

  m_timer->start(33);

  return 0;
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
  if (m_isBrokerConnection)
  {
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
    else if ((detectedObjects.size() == 0) && (m_selectedDetection != none))
    {
      motion.stopWalking(*motionProxy);
    }
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
  if (m_isBrokerConnection) // kontrola, ci som pripojeny na robota
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
 * Vykreslenie obrazu do GUI (z premennej imageMat)
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

/**
 * Nacitanie konfiguracneho suboru s haar kaskadami (.xml subory), vypisanie nacitanych hodnot (z konfiguracneho suboru) do listWidgetu a pripojenie signalu, ktory spusti funkciu nacitania .xml suboru po kliknuti na hodnotu listWidgetu
 */
void MainWindow::templateProcessing()
{
  getConfig();
  showTemplates();
  connect(ui->haarCascadesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(getItem(int)));
}

/**
 * Nacitanie konfiguracneho suboru so zoznamom .xml suborov (haar kaskady)
 */
void MainWindow::getConfig()
{
  ui->haarCascadesListWidget->clear();

  XMLName.clear();
  XMLPath.clear();

  m_fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files (*.rnns)"));


  RNNSFile.setFileName(m_fileName);

  RNNSFile.open(QIODevice::ReadOnly | QIODevice::Text);

  QTextStream in(&RNNSFile);

  std::vector<QString> line;

  while (! in.atEnd())
  {
    line.push_back(in.readLine());
  }

  for (unsigned int i = 0; i < line.size(); i++)
  {
    if ((i % 2) == 0)
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
 * Vypisanie zoznamu nacitanych .xml suborov z konfiguracneho suboru do listWidgetu (ui->haarCascadesListWidget)
 */
void MainWindow::showTemplates()
{
  for (unsigned int i = 0; i < XMLName.size(); i++)
  {
    ui->haarCascadesListWidget->addItem(XMLName[i]);
  }
  ui->haarCascadesListWidget->setSortingEnabled(true);
  ui->haarCascadesListWidget->sortItems(Qt::AscendingOrder);
}

/**
 * Nacitanie Haar kaskad
 *
 * @param int row - index prvku, ktory oznacuje dany .xml, ktory sa nacita
 */
void MainWindow::getItem(int row)
{
  std::string cesticka = XMLPath[row].toStdString();
  //std::cout << cesticka << std::endl;

  if (cesticka.size() > 0)
  {
    m_haarCascadeLoaded = objectDetection.loadHaarObjectDetector(cesticka);
    // zistenie, ci bola kaskada uspesne nacitana (.xml subor)
    if (! m_haarCascadeLoaded) { QMessageBox::critical(this, "Haar cascade loading failed", "Haar cascade wasn't loaded properly.\nPlease, check the path to the Haar cascade in your config file.\n\nPath to haar cascade:\n " + QString::fromUtf8(cesticka.c_str()) + ".\n\nPath to config file: \n" + m_fileName + ".", QMessageBox::Ok, QMessageBox::Ok); }
  }
  else { m_haarCascadeLoaded = false; }
}

/**
 * Spustenie behavior v threade
 *
 * @param QListWidgetItem *item - prvok v listWidgetu (behavior)
 */
void MainWindow::behaviorProcessing(QListWidgetItem *item)
{
    QString behaviourName = item->text();
    behaviorThread.setBehaviorProxy(robotIP, robotPort);
    behaviorThread.setSelectedBehavior(behaviourName.toStdString());
    behaviorThread.start(QThread::HighPriority);
    std::vector<std::string> runBehavior = behaviorProxy->getRunningBehaviors();
    if (runBehavior.empty()) { behaviorThread.quit(); }
}

/**
 * Spustenie behavior v threade
 *
 * @param std::string selectedMotion - nazov behavioru
 */
void MainWindow::behaviorProcessing(std::string selectedMotion)
{
    behaviorThread.setBehaviorProxy(robotIP, robotPort);
    behaviorThread.setSelectedBehavior(selectedMotion);
    behaviorThread.start(QThread::HighPriority);
    std::vector<std::string> runBehavior = behaviorProxy->getRunningBehaviors();
    if (runBehavior.empty()) { behaviorThread.quit(); }
}

/**
 * Event handler zachytavajuci zmenu hodnoty v combo boxe
 *
 * @param int index - index zvolenej detekcie
 */
void MainWindow::on_chooseDetectionComboBox_activated(int index)
{
  //qDebug() << index;
  m_selectedDetection = index;
}

/**
 * Event handler zachytavajuci stlacenie tlacidla na prepnutie stiffness na Naovi
 */
void MainWindow::on_stiffnessToggleButton_clicked()
{
  std::vector<float> stiffnesses = motionProxy->getStiffnesses("Body");
  bool weakStifness = false;

  //std::cout << "stiffnesses: " << stiffnesses << std::endl;
  for (unsigned int i = 0; i < stiffnesses.size(); i++)
  {
    if ((stiffnesses[i] < 1) && (! weakStifness)) { weakStifness = true; }
  }

  if (weakStifness) { motionProxy->setStiffnesses("Body", 1); }
  else              { motionProxy->setStiffnesses("Body", 0); }
}

/**
 * Funkcia obsluhujuca nastavenie textu do buttonu na prepinanie stiffness na Naovi (ON/OFF)
 */
void MainWindow::setStiffnessToggleButtonText()
{
  std::vector<float> stiffnesses = motionProxy->getStiffnesses("Body");
  bool weakStifness = false;

  for (unsigned int i = 0; i < stiffnesses.size(); i++)
  {
    if ((stiffnesses[i] < 1) && (! weakStifness)) { weakStifness = true; }
  }

  if (weakStifness) { ui->stiffnessToggleButton->setText("Turn ON body stiffness"); }
  else              { ui->stiffnessToggleButton->setText("Turn OFF body stiffness"); }
}

/**
 * Ovladanie pohybu robota Nao pomocou sipiek (hlava) a klaves WASDQ (chodza)
 *
 * @param QKeyEvent *event - event obsahujuci, ze ktora klavesa bola stlacena
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
  // zaokruhlovanie - zdroj: http://stackoverflow.com/questions/1343890/rounding-number-to-2-decimal-places-in-c
  if (m_isBrokerConnection)
  {
    //setFocus();
    //qDebug() << event->key();

    switch (event->key())
    {
      case Qt::Key_Up: // pohyb hlavou hore
        motionProxy->post.changeAngles("HeadPitch", -0.1f, 0.05f);
        break;

      case Qt::Key_Down: // pohyb hlavou dole
        motionProxy->post.changeAngles("HeadPitch", 0.1f, 0.05f);
        break;

      case Qt::Key_Left: // pohyb hlavou dolava
        motionProxy->post.changeAngles("HeadYaw", 0.1f, 0.05f);
        break;

      case Qt::Key_Right: // pohyb hlavou doprava
        motionProxy->post.changeAngles("HeadYaw", -0.1f, 0.05f);
        break;

      case Qt::Key_Q: // zastavenie
        speed = 0;
        theta = 0;
        break;

      case Qt::Key_W: // pohyb dopredu
        if (speed < 1) { speed += 0.05f; }
        speed = floorf(speed * 100 + 0.5) / 100;
        break;

      case Qt::Key_S: // pohyb dozadu
        if (speed > -1) { speed -= 0.05f; }
        speed = floorf(speed * 100 + 0.5) / 100;
        break;

      case Qt::Key_A: // pohyb dolava
        if (theta < 1) { theta += 0.05f; }
        theta = floorf(theta * 100 + 0.5) / 100;
        break;

      case Qt::Key_D: // pohyb doprava
        if (theta > -1) { theta -= 0.05f; }
        theta = floorf(theta * 100 + 0.5) / 100;
        break;
    }

    //std::cout << "theta: " << theta << "; speed: " << speed << std::endl;
    motionProxy->post.setWalkTargetVelocity(speed, 0, theta, 1);

    QString Speed;
    Speed.setNum(speed);

    QString Theta;
    Theta.setNum(theta);

    emit getSpeed(Speed);
    emit getAngle(Theta);
  }
}
