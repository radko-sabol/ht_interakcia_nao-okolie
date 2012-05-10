#ifndef CONNECTTONAO_IPCOMBOBOX_HPP
#define CONNECTTONAO_IPCOMBOBOX_HPP

#include <QComboBox>
#include <QKeyEvent>

#include <iostream>

#include "connecttonao.h"

class ConnectToNao_IPComboBox : public QComboBox
{
  Q_OBJECT

public:
  explicit ConnectToNao_IPComboBox(QWidget *parent = 0);
  ~ConnectToNao_IPComboBox();

protected:
  void keyPressEvent(QKeyEvent *event);

signals:
  void enterPressed();
};

#endif // CONNECTTONAO_IPCOMBOBOX_HPP
