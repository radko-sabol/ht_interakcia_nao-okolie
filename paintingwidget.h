#ifndef PAINTINGWIDGET_H
#define PAINTINGWIDGET_H

#include <QWidget>
#include <QColor>
#include <QImage>
#include <QPoint>

class PaintingWidget : public QWidget
{
    Q_OBJECT

public:
    PaintingWidget(QWidget *parent = 0);

protected:
    //We reimplement the mouse/paint events handlers, inherited from QWidget
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    //Utility function that draws a line between the last point and the given end point
    void drawLineTo(const QPoint &endPoint);
    //Useful variables for drawing on the widget
    int     _penWidth;
    QColor  _penColor;
    QImage  _image;
    QPoint  _lastPoint;
};

#endif // PAINTINGWIDGET_H
