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

protected:
  void keyPressEvent(QKeyEvent *event);

private:
  //ConnectToNao connectToNao; // TODO: toto robi nejaky problem...
};

#endif // CONNECTTONAO_IPCOMBOBOX_HPP
