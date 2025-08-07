#ifndef FRAMELESSMAINWINDOW_H
#define FRAMELESSMAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include <QPoint>
#include <QPointF>
#include <QIcon>

namespace Ui {
class FramelessMainWindow;
}
class QHBoxLayout;

class FramelessMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FramelessMainWindow(QWidget *parent = nullptr);
    ~FramelessMainWindow();

    void setSupportStretch(bool isSupportStretch);

    void setWindowTitle(const QString &title);
    void showMessage(const QString &msg);
    void setMinimumHeight(int height) { m_windowMinHeight = height; }
    void setMinimumWidth(int width) { m_windowMinWidth = width; }

    void addCornerWidget(QWidget *wgt);
    void setIcon(const QIcon &icon);

    void addCentralWidget(QWidget *wgt);

private:
    enum WindowStretchRectState {
        NO_SELECT = 0,
        LEFT_TOP_RECT,
        TOP_BORDER,
        RIGHT_TOP_RECT,
        RIGHT_BORDER,
        RIGHT_BOTTOM_RECT,
        BOTTOM_BORDER,
        LEFT_BOTTOM_RECT,
        LEFT_BORDER
    };
    Q_ENUM(WindowStretchRectState)

    void calculateCurrentStrechRect();
    WindowStretchRectState getCurrentStretchState(QPoint cursorPos);
    void updateMouseStyle(WindowStretchRectState stretchState);
    void updateWindowSize();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void tbtnCloseClicked();
    void tbtnMaxClicked();
    void tbtnMinClicked();

private:
    Ui::FramelessMainWindow *ui;

    QHBoxLayout *m_bodyLayout;
    QHBoxLayout *m_cornerLayout;

    QRect m_leftTopRect;
    QRect m_leftBottomRect;
    QRect m_rightTopRect;
    QRect m_rightBottomRect;
    QRect m_topBorderRect;
    QRect m_rightBorderRect;
    QRect m_bottomBorderRect;
    QRect m_leftBorderRect;

    QPointF m_last;
    QPoint m_startPoint;
    QPoint m_endPoint;
    bool m_showFlag;
    bool m_isPressedWidget;

    WindowStretchRectState m_stretchRectState;
    QRect m_windowRectBeforeStretch;

    bool m_isMousePressed;
    bool m_isSupportStretch;
    bool m_isMaxShow;

    int m_windowMinWidth;
    int m_windowMinHeight;

private:
    static constexpr int STRETCH_RECT_HEIGHT = 10;
    static constexpr int STRETCH_RECT_WIDTH = 10;
};

#endif // FRAMELESSMAINWINDOW_H
