#ifndef BEHAVIORTHREAD_H
#define BEHAVIORTHREAD_H

#include <QWidget>
#include <QThread>
#include <QDebug>

#include <string>
#include <iostream>

#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/almotionproxy.h>

class BehaviorThread : public QThread
{
public:
  std::string m_robotIP;
  int m_robotPort;
  std::string m_selectedBehavior;
  AL::ALBehaviorManagerProxy *m_behaviorProxy;
  AL::ALMotionProxy *m_motionProxy;

  BehaviorThread();
  ~BehaviorThread();
  void setBehaviorProxy(std::string &robotIP, int robotPort);
  void setSelectedBehavior(std::string &selectedBehavior);
  void run();
};

#endif // BEHAVIORTHREAD_H
