#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QPoint>

#include <alproxies/almotionproxy.h>

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
    QPoint firstPoint;
    QPoint secondPoint;
    QPoint point;
    AL::ALMotionProxy *motionProxy;

    bool m_isNaoqiEnabled;

};

#endif // LABEL_H
