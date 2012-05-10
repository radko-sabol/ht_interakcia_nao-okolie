/**
 * @file label.cpp
 *
 * @author Roman Michna
 */

#include "label.h"

/**
 * Konstruktor
 */
Label::Label(QWidget *parent): QLabel(parent)
{
  m_isBrokerConnection = false;
}

/**
 * Destruktor
 */
Label::~Label()
{
  if(m_isBrokerConnection)
  {
    delete motionProxy;
  }
}

/**
 * Event handler zachytavajuci stlacenie tlacidla na mysi
 *
 * @param QMouseEvent *event - obsahuje informacie o evente (stlacene tlacidlo, pozicia kurzora,...)
 */
void Label::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) { m_firstPoint = event->pos(); }
}

/**
 * Event handler zachytavajuci zmenu polohy kurzora mysi
 *
 * @param QMouseEvent *event - obsahuje informacie o evente (pozicia kurzora,...)
 */
void Label::mouseMoveEvent(QMouseEvent *event)
{
  if (m_isBrokerConnection)
  {
    if (event->buttons() & Qt::LeftButton) { m_secondPoint = event->pos(); }

    m_point = m_firstPoint - m_secondPoint;

    if (m_point.x() > 0)      { motionProxy->changeAngles("HeadYaw", 0.1, 0.05f); }
    else if (m_point.x() < 0) { motionProxy->changeAngles("HeadYaw", -0.1, 0.05f); }

    if (m_point.y() > 0)      { motionProxy->changeAngles("HeadPitch", -0.1, 0.05f); }
    else if (m_point.y() < 0) { motionProxy->changeAngles("HeadPitch", 0.1, 0.05f); }
  }
}

/**
 * Pripojenie na motion proxy NaoQi
 *
 * @param QString &IP - IP adresa robota
 * @param QString &port - port na ktorom bezi NaoQi na robotovi
 */
void Label::getMotionProxy(QString &IP, QString &port)
{
  m_isBrokerConnection = true;
  motionProxy = new AL::ALMotionProxy(IP.toStdString(), port.toInt());
}
