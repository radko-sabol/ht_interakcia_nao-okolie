#include "behaviorthread.h"

void BehaviorThread::setBehaviorProxy(std::string &robotIP, int robotPort)
{
    m_robotIP = robotIP;
    m_robotPort = robotPort;
}

void BehaviorThread::setSelectedBehavior(std::string &selectedBehavior)
{
    m_selectedBehavior = selectedBehavior;
}

void BehaviorThread::run()
{
  qDebug() << "Spustenie vlakna";
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
  m_behaviorProxy->runBehavior(m_selectedBehavior);

  delete m_behaviorProxy;
  delete m_motionProxy;

  exec();
}

BehaviorThread::~BehaviorThread()
{
  qDebug() << "Koniec vlakna";
}
