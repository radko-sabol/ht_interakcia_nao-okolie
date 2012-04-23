#include "connecttonao_ipcombobox.hpp"

ConnectToNao_IPComboBox::ConnectToNao_IPComboBox(QWidget *parent) : QComboBox(parent)
{
}

void ConnectToNao_IPComboBox::keyPressEvent(QKeyEvent* event)
{
  // let base class handle the event
  QComboBox::keyPressEvent(event);

  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
  {
    std::cout << "ENTER" << std::endl;
    // accept enter/return events so they won't
    // be ever propagated to the parent dialog..
    event->accept();
    //connectToNao.
  }
}
