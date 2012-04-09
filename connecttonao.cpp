#include "connecttonao.h"
#include "ui_connecttonao.h"

/**
 * Konstruktor dialogoveho okna pre vyplnenie parametrov pripojenia na robota
 */
ConnectToNao::ConnectToNao(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectToNao)
{
  robotIP = '\0';
  robotPort = '\0';
  ui->setupUi(this);
  ui->comboBox->setFocus();
  //ui->comboBox->addItem(""); // prazdna IP adresa
  ui->comboBox->addItem("147.232.24.");
  ui->comboBox->addItem("127.0.0.1"); // localhost - prijimanie video streamu z kamery v PC
  ui->checkBox->setChecked(true); // default zaskrtnutie pouzitia predvoleneho portu
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
void ConnectToNao::on_comboBox_editTextChanged(const QString &IP)
{
  robotIP = IP;

  qDebug() << "IP: " << robotIP;
  ui->pushButton->setEnabled(isDataValid());
}

/**
 * Event handler zachytavajuci zmenu stavu checkboxu "Use default port"
 *
 * @param bool checked - boolean hodnota signalizujuca zaskrtnutie/odskrtnutie checkboxu
 */
void ConnectToNao::on_checkBox_toggled(bool checked)
{
  QString port;
  if (checked == true) { port = "9559"; }
  else                 { port = ""; }

  robotPort = port;
  ui->lineEdit_2->setText(port);

  qDebug() << robotPort;
}

/**
 * Event handler zachytavajuci zmenu stavu v poli pre port
 *
 * @param const QString &port - port
 */
void ConnectToNao::on_lineEdit_2_textEdited(const QString &port)
{
  robotPort = port;

  qDebug() << robotPort;
}

/**
 * Event handler zachytavajuci zmenu stavu v poli pre port
 *
 * @param const QString &port - port
 */
void ConnectToNao::on_lineEdit_2_textChanged(const QString &port)
{
  ui->pushButton->setEnabled(isDataValid());
}

/**
 * Event handler zachytavajuci stlacenie tlacidla pripojenia na robota
 */
void ConnectToNao::on_pushButton_pressed()
{
  if (isDataValid()) // kontrola validnosti dat pred pokusom o pripojenie
  {
    this->close();
    mainWindow.show();
    mainWindow.getIpAndPort(robotIP, robotPort);
  }
  else
  {
    QMessageBox::critical(this, "Connect to Nao", "Bad connection parameters", QMessageBox::Ok, QMessageBox::Ok);
  }
}

/**
 * [ALPHA] Validator a pomocnik pri vyplnovani IP adresy
 *
 * @param QString &input - hodnota v poli s IP adresou
 * @return QValidator::State - Acceptable/Invalid - vysledok po validacii IP adresy
 */
QValidator::State ConnectToNao::validate(QString &input) const
{
  //std::cout << "validujem " << ui->lineEdit->text().toStdString() << std::endl;

  /*
  // remove trailing comma
  if (ui->lineEdit->text().endsWith('.')) {
  //qDebug() << "backspace";
  //ui->lineEdit->backspace();
  //input.chop(1);
  }
  */

  //qDebug() << "pred: " << input;
  // insert comma when third hex in a row was entered
  QRegExp rxThreeHexAtTheEnd("(?:[0-9]{1,3}\\.){0,2}(?:[0-9]{3})");
  if (rxThreeHexAtTheEnd.exactMatch(input))
  {
    //qDebug() << "vkladam bodku";
    //ui->lineEdit->setText(ui->lineEdit->text() + ".");
    ui->comboBox->setItemText(ui->comboBox->currentIndex(), ui->comboBox->currentText() + ".");
    //ui->lineEdit->insert(".");
    //input.insert(input.length()-1, '.');
    //pos = input.length();
  }

  QRegExp rxThreeHexAtTheEnd2("[\\d{1-3}\\.]*(?:[0-9]{4,})[\\D]*");
  if (rxThreeHexAtTheEnd2.exactMatch(input))
  {
    //qDebug() << "backspace2";
    //ui->lineEdit->backspace();
    //ui->lineEdit->insert(".");
    //input.insert(input.length()-1, '.');
    //pos = input.length();
  }

  input = ui->comboBox->currentText();
  qDebug() << input;

  // match against needed regexp
  QRegExp rx("(?:(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
  if (rx.exactMatch(input))
  {
    std::cout << "IP is acceptable" << std::endl;
    return QValidator::Acceptable;
  }
  std::cout << "IP is invalid" << std::endl;
  return QValidator::Invalid;
}

bool ConnectToNao::isDataValid()
{
  if ((validate(ui->comboBox->currentText()) == QValidator::Acceptable) && (ui->lineEdit_2->text() != "")) { return true; }
  else                                                                                                     { return false; }
}
