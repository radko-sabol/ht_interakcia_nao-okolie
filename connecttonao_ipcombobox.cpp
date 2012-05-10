/**
 * @file connecttonao_ipcombobox.cpp
 *
 * @author Roman Michna
 * @author Radko Sabol
 */

#include "connecttonao_ipcombobox.hpp"

/**
 * Konstruktor
 */
ConnectToNao_IPComboBox::ConnectToNao_IPComboBox(QWidget *parent) : QComboBox(parent)
{}

/**
 * Destruktor
 */
ConnectToNao_IPComboBox::~ConnectToNao_IPComboBox()
{}

/**
 * Event handler zachytavajuci stlacenie klavesy v poli pre vyplnenie IP adresy
 */
void ConnectToNao_IPComboBox::keyPressEvent(QKeyEvent* event)
{
  QComboBox::keyPressEvent(event);

  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
  {
    emit enterPressed();
    event->accept();
  }
}
