#include "framelessdialog.h"
#include "ui_framelessdialog.h"

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSize>
#include <QToolButton>
#include <QPixmap>
#include <QApplication>

using namespace Qt::Literals::StringLiterals;

FramelessDialog::FramelessDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::FramelessDialog),
      m_showFlag(false),
      m_isPressedWidget(false),
      m_windowMinWidth(500),
      m_windowMinHeight(300)
{
    Q_INIT_RESOURCE(fwres);

    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setContentsMargins(5, 5, 5, 5);
    setSizeGripEnabled(false);

    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(15);
    defaultShadow->setColor(Qt::black);
    defaultShadow->setOffset(0, 0);
    ui->centralWidget->setGraphicsEffect(defaultShadow);

    connect(ui->tbtnClose, &QToolButton::clicked, this, &FramelessDialog::tbtnCloseClicked);
    connect(ui->tbtnMin, &QToolButton::clicked, this, &FramelessDialog::tbtnMinClicked);
    connect(ui->tbtnMax, &QToolButton::clicked, this, &FramelessDialog::tbtnMaxClicked);

    ui->tbtnClose->installEventFilter(this);
    ui->tbtnMax->setVisible(false);
    ui->tbtnMin->setVisible(false);

    setSupportStretch(true);

    m_bodyLayout = new QHBoxLayout(ui->bodyWidget);
    m_bodyLayout->setContentsMargins(0, 0, 0, 0);
    m_cornerLayout = new QHBoxLayout(ui->cornerWidget);
}

FramelessDialog::~FramelessDialog()
{
    delete ui;
}

void FramelessDialog::calculateCurrentStrechRect()
{
    m_leftTopRect = QRect(0, 0, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_leftBottomRect =
            QRect(0, height() - STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH, STRETCH_RECT_WIDTH);
    m_rightTopRect =
            QRect(width() - STRETCH_RECT_WIDTH, 0, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_rightBottomRect = QRect(width() - STRETCH_RECT_WIDTH, height() - STRETCH_RECT_HEIGHT,
                              STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);

    m_topBorderRect =
            QRect(STRETCH_RECT_WIDTH, 0, width() - STRETCH_RECT_WIDTH * 2, STRETCH_RECT_HEIGHT);
    m_rightBorderRect = QRect(width() - STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH,
                              height() - STRETCH_RECT_HEIGHT * 2);
    m_bottomBorderRect = QRect(STRETCH_RECT_WIDTH, height() - STRETCH_RECT_HEIGHT,
                               width() - STRETCH_RECT_WIDTH * 2, STRETCH_RECT_HEIGHT);
    m_leftBorderRect =
            QRect(0, STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH, height() - STRETCH_RECT_HEIGHT * 2);
}

FramelessDialog::WindowStretchRectState FramelessDialog::getCurrentStretchState(QPoint cursorPos)
{
    WindowStretchRectState stretchState;
    if (m_leftTopRect.contains(cursorPos))
        stretchState = LEFT_TOP_RECT;
    else if (m_rightTopRect.contains(cursorPos))
        stretchState = RIGHT_TOP_RECT;
    else if (m_rightBottomRect.contains(cursorPos))
        stretchState = RIGHT_BOTTOM_RECT;
    else if (m_leftBottomRect.contains(cursorPos))
        stretchState = LEFT_BOTTOM_RECT;
    else if (m_topBorderRect.contains(cursorPos))
        stretchState = TOP_BORDER;
    else if (m_rightBorderRect.contains(cursorPos))
        stretchState = RIGHT_BORDER;
    else if (m_bottomBorderRect.contains(cursorPos))
        stretchState = BOTTOM_BORDER;
    else if (m_leftBorderRect.contains(cursorPos))
        stretchState = LEFT_BORDER;
    else
        stretchState = NO_SELECT;
    return stretchState;
}

void FramelessDialog::updateMouseStyle(WindowStretchRectState stretchState)
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

void FramelessDialog::updateWindowSize()
{
    QRect windowRect = m_windowRectBeforeStretch;
    int delValueX = m_startPoint.x() - m_endPoint.x();
    int delValueY = m_startPoint.y() - m_endPoint.y();

    switch (m_stretchRectState) {
    case LEFT_BORDER: {
        if (geometry().width() <= m_windowMinWidth && delValueX <= 0)
            return;

        QPoint bottomLeftPoint = windowRect.bottomLeft();
        bottomLeftPoint.setX(bottomLeftPoint.x() - delValueX);
        windowRect.setBottomLeft(bottomLeftPoint);
        setGeometry(windowRect);
        break;
    }

    case RIGHT_BORDER: {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setX(bottomRightPoint.x() - delValueX);
        windowRect.setBottomRight(bottomRightPoint);
        setGeometry(windowRect);
        break;
    }

    case TOP_BORDER: {
        if (geometry().height() <= m_windowMinHeight && delValueY <= 0)
            return;

        QPoint topLeftPoint = windowRect.topLeft();
        topLeftPoint.setY(topLeftPoint.y() - delValueY);
        windowRect.setTopLeft(topLeftPoint);
        setGeometry(windowRect);
        break;
    }

    case BOTTOM_BORDER: {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setY(bottomRightPoint.y() - delValueY);
        windowRect.setBottomRight(bottomRightPoint);
        setGeometry(windowRect);
        break;
    }

    case LEFT_TOP_RECT: {
        if (geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0
            && geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0)
            return;

        if (geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0) {
            QPoint topLeftPoint = windowRect.topLeft();
            topLeftPoint.setX(geometry().x());
            topLeftPoint.setY(topLeftPoint.y() - delValueY);
            windowRect.setTopLeft(topLeftPoint);
            setGeometry(windowRect);
        } else if (geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            QPoint topLeftPoint = windowRect.topLeft();
            topLeftPoint.setX(topLeftPoint.x() - delValueX);
            topLeftPoint.setY(geometry().y());
            windowRect.setTopLeft(topLeftPoint);
            setGeometry(windowRect);
        } else {
            QPoint topLeftPoint = windowRect.topLeft();
            topLeftPoint.setX(topLeftPoint.x() - delValueX);
            topLeftPoint.setY(topLeftPoint.y() - delValueY);
            windowRect.setTopLeft(topLeftPoint);
            setGeometry(windowRect);
        }
        break;
    }

    case RIGHT_TOP_RECT: {
        if (geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0
            && geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0)
            return;

        if (geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0) {
            QPoint topRightPoint = windowRect.topRight();
            topRightPoint.setX(geometry().x());
            topRightPoint.setY(topRightPoint.y() - delValueY);
            windowRect.setTopRight(topRightPoint);
            setGeometry(windowRect);
        } else if (geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            QPoint topRightPoint = windowRect.topRight();
            topRightPoint.setX(topRightPoint.x() - delValueX);
            topRightPoint.setY(geometry().y());
            windowRect.setTopRight(topRightPoint);
            setGeometry(windowRect);
        } else {
            QPoint topRightPoint = windowRect.topRight();
            topRightPoint.setX(topRightPoint.x() - delValueX);
            topRightPoint.setY(topRightPoint.y() - delValueY);
            windowRect.setTopRight(topRightPoint);
            setGeometry(windowRect);
        }
        break;
    }

    case RIGHT_BOTTOM_RECT: {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setX(bottomRightPoint.x() - delValueX);
        bottomRightPoint.setY(bottomRightPoint.y() - delValueY);
        windowRect.setBottomRight(bottomRightPoint);
        setGeometry(windowRect);
        break;
    }

    case LEFT_BOTTOM_RECT: {
        if (geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0
            && geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0)
            return;

        if (geometry().width() - 3 <= m_windowMinWidth && delValueX <= 0) {
            QPoint bottomLeftPoint = windowRect.bottomLeft();
            bottomLeftPoint.setX(geometry().x());
            bottomLeftPoint.setY(bottomLeftPoint.y() - delValueY);
            windowRect.setBottomLeft(bottomLeftPoint);
            setGeometry(windowRect);
        } else if (geometry().height() - 3 <= m_windowMinHeight && delValueY <= 0) {
            QPoint bottomLeftPoint = windowRect.bottomLeft();
            bottomLeftPoint.setX(bottomLeftPoint.x() - delValueX);
            bottomLeftPoint.setY(geometry().y());
            windowRect.setBottomLeft(bottomLeftPoint);
            setGeometry(windowRect);
        } else {
            QPoint bottomLeftPoint = windowRect.bottomLeft();
            bottomLeftPoint.setX(bottomLeftPoint.x() - delValueX);
            bottomLeftPoint.setY(bottomLeftPoint.y() - delValueY);
            windowRect.setBottomLeft(bottomLeftPoint);
            setGeometry(windowRect);
        }
        break;
    }

    default:
        break;
    }
}

void FramelessDialog::setSupportStretch(bool isSupportStretch)
{
    m_isSupportStretch = isSupportStretch;
    setMouseTracking(isSupportStretch);
    const QWidgetList allWidgets = QApplication::allWidgets();
    for (QWidget *widget : allWidgets)
        widget->setMouseTracking(isSupportStretch);
}

void FramelessDialog::mousePressEvent(QMouseEvent *event)
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
    QDialog::mousePressEvent(event);
}

void FramelessDialog::mouseMoveEvent(QMouseEvent *event)
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

    QDialog::mouseMoveEvent(event);
}

void FramelessDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressedWidget = false;
    m_isMousePressed = false;
    calculateCurrentStrechRect();
    QDialog::mouseReleaseEvent(event);
}

void FramelessDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (ui->titleWidget->underMouse())
        tbtnMaxClicked();

    QDialog::mouseDoubleClickEvent(event);
}

void FramelessDialog::showEvent(QShowEvent *event)
{
    calculateCurrentStrechRect();

    setAttribute(Qt::WA_Mapped);
    QDialog::showEvent(event);

    QSize oldSize = size();
    resize(oldSize + QSize(10, 10));
    resize(oldSize);
}

void FramelessDialog::changeEvent(QEvent *event)
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
    QDialog::changeEvent(event);
}

void FramelessDialog::focusInEvent(QFocusEvent *event)
{
    QDialog::focusInEvent(event);
    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(15);
    defaultShadow->setColor(Qt::black);
    defaultShadow->setOffset(0, 0);
    ui->centralWidget->setGraphicsEffect(defaultShadow);
}

void FramelessDialog::focusOutEvent(QFocusEvent *event)
{
    QDialog::focusOutEvent(event);
    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(15);
    defaultShadow->setColor(Qt::transparent);
    defaultShadow->setOffset(0, 0);
    ui->centralWidget->setGraphicsEffect(defaultShadow);
}

bool FramelessDialog::eventFilter(QObject *obj, QEvent *event)
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
    return QDialog::eventFilter(obj, event);
}

void FramelessDialog::tbtnCloseClicked()
{
    close();
}

void FramelessDialog::tbtnMaxClicked()
{
    if (!m_showFlag) {
        setContentsMargins(0, 0, 0, 0);
        setWindowState(Qt::WindowState::WindowMaximized);
        m_showFlag = true;
    } else {
        setContentsMargins(5, 5, 5, 5);
        setWindowState(Qt::WindowState::WindowNoState);
        showNormal();
        m_showFlag = false;
    }
}

void FramelessDialog::tbtnMinClicked()
{
    showMinimized();
}

void FramelessDialog::setWindowTitle(const QString &title)
{
    ui->labTitle->setText(title);
}

void FramelessDialog::showMessage(const QString &msg)
{
    ui->labStatus->setText(msg);
}

void FramelessDialog::addCornerWidget(QWidget *wgt)
{
    m_cornerLayout->addWidget(wgt);
}

void FramelessDialog::setWindowIcon(const QIcon &icon)
{
    ui->labIcon->setPixmap(icon.pixmap(QSize(16, 16)));
}

void FramelessDialog::addCentralWidget(QWidget *wgt)
{
    m_bodyLayout->addWidget(wgt);
    setSupportStretch(true);
}
