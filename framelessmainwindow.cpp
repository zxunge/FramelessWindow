#include "framelessmainwindow.h"
#include "ui_framelessmainwindow.h"

#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSize>
#include <QToolButton>

using namespace Qt::Literals::StringLiterals;

FramelessMainWindow::FramelessMainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::FramelessMainWindow),
      m_showFlag(false),
      m_isPressedWidget(false),
      m_isMaxShow(false),
      m_windowMinWidth(500),
      m_windowMinHeight(300)
{
    Q_INIT_RESOURCE(resources);

    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setContentsMargins(5, 5, 5, 5);

    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(15);
    defaultShadow->setColor(QColor(0, 0, 0));
    defaultShadow->setOffset(0, 0);
    ui->centralWidget->setGraphicsEffect(defaultShadow);

    connect(ui->tbtnClose, &QToolButton::clicked, this, &FramelessMainWindow::tbtnCloseClicked);
    connect(ui->tbtnMin, &QToolButton::clicked, this, &FramelessMainWindow::tbtnMinClicked);
    connect(ui->tbtnMax, &QToolButton::clicked, this, &FramelessMainWindow::tbtnMaxClicked);

    setSupportStretch(true);

    m_bodyLayout = new QVBoxLayout(ui->bodyWidget);
}

FramelessMainWindow::~FramelessMainWindow()
{
    delete ui;
}

void FramelessMainWindow::calculateCurrentStrechRect()
{
    m_leftTopRect = QRect(0, 0, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_leftBottomRect =
            QRect(0, this->height() - STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH, STRETCH_RECT_WIDTH);
    m_rightTopRect =
            QRect(this->width() - STRETCH_RECT_WIDTH, 0, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_rightBottomRect =
            QRect(this->width() - STRETCH_RECT_WIDTH, this->height() - STRETCH_RECT_HEIGHT,
                  STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);

    m_topBorderRect = QRect(STRETCH_RECT_WIDTH, 0, this->width() - STRETCH_RECT_WIDTH * 2,
                            STRETCH_RECT_HEIGHT);
    m_rightBorderRect = QRect(this->width() - STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT,
                              STRETCH_RECT_WIDTH, this->height() - STRETCH_RECT_HEIGHT * 2);
    m_bottomBorderRect = QRect(STRETCH_RECT_WIDTH, this->height() - STRETCH_RECT_HEIGHT,
                               this->width() - STRETCH_RECT_WIDTH * 2, STRETCH_RECT_HEIGHT);
    m_leftBorderRect = QRect(0, STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH,
                             this->height() - STRETCH_RECT_HEIGHT * 2);
}

FramelessMainWindow::WindowStretchRectState
FramelessMainWindow::getCurrentStretchState(QPoint cursorPos)
{
    WindowStretchRectState stretchState;
    if (m_leftTopRect.contains(cursorPos)) {
        stretchState = LEFT_TOP_RECT;
    } else if (m_rightTopRect.contains(cursorPos)) {
        stretchState = RIGHT_TOP_RECT;
    } else if (m_rightBottomRect.contains(cursorPos)) {
        stretchState = RIGHT_BOTTOM_RECT;
    } else if (m_leftBottomRect.contains(cursorPos)) {
        stretchState = LEFT_BOTTOM_RECT;
    } else if (m_topBorderRect.contains(cursorPos)) {
        stretchState = TOP_BORDER;
    } else if (m_rightBorderRect.contains(cursorPos)) {
        stretchState = RIGHT_BORDER;
    } else if (m_bottomBorderRect.contains(cursorPos)) {
        stretchState = BOTTOM_BORDER;
    } else if (m_leftBorderRect.contains(cursorPos)) {
        stretchState = LEFT_BORDER;
    } else {
        stretchState = NO_SELECT;
    }
    return stretchState;
}

void FramelessMainWindow::updateMouseStyle(WindowStretchRectState stretchState)
{
    switch (stretchState) {
    case LEFT_TOP_RECT:
    case RIGHT_BOTTOM_RECT:
        setCursor(Qt::SizeFDiagCursor);
        break;

    case TOP_BORDER:
    case BOTTOM_BORDER:
        setCursor(Qt::SizeVerCursor);
        break;

    case RIGHT_TOP_RECT:
    case LEFT_BOTTOM_RECT:
        setCursor(Qt::SizeBDiagCursor);
        break;

    case LEFT_BORDER:
    case RIGHT_BORDER:
        setCursor(Qt::SizeHorCursor);
        break;

    default:
        setCursor(Qt::ArrowCursor);
        break;
    }
}

void FramelessMainWindow::updateWindowSize()
{
    QRect windowRect = m_windowRectBeforeStretch;
    int delValueX = m_startPoint.x() - m_endPoint.x();
    int delValueY = m_startPoint.y() - m_endPoint.y();

    switch (m_stretchRectState) {
    case LEFT_BORDER: {
        if (this->geometry().width() <= m_windowMinWidth && delValueX <= 0) {
            return;
        }
        QPoint bottomLeftPoint = windowRect.bottomLeft();
        bottomLeftPoint.setX(bottomLeftPoint.x() - delValueX);
        windowRect.setBottomLeft(bottomLeftPoint);
        this->setGeometry(windowRect);
        break;
    }

    case RIGHT_BORDER: {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setX(bottomRightPoint.x() - delValueX);
        windowRect.setBottomRight(bottomRightPoint);
        this->setGeometry(windowRect);
        break;
    }

    case TOP_BORDER: {
        if (this->geometry().height() <= m_windowMinHeight && delValueY <= 0) {
            return;
        }
        QPoint topLeftPoint = windowRect.topLeft();
        topLeftPoint.setY(topLeftPoint.y() - delValueY);
        windowRect.setTopLeft(topLeftPoint);
        this->setGeometry(windowRect);
        break;
    }

    case BOTTOM_BORDER: {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setY(bottomRightPoint.y() - delValueY);
        windowRect.setBottomRight(bottomRightPoint);
        this->setGeometry(windowRect);
        break;
    }

    case LEFT_TOP_RECT: {
        if (this->geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0
            && this->geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            return;
        }
        int a{};
        if (this->geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0) {
            a = 1;
        }
        if (this->geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            a = 2;
        }
        if (a == 0) {
            QPoint topLeftPoint = windowRect.topLeft();
            topLeftPoint.setX(topLeftPoint.x() - delValueX);
            topLeftPoint.setY(topLeftPoint.y() - delValueY);
            windowRect.setTopLeft(topLeftPoint);
            this->setGeometry(windowRect);
        } else if (a == 1) {
            QPoint topLeftPoint = windowRect.topLeft();
            topLeftPoint.setX(this->geometry().x());
            topLeftPoint.setY(topLeftPoint.y() - delValueY);
            windowRect.setTopLeft(topLeftPoint);
            this->setGeometry(windowRect);
        } else if (a == 2) {
            QPoint topLeftPoint = windowRect.topLeft();
            topLeftPoint.setX(topLeftPoint.x() - delValueX);
            topLeftPoint.setY(this->geometry().y());
            windowRect.setTopLeft(topLeftPoint);
            this->setGeometry(windowRect);
        }
        break;
    }

    case RIGHT_TOP_RECT: {
        if (this->geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0
            && this->geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            return;
        }
        int a{};
        if (this->geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0) {
            a = 1;
        }
        if (this->geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            a = 2;
        }

        if (a == 0) {
            QPoint topRightPoint = windowRect.topRight();
            topRightPoint.setX(topRightPoint.x() - delValueX);
            topRightPoint.setY(topRightPoint.y() - delValueY);
            windowRect.setTopRight(topRightPoint);
            this->setGeometry(windowRect);
        } else if (a == 1) {
            QPoint topRightPoint = windowRect.topRight();
            topRightPoint.setX(this->geometry().x());
            topRightPoint.setY(topRightPoint.y() - delValueY);
            windowRect.setTopRight(topRightPoint);
            this->setGeometry(windowRect);
        } else if (a == 2) {
            QPoint topRightPoint = windowRect.topRight();
            topRightPoint.setX(topRightPoint.x() - delValueX);
            topRightPoint.setY(this->geometry().y());
            windowRect.setTopRight(topRightPoint);
            this->setGeometry(windowRect);
        }
        break;
    }

    case RIGHT_BOTTOM_RECT: {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setX(bottomRightPoint.x() - delValueX);
        bottomRightPoint.setY(bottomRightPoint.y() - delValueY);
        windowRect.setBottomRight(bottomRightPoint);
        this->setGeometry(windowRect);
        break;
    }

    case LEFT_BOTTOM_RECT: {
        if (this->geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0
            && this->geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            return;
        }
        int a{};
        if (this->geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0) {
            a = 1;
        }
        if (this->geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            a = 2;
        }
        if (a == 0) {
            QPoint bottomLeftPoint = windowRect.bottomLeft();
            bottomLeftPoint.setX(bottomLeftPoint.x() - delValueX);
            bottomLeftPoint.setY(bottomLeftPoint.y() - delValueY);
            windowRect.setBottomLeft(bottomLeftPoint);
            this->setGeometry(windowRect);
        } else if (a == 1) {
            QPoint bottomLeftPoint = windowRect.bottomLeft();
            bottomLeftPoint.setX(this->geometry().x());
            bottomLeftPoint.setY(bottomLeftPoint.y() - delValueY);
            windowRect.setBottomLeft(bottomLeftPoint);
            this->setGeometry(windowRect);
        } else if (a == 2) {
            QPoint bottomLeftPoint = windowRect.bottomLeft();
            bottomLeftPoint.setX(bottomLeftPoint.x() - delValueX);
            bottomLeftPoint.setY(this->geometry().y());
            windowRect.setBottomLeft(bottomLeftPoint);
            this->setGeometry(windowRect);
        }
        break;
    }

    default:
        return;
    }
}

void FramelessMainWindow::setSupportStretch(bool isSupportStretch)
{
    m_isSupportStretch = isSupportStretch;
    setMouseTracking(isSupportStretch);
    ui->tailWidget->setMouseTracking(isSupportStretch);
    ui->titleWidget->setMouseTracking(isSupportStretch);
    ui->centralWidget->setMouseTracking(isSupportStretch);
    ui->bodyWidget->setMouseTracking(isSupportStretch);
}

void FramelessMainWindow::mousePressEvent(QMouseEvent *event)
{
    if (ui->titleWidget->underMouse() && !m_showFlag) {
        m_isPressedWidget = true;
    }
    m_last = event->globalPosition();

    if (m_stretchRectState != NO_SELECT && event->button() == Qt::LeftButton) {
        m_isMousePressed = true;
        m_startPoint = mapToGlobal(event->pos());
        m_windowRectBeforeStretch = geometry();
    }
    QMainWindow::mousePressEvent(event);
}

void FramelessMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressedWidget) {
        int dx = event->globalPosition().x() - m_last.x();
        int dy = event->globalPosition().y() - m_last.y();
        m_last = event->globalPosition();
        move(x() + dx, y() + dy);
    }

    if (!m_isMousePressed) {
        QPoint cursorPos = event->pos();
        m_stretchRectState = getCurrentStretchState(cursorPos);
        updateMouseStyle(m_stretchRectState);
    } else {
        m_endPoint = mapToGlobal(event->pos());
        updateWindowSize();
    }
    QMainWindow::mouseMoveEvent(event);
}

void FramelessMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressedWidget = false;
    m_isMousePressed = false;
    calculateCurrentStrechRect();
    QMainWindow::mouseReleaseEvent(event);
}

void FramelessMainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    tbtnMaxClicked();
    QMainWindow::mouseDoubleClickEvent(event);
}

void FramelessMainWindow::showEvent(QShowEvent *event)
{
    calculateCurrentStrechRect();

    setAttribute(Qt::WA_Mapped);
    QMainWindow::showEvent(event);

    QSize oldSize = size();
    resize(oldSize + QSize(10, 10));
    resize(oldSize);
}

void FramelessMainWindow::changeEvent(QEvent *event)
{
    if (QEvent::WindowStateChange == event->type()) {
        QWindowStateChangeEvent *stateEvent = dynamic_cast<QWindowStateChangeEvent *>(event);
        if (nullptr != stateEvent) {
            if (windowState() == Qt::WindowNoState
                && stateEvent->oldState() == Qt::WindowMaximized) {
                ui->centralWidget->setStyleSheet("#centralWidget {border-radius:10px;}");
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
                                             "background-color: rgb(200, 0, 0);"
                                             "border: none;}"_s);
                ui->tbtnMax->setIcon(QIcon(u":/icons/max-light.png"_s));
            } else if (windowState() == Qt::WindowMaximized
                       && stateEvent->oldState() == Qt::WindowNoState) {
                ui->centralWidget->setStyleSheet(
                        u"#centralWidget {background-color: rgb(255, 255, 255);"
                        "border-radius:0px;}"_s);
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
                                             "background-color: rgb(200, 0, 0);"
                                             "border: none;}"_s);
                ui->tbtnMax->setIcon(QIcon(":/icons/normal-light.png"));
            }
        }
    }
}

void FramelessMainWindow::tbtnCloseClicked()
{
    close();
}

void FramelessMainWindow::tbtnMaxClicked()
{
    if (!m_showFlag) {
        setContentsMargins(0, 0, 0, 0);
        setWindowState(Qt::WindowState::WindowMaximized);
        m_isMaxShow = true;
        m_showFlag = true;
    } else {
        setContentsMargins(5, 5, 5, 5);
        setWindowState(Qt::WindowState::WindowNoState);
        showNormal();
        m_isMaxShow = false;
        m_showFlag = false;
    }
}

void FramelessMainWindow::tbtnMinClicked()
{
    showMinimized();
}
