#include "flesshelper.h"
#include "winframelessmainwindow.h"
#include "mainwindow.h"

#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QApplication a(argc, argv);
    MakeMainWindowFrameless<MainWindow> w;
    w.setWindowTitle(QObject::tr("Hello"));
    w.showMessage(QObject::tr("This is Hello Window."));
    w.setWindowIcon(QIcon(":/icons/icon.png"));
    w.show();

    WinFramelessMainWindow w1;
    w1.show();
    return a.exec();
}
