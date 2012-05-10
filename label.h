#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>

#include <alproxies/almotionproxy.h>

#include "mainwindow.h"

class Label : public QLabel
{
  Q_OBJECT

public:
  Label(QWidget *parent = 0);
  ~Label();
  void getMotionProxy(QString& IP, QString& port);

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

private:
  QPoint m_firstPoint;
  QPoint m_secondPoint;
  QPoint m_point;
  AL::ALMotionProxy *motionProxy;

  bool m_isBrokerConnection; // indikator, ci je pripojeny na robota Nao
};

#endif // LABEL_H
