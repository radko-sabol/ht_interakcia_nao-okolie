#include "label.h"

Label::Label(QWidget *parent): QLabel(parent)
{
    m_isNaoqiEnabled = false;
}

Label::~Label()
{
    if(m_isNaoqiEnabled)
    {
        delete motionProxy;
    }
}

void Label::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        firstPoint = event->pos();
}

void Label::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        secondPoint = event->pos();


    point = firstPoint - secondPoint;

    if(point.x() > 0)
        motionProxy->changeAngles("HeadYaw", 0.1, 0.05f);
    if(point.x() < 0)
        motionProxy->changeAngles("HeadYaw", -0.1, 0.05f);
    if(point.y() > 0)
        motionProxy->changeAngles("HeadPitch", -0.1, 0.05f);
    if(point.y() < 0)
        motionProxy->changeAngles("HeadPitch", 0.1, 0.05f);
}

void Label::getMotionProxy(QString& IP, QString& port)
{
    m_isNaoqiEnabled = true;
    motionProxy = new AL::ALMotionProxy(IP.toStdString(), port.toInt());
}
