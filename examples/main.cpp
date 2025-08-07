#include "framelessmainwindow.h"
#include "widget.h"

#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FramelessMainWindow w;
    w.setWindowTitle(QObject::tr("Hello"));
    w.showMessage(QObject::tr("This is Hello Window."));
    w.setIcon(QIcon(":/icons/icon.png"));
    w.addCentralWidget(new Widget);
    w.show();
    return a.exec();
}
