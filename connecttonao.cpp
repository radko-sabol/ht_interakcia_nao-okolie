#include "connecttonao.h"
#include "ui_connecttonao.h"

ConnectToNao::ConnectToNao(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectToNao)
{
    qDebug() << "ConnectToNao konstruktor";
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
    QString port;
    if (checked == true) { port = "9559"; }
    else                 { port = '\0'; }

    robotPort = port;
    ui->lineEdit_2->setText(port);

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

void ConnectToNao::closeEvent(QCloseEvent *event)
{
    qDebug() << "close event";
}

void ConnectToNao::on_lineEdit_textChanged(const QString &arg1)
{

}
