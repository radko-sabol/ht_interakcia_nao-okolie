#include "connecttonao.h"
#include "ui_connecttonao.h"

ConnectToNao::ConnectToNao(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectToNao)
{
    robotIP = '\0';
    robotPort = '\0';
    ui->setupUi(this);

}

ConnectToNao::~ConnectToNao()
{
    delete ui;
}

void ConnectToNao::on_lineEdit_textEdited(const QString &IP)
{
    robotIP = IP;
    qDebug() << robotIP;
}



void ConnectToNao::on_checkBox_toggled(bool checked)
{
    if(checked == true)
        robotPort = "9559";
    else
        robotPort = '\0';

    qDebug() << robotPort;
}

void ConnectToNao::on_lineEdit_2_textEdited(const QString &port)
{
    robotPort = port;
    qDebug() << robotPort;
}

void ConnectToNao::on_pushButton_pressed()
{
    this->close();
    mainWindow.show();
    mainWindow.getIpAndPort(robotIP, robotPort);

}
