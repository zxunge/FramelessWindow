#ifndef FLESSHELPER_H
#define FLESSHELPER_H

#include "framelessmainwindow.h"
#include "framelessdialog.h"

#include <QWidget>
#include <QMainWindow>
#include <QStatusBar>
#include <QString>
#include <QGuiApplication>
#include <QtAssert>

#include <type_traits>

template <typename T>
class MakeMainWindowFrameless : public FramelessMainWindow
{
    static_assert(std::is_base_of<QMainWindow, T>::value, "Not derived from QMainWindow.");

public:
    MakeMainWindowFrameless(QWidget *parent = nullptr) : FramelessMainWindow()
    {
        w = new T(parent);
        Q_ASSERT(w != nullptr);
        Q_ASSERT(w->centralWidget() != nullptr);
        addCentralWidget(w->centralWidget());

        // Signals connection: StatusBar, Window Icon, Window Title
        Q_ASSERT(w->statusBar() != nullptr);
        connect(w->statusBar(), &QStatusBar::messageChanged, this,
                [this](const QString &msg) { showMessage(msg); });
        connect(w, &QMainWindow::windowIconChanged, this,
                [this](const QIcon &icon) { setWindowIcon(icon); });
        connect(w, &QMainWindow::windowTitleChanged, this,
                [this](const QString &title) { setWindowTitle(title); });

        setWindowIcon(w->windowIcon());
        setWindowTitle(w->windowTitle());
    }
    ~MakeMainWindowFrameless() { delete w; }

private:
    T *w; // Central widget used
};

template <typename T>
class MakeDialogFrameless : public FramelessDialog
{
    static_assert(std::is_base_of<QDialog, T>::value, "Not derived from QDialog.");

public:
    MakeDialogFrameless(QWidget *parent = nullptr) : FramelessDialog()
    {
        w = new T(parent);
        Q_ASSERT(w != nullptr);
		addCentralWidget(w);

        setWindowIcon(w->windowIcon());
        setWindowTitle(w->windowTitle());
    }
    ~MakeDialogFrameless() { delete w; }

private:
    T *w; // Central widget used
};

#endif // FLESSHELPER_H
