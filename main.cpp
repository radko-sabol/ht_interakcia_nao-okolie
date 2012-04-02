#include <QtGui/QApplication>
#include "connecttonao.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConnectToNao connectToNao;
    connectToNao.show();

    return a.exec();
}
