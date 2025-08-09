#include "flesshelper.h"
#include "mainwindow.h"

#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MakeMainWindowFrameless<MainWindow> w;
    w.setWindowTitle(QObject::tr("Hello"));
    w.showMessage(QObject::tr("This is Hello Window."));
    w.setWindowIcon(QIcon(":/icons/icon.png"));
    w.show();
    return a.exec();
}
