#ifndef WINFRAMELESSMAINWINDOW_H
#define WINFRAMELESSMAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include <QPoint>
#include <QPointF>
#include <QIcon>

namespace Ui {
class WinFramelessMainWindow;
}
class QHBoxLayout;

class WinFramelessMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    WinFramelessMainWindow(QWidget *parent = nullptr);
    ~WinFramelessMainWindow();

    void setWindowTitle(const QString &title);
    void showMessage(const QString &msg);
    void setMinimumHeight(int height)
    {
        m_windowMinHeight = height;
        QMainWindow::setMinimumHeight(height);
    }
    void setMinimumWidth(int width)
    {
        m_windowMinWidth = width;
        QMainWindow::setMinimumWidth(width);
    }

    void addCornerWidget(QWidget *wgt);
    void setWindowIcon(const QIcon &icon);

    void addCentralWidget(QWidget *wgt);

protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void tbtnCloseClicked();
    void tbtnMaxClicked();
    void tbtnMinClicked();

private:
    Ui::WinFramelessMainWindow *ui;

    QHBoxLayout *m_bodyLayout;
    QHBoxLayout *m_cornerLayout;

    bool m_showFlag;

    int m_windowMinWidth;
    int m_windowMinHeight;
};

#endif // WINFRAMELESSMAINWINDOW_H
