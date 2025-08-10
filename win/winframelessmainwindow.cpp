#include "winframelessmainwindow.h"
#include "ui_winframelessmainwindow.h"

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSize>
#include <QToolButton>
#include <QPixmap>
#include <QApplication>

#include <Windows.h>
#include <dwmapi.h>
#include <windowsx.h>

using namespace Qt::Literals::StringLiterals;

WinFramelessMainWindow::WinFramelessMainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::WinFramelessMainWindow),
      m_showFlag(false),
      m_windowMinWidth(500),
      m_windowMinHeight(300)
{
    Q_INIT_RESOURCE(fwres);

    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
    ::SetWindowLong(hwnd, GWL_STYLE, style & ~WS_CAPTION | WS_THICKFRAME);
    ::SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    setContentsMargins(1, 1, 1, 1);

    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(15);
    defaultShadow->setColor(Qt::black);
    defaultShadow->setOffset(0, 0);
    ui->centralWidget->setGraphicsEffect(defaultShadow);

    connect(ui->tbtnClose, &QToolButton::clicked, this, &WinFramelessMainWindow::tbtnCloseClicked);
    connect(ui->tbtnMin, &QToolButton::clicked, this, &WinFramelessMainWindow::tbtnMinClicked);
    connect(ui->tbtnMax, &QToolButton::clicked, this, &WinFramelessMainWindow::tbtnMaxClicked);

    ui->tbtnClose->installEventFilter(this);

    m_bodyLayout = new QHBoxLayout(ui->bodyWidget);
    m_bodyLayout->setContentsMargins(0, 0, 0, 0);
    m_cornerLayout = new QHBoxLayout(ui->cornerWidget);
}

WinFramelessMainWindow::~WinFramelessMainWindow()
{
    delete ui;
}

void WinFramelessMainWindow::showEvent(QShowEvent *event)
{
    setAttribute(Qt::WA_Mapped);
    QMainWindow::showEvent(event);

    resize(size());
}

void WinFramelessMainWindow::changeEvent(QEvent *event)
{
    if (QEvent::WindowStateChange == event->type()) {
        QWindowStateChangeEvent *stateEvent = dynamic_cast<QWindowStateChangeEvent *>(event);
        if (nullptr != stateEvent) {
            if (windowState() == Qt::WindowNoState
                && stateEvent->oldState() == Qt::WindowMaximized) {
                ui->centralWidget->setStyleSheet(u"#centralWidget {border-radius:10px;}"_s);
                ui->titleWidget->setStyleSheet(u"#titleWidget{"
                                               "background-color: rgb(226, 231, 237);"
                                               "color: rgb(0, 0, 0);"
                                               "border-top-left-radius: 10px;"
                                               "border-top-right-radius: 10px;}"_s);
                ui->tailWidget->setStyleSheet(u"#tailWidget{"
                                              "background-color: rgb(226, 231, 237);"
                                              "color: rgb(0, 0, 0);"
                                              "border-bottom-left-radius: 10px;"
                                              "border-bottom-right-radius: 10px;}"_s);
                ui->tbtnClose->setStyleSheet(u"QToolButton {color : rgb(255, 255, 255);"
                                             "border-top-right-radius: 9px;"
                                             "background-color: rgba(94, 255, 210, 0);"
                                             "border: none;}"
                                             "QToolButton::menu-indicator {"
                                             "image: none;}"
                                             "QToolButton:hover {"
                                             "color: rgb(255, 255, 255);"
                                             "background-color: rgb(180, 0, 0);"
                                             "border: none;}"_s);
                ui->tbtnMax->setIcon(QIcon(u":/icons/max-light.svg"_s));
            } else if (windowState() == Qt::WindowMaximized
                       && stateEvent->oldState() == Qt::WindowNoState) {
                ui->centralWidget->setStyleSheet(
                        u"#centralWidget {background-color: rgb(255, 255, 255);"
                        "border-radius:0px;}"_s);
                ui->titleWidget->setStyleSheet(u"#titleWidget{"
                                               "background-color: rgb(226, 231, 237);"
                                               "color: rgb(0, 0, 0);"
                                               "border-top-left-radius: 0px;"
                                               "border-top-right-radius: 0px;}"_s);
                ui->tailWidget->setStyleSheet(u"#tailWidget{"
                                              "background-color: rgb(226, 231, 237);"
                                              "color: rgb(0, 0, 0);"
                                              "border-bottom-left-radius: 0px;"
                                              "border-bottom-right-radius: 0px;}"_s);
                ui->tbtnClose->setStyleSheet(u"QToolButton {"
                                             "color: rgb(255, 255, 255);"
                                             "border-top-right-radius: 0px;"
                                             "background-color: rgba(94, 255, 210, 0);"
                                             "border: none;}"
                                             "QToolButton::menu-indicator {"
                                             "image: none;}"
                                             "QToolButton:hover {"
                                             "color: rgb(255, 255, 255);"
                                             "background-color: rgb(180, 0, 0);"
                                             "border: none;}"_s);
                ui->tbtnMax->setIcon(QIcon(u":/icons/normal-light.svg"_s));
            }
        }
    }
    QMainWindow::changeEvent(event);
}

void WinFramelessMainWindow::focusInEvent(QFocusEvent *event)
{
    QMainWindow::focusInEvent(event);
    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(15);
    defaultShadow->setColor(Qt::black);
    defaultShadow->setOffset(0, 0);
    ui->centralWidget->setGraphicsEffect(defaultShadow);
}

void WinFramelessMainWindow::focusOutEvent(QFocusEvent *event)
{
    QMainWindow::focusOutEvent(event);
    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(15);
    defaultShadow->setColor(Qt::transparent);
    defaultShadow->setOffset(0, 0);
    ui->centralWidget->setGraphicsEffect(defaultShadow);
}

bool WinFramelessMainWindow::nativeEvent(const QByteArray &eventType, void *message,
                                         qintptr *result)
{
    MSG *pMsg = reinterpret_cast<MSG *>(message);

    switch (pMsg->message) {
    case WM_NCCALCSIZE: {
        *result = 0;
        return true;
        break;
    }

    case WM_ACTIVATE: {
        MARGINS margins{ 1, 1, 1, 1 };
        HRESULT hr = S_OK;
        hr = DwmExtendFrameIntoClientArea(pMsg->hwnd, &margins);
        *result = hr;
        return true;
    }

    case WM_NCHITTEST: {
        bool isResize = false;

        POINT ptMouse{ GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
        RECT rcWindow;
        GetWindowRect(pMsg->hwnd, &rcWindow);
        RECT rcFrame{ 0, 0, 0, 0 };
        AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, 0);
        USHORT uRow = 1;
        USHORT uCol = 1;
        bool fOnResizeBorder = false;

        if (ptMouse.y >= rcWindow.top && ptMouse.y < rcWindow.top + 1) {
            fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
            uRow = 0;
            isResize = true;
        } else if (ptMouse.y < rcWindow.bottom && ptMouse.y >= rcWindow.bottom - 5) {
            uRow = 2;
            isResize = true;
        }
        if (ptMouse.x >= rcWindow.left && ptMouse.x < rcWindow.left + 5) {
            uCol = 0; // left side
            isResize = true;
        } else if (ptMouse.x < rcWindow.right && ptMouse.x >= rcWindow.right - 5) {
            uCol = 2; // right side
            isResize = true;
        }

        if (ptMouse.x >= rcWindow.left && ptMouse.x <= rcWindow.right - 135
            && ptMouse.y > rcWindow.top + 3 && ptMouse.y <= rcWindow.top + 30) {
            *result = HTCAPTION;
            return true;
        }

        LRESULT hitTests[3][3]{
            { HTTOPLEFT, fOnResizeBorder ? HTTOP : HTCAPTION, HTTOPRIGHT },
            { HTLEFT, HTNOWHERE, HTRIGHT },
            { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
        };

        if (isResize == true) {
            *result = hitTests[uRow][uCol];
            return true;
        } else {
            return false;
        }
    }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

bool WinFramelessMainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->tbtnClose) {
        switch (event->type()) {
        case QEvent::HoverEnter:
            ui->tbtnClose->setIcon(QIcon(u":/icons/close-dark.svg"_s));
            return true;

        case QEvent::HoverLeave:
            ui->tbtnClose->setIcon(QIcon(u":/icons/close-light.svg"_s));
            return true;

        default:
            break;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void WinFramelessMainWindow::tbtnCloseClicked()
{
    close();
}

void WinFramelessMainWindow::tbtnMaxClicked()
{
    if (!m_showFlag) {
        setContentsMargins(0, 0, 0, 0);
        setWindowState(Qt::WindowState::WindowMaximized);
        m_showFlag = true;
    } else {
        setContentsMargins(1, 1, 1, 1);
        setWindowState(Qt::WindowState::WindowNoState);
        showNormal();
        m_showFlag = false;
    }
}

void WinFramelessMainWindow::tbtnMinClicked()
{
    showMinimized();
}

void WinFramelessMainWindow::setWindowTitle(const QString &title)
{
    ui->labTitle->setText(title);
}

void WinFramelessMainWindow::showMessage(const QString &msg)
{
    ui->labStatus->setText(msg);
}

void WinFramelessMainWindow::addCornerWidget(QWidget *wgt)
{
    m_cornerLayout->addWidget(wgt);
}

void WinFramelessMainWindow::setWindowIcon(const QIcon &icon)
{
    ui->labIcon->setPixmap(icon.pixmap(QSize(16, 16)));
}

void WinFramelessMainWindow::addCentralWidget(QWidget *wgt)
{
    m_bodyLayout->addWidget(wgt);
}
