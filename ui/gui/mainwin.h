#ifndef MAINWIN_H
#define MAINWIN_H

#include <QWebEngineView>

class JakilidUI;

class MainWin : public QWebEngineView
{
    Q_OBJECT

public:
    explicit MainWin(QWidget * parent = 0);
    ~MainWin();

private:
    QNetworkAccessManager * m_network;
    bool m_isResizeMode;
    bool m_isMovable;
    QPoint m_offset;

    JakilidUI *m_jakilidUi;

    qreal zoomFactor() const ;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void linkClickedSlot(const QUrl &url);

};
#endif
