#ifndef CONNECTTONAO_H
#define CONNECTTONAO_H

//http://www.tinesoft.com/qt/use-custom-widgets-with-qt-designer-promotion-technique

#include <QWidget>
#include <QDebug>
#include "mainwindow.h"

namespace Ui
{
class ConnectToNao;
}

class ConnectToNao : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConnectToNao(QWidget *parent = 0);
    ~ConnectToNao();
    
private slots:
    void on_lineEdit_textEdited(const QString &IP);
    void on_lineEdit_2_textEdited(const QString &port);
    void on_checkBox_toggled(bool checked);
    void on_pushButton_pressed();

private:
    MainWindow mainWindow;
    Ui::ConnectToNao *ui;
    QString robotIP;
    QString robotPort;
};

#endif // CONNECTTONAO_H
