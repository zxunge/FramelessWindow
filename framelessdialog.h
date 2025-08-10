#ifndef FRAMELESSDIALOG_H
#define FRAMELESSDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QPointF>
#include <QIcon>
#include <QRect>

namespace Ui {
class FramelessDialog;
}
class QHBoxLayout;

class FramelessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FramelessDialog(QWidget *parent = nullptr);
    ~FramelessDialog();

    void setSupportStretch(bool isSupportStretch);

    void setWindowTitle(const QString &title);
    void showMessage(const QString &msg);
    void setMinimumHeight(int height)
    {
        m_windowMinHeight = height;
        QDialog::setMinimumHeight(height);
    }
    void setMinimumWidth(int width)
    {
        m_windowMinWidth = width;
        QDialog::setMinimumWidth(width);
    }

    void addCornerWidget(QWidget *wgt);
    void setWindowIcon(const QIcon &icon);

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
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void tbtnCloseClicked();
    void tbtnMaxClicked();
    void tbtnMinClicked();

private:
    Ui::FramelessDialog *ui;

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

    int m_windowMinWidth;
    int m_windowMinHeight;

private:
    static constexpr int STRETCH_RECT_HEIGHT = 10;
    static constexpr int STRETCH_RECT_WIDTH = 10;
};

#endif // FRAMELESSDIALOG_H
