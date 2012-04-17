#include "behaviorthread.h"

void BehaviorThread::setBehaviorProxy(std::string &robotIP, int robotPort)
{
    m_robotIP = robotIP;
    m_robotPort = robotPort;
    m_behaviorProxy = new AL::ALBehaviorManagerProxy(m_robotIP, m_robotPort);
}

void BehaviorThread::setSelectedBehavior(std::string &selectedBehavior)
{
    m_selectedBehavior = selectedBehavior;
}

void BehaviorThread::run()
{
    qDebug() << "Spustenie vlakna";
    m_behaviorProxy->runBehavior(m_selectedBehavior);
    exec();
}

BehaviorThread::~BehaviorThread()
{
    delete m_behaviorProxy;
    qDebug() << "Koniec vlakna";
}
