#include "paintingwidget.h"
#include <QtGui>

PaintingWidget::PaintingWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);//for optimizing painting events
    _modified = false;
    _isDrawing = false;
    _penWidth = 10;
    _penColor = Qt::black;
}

void PaintingWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _lastPoint = event->pos();
        _isDrawing = true;
    }
}

void PaintingWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && _isDrawing)
        drawLineTo(event->pos());
}

void PaintingWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _isDrawing)
    {
        drawLineTo(event->pos());
        _isDrawing = false;
    }
}

void PaintingWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, _image, dirtyRect);
}

void PaintingWidget::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&_image);
    painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(_lastPoint, endPoint);
    _modified = true;

    int rad = (_penWidth / 2) + 2;
    update(QRect(_lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    _lastPoint = endPoint;
}
