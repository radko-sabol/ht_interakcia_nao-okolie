/**
 * @file connecttonao.cpp
 *
 * @author Roman Michna
 * @author Radko Sabol
 */

#include "connecttonao.h"
#include "ui_connecttonao.h"

/**
 * Konstruktor dialogoveho okna pre vyplnenie parametrov pripojenia na robota
 */
ConnectToNao::ConnectToNao(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectToNao)
{
  m_robotIP = '\0';
  m_robotPort = '\0';
  ui->setupUi(this);
  ui->IPComboBox->setFocus();
  //ui->IPComboBox->addItem(""); // prazdna IP adresa
  ui->IPComboBox->addItem("147.232.24.");
  ui->IPComboBox->addItem("192.168.1.");
  ui->IPComboBox->addItem("127.0.0.1"); // localhost - prijimanie video streamu z kamery v PC
  ui->defaultPort->setChecked(true); // default zaskrtnutie pouzitia predvoleneho portu

  connect(ui->IPComboBox, SIGNAL(enterPressed()), this, SLOT(on_connectButton_clicked()));
}

/**
 * Destruktor
 */
ConnectToNao::~ConnectToNao()
{
  delete ui;
}

/**
 * Event handler zachytavajuci zmenu textu v poli pre IP adresu
 *
 * @param const QString &IP - IP adresa
 */
void ConnectToNao::on_IPComboBox_editTextChanged(const QString &IP)
{
  m_robotIP = IP;

  //qDebug() << "IP: " << robotIP;
  ui->connectButton->setEnabled(isDataValid()); // kontrola, ci je IP adresa validna a ak ano, tak sa povoli tlacidlo "Connect"
}

/**
 * Event handler zachytavajuci zmenu stavu checkboxu "Use default port"
 *
 * @param bool checked - boolean hodnota signalizujuca zaskrtnutie/odskrtnutie checkboxu
 */
void ConnectToNao::on_defaultPort_toggled(bool checked)
{
  QString port;
  if (checked == true) { port = "9559"; }
  else                 { port = ""; }

  m_robotPort = port;
  ui->PortLineEdit->setText(port);

  //qDebug() << robotPort;
}

/**
 * Event handler zachytavajuci zmenu stavu v poli pre port
 *
 * @param const QString &port - port
 */
void ConnectToNao::on_PortLineEdit_textEdited(const QString &port)
{
  m_robotPort = port;

  //qDebug() << robotPort;
}

/**
 * Event handler zachytavajuci zmenu stavu v poli pre port
 *
 * @param const QString &port - port
 */
void ConnectToNao::on_PortLineEdit_textChanged(/*const QString &port*/)
{
  ui->connectButton->setEnabled(isDataValid());
}

/**
 * Event handler zachytavajuci stlacenie klavesy Enter/Return v poli pre port
 */
void ConnectToNao::on_PortLineEdit_returnPressed()
{
  ui->connectButton->click();
}

/**
 * Event handler zachytavajuci stlacenie tlacidla pripojenia na robota (pressed down then released while the mouse cursor is inside the button)
 */
void ConnectToNao::on_connectButton_clicked()
{
  if (isDataValid()) // kontrola validnosti dat pred pokusom o pripojenie
  {
    this->close();
    mainWindow.show();
    mainWindow.getIpAndPort(m_robotIP, m_robotPort);
  }
  else
  {
    QMessageBox::critical(this, "Connect to Nao", "Bad connection parameters", QMessageBox::Ok, QMessageBox::Ok);
  }
}

/**
 * Validator IP adresy
 *
 * @param QString &input - hodnota v poli s IP adresou
 * @return QValidator::State - Acceptable/Invalid - vysledok po validacii IP adresy
 */
QValidator::State ConnectToNao::validate(QString &input) const
{
  // match against needed regexp
  QRegExp rx("(?:(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
  if (rx.exactMatch(input))
  {
    //std::cout << "IP is acceptable" << std::endl;
    return QValidator::Acceptable;
  }
  //std::cout << "IP is invalid" << std::endl;
  return QValidator::Invalid;
}

/**
 * Kontrola spravnosti vyplnenia IP adresy a portu
 *
 * @return bool - indikator, ci su data validne
 */
bool ConnectToNao::isDataValid()
{
  if ((validate(ui->IPComboBox->currentText()) == QValidator::Acceptable) && (ui->PortLineEdit->text() != "")) { return true; }
  else                                                                                                         { return false; }
}
