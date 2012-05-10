/**
 * @file behaviorthread.cpp
 *
 * @author Roman Michna
 * @author Radko Sabol
 */

#include "behaviorthread.h"

/**
 * Konstruktor
 */
BehaviorThread::BehaviorThread()
{}

/**
 * Destruktor
 */
BehaviorThread::~BehaviorThread()
{
  //qDebug() << "Koniec vlakna";
}

/**
 * Nastavenie IP adresy a portu, ktore sa pouzije pri pripajani na robota Nao
 *
 * @param std::string &robotIP - IP adresa robota
 * @param int robotPort - port
 */
void BehaviorThread::setBehaviorProxy(std::string &robotIP, int robotPort)
{
  m_robotIP = robotIP;
  m_robotPort = robotPort;
}

/**
 * Nastavenie pozadovaneho spravania, ktore sa neskor v threade spusti
 *
 * @param std::string &selectedBehavior - nazov spravania
 */
void BehaviorThread::setSelectedBehavior(std::string &selectedBehavior)
{
  m_selectedBehavior = selectedBehavior;
}

/**
 * Spustenie spravania v threade
 */
void BehaviorThread::run()
{
  //qDebug() << "Spustenie vlakna";
  m_behaviorProxy = new AL::ALBehaviorManagerProxy(m_robotIP, m_robotPort);
  m_motionProxy = new AL::ALMotionProxy(m_robotIP, m_robotPort);

  std::vector<float> stiffnesses = m_motionProxy->getStiffnesses("Body");
  bool weakStifness = false;

  //std::cout << "stiffnesses: " << stiffnesses << std::endl;
  for (unsigned int i = 0; i < stiffnesses.size(); i++)
  {
    if ((stiffnesses[i] < 1) && (! weakStifness)) { weakStifness = true; }
  }
  if (weakStifness) { m_motionProxy->setStiffnesses("Body", 1); }

  if (m_behaviorProxy->isBehaviorInstalled(m_selectedBehavior))
  {
    m_behaviorProxy->runBehavior(m_selectedBehavior);
  }
  else
  {
    // TODO: nejaka chybova hlaska (QMessageBox mi nefunguje...)
  }

  delete m_behaviorProxy;
  delete m_motionProxy;

  exec();
}
