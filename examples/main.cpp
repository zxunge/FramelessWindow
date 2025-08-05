#include "framelessmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FramelessMainWindow w;
    w.show();
    return a.exec();
}
