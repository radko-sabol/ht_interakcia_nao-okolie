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
  void on_defaultPort_toggled(bool checked);
  void on_PortLineEdit_textEdited(const QString &port);
  void on_PortLineEdit_textChanged(/*const QString &port*/);
  void on_PortLineEdit_returnPressed();
  void on_connectButton_clicked();

private:
  MainWindow mainWindow;
  Ui::ConnectToNao *ui;
  QString m_robotIP;
  QString m_robotPort;

  QValidator::State validate(QString &input) const;
  bool isDataValid();
};

#endif // CONNECTTONAO_H
