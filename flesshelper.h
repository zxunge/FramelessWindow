#ifndef FLESSHELPER_H
#define FLESSHELPER_H

#include "framelessmainwindow.h"

#include <QWidget>
#include <QMainWindow>

#include <type_traits>

template <typename T>
class MakeMainWindowFrameless : public FramelessMainWindow
{
    static_assert(std::is_base_of<QMainWindow, T>::value, "Not derived from QMainWindow.");

public:
    MakeMainWindowFrameless(QWidget *parent = nullptr) : FramelessMainWindow()
    {
        w = new T(parent);
        addCentralWidget(w->centralWidget());
    }
    ~MakeMainWindowFrameless() { delete w; }

private:
    T *w; // Central widget used
};

#endif // FLESSHELPER_H
