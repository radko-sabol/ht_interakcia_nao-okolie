#ifndef BEHAVIORTHREAD_H
#define BEHAVIORTHREAD_H

#include <QThread>
#include <string>
#include <iostream>
#include <QDebug>

#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/almotionproxy.h>

class BehaviorThread : public QThread
{
private:
  bool naoqiBehaviorInitialized;

public:
  std::string m_robotIP;
  int m_robotPort;
  std::string m_selectedBehavior;

  AL::ALBehaviorManagerProxy *m_behaviorProxy;
  AL::ALMotionProxy *m_motionProxy;

  ~BehaviorThread();

  void setBehaviorProxy(std::string &robotIP, int robotPort);
  void setSelectedBehavior(std::string &selectedBehavior);
  void run();
};

#endif // BEHAVIORTHREAD_H
