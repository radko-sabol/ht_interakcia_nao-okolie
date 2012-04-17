#ifndef BEHAVIORTHREAD_H
#define BEHAVIORTHREAD_H

#include <QThread>
#include <string>
#include <QDebug>

#include <alproxies/albehaviormanagerproxy.h>

class BehaviorThread : public QThread
{
public:
    std::string m_robotIP;
    int m_robotPort;
    std::string m_selectedBehavior;

    AL::ALBehaviorManagerProxy *m_behaviorProxy;

    ~BehaviorThread();

    void setBehaviorProxy(std::string &robotIP, int robotPort);
    void setSelectedBehavior(std::string &selectedBehavior);
    void run();
};

#endif // BEHAVIORTHREAD_H
