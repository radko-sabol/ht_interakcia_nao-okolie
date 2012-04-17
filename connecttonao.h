#ifndef CONNECTTONAO_H
#define CONNECTTONAO_H

//http://www.tinesoft.com/qt/use-custom-widgets-with-qt-designer-promotion-technique

#include <QWidget>
#include <QMessageBox>
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
  void on_IPComboBox_editTextChanged(const QString &IP);
  void on_PortLineEdit_textEdited(const QString &port);
  void on_PortLineEdit_textChanged(const QString &port);
  void on_defaultPort_toggled(bool checked);
  void on_connectButton_clicked();

  void on_PortLineEdit_returnPressed();

private:
  MainWindow mainWindow;
  Ui::ConnectToNao *ui;
  QString robotIP;
  QString robotPort;
  QValidator::State validate(QString &input) const;
  //void pushConnectButton();
  bool isDataValid();
};

#endif // CONNECTTONAO_H
