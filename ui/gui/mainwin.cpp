#include "mainwin.h"
#include <QMouseEvent>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QWebChannel>
#include <QGuiApplication>
#include <QScreen>
#include "jakilidui.h"

MainWin::MainWin(QWidget * parent) : QWebEngineView(parent)
{
    m_jakilidUi = new JakilidUI(this);
    m_network = new QNetworkAccessManager(this);


    // Signal is emitted before frame loads any web content:
    setZoomFactor(zoomFactor());

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("JakilidUI", m_jakilidUi);
    page()->setWebChannel(channel);


    this->resize(1024,800);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint   | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setWindowTitle(QStringLiteral("Jakilid Password Manager"));

    // Load web content now!
    setUrl(QUrl(QStringLiteral("qrc:/UI/index.html")));
}

MainWin::~MainWin()
{
    delete m_network;
    delete m_jakilidUi;
}

qreal MainWin::zoomFactor() const
{
    QRect rect = QGuiApplication::screens().first()->geometry(); // TODO: use screenAt
    if (rect.width() * rect.height() > 1093120) {
        // display is bigger than HDTV
        return 1.25;
    } else {
        return 1.0;
    }
}

void MainWin::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)  //dragging
    {
        if(m_isResizeMode)
        {
            QPoint p = mapToGlobal(event->pos()) - geometry().topLeft();
            resize(p.x(), p.y());
            return;
        }
        if (m_isMovable)
        {
            move(mapToParent(event->pos() - m_offset));
            return;
        }

        QWebEngineView::mouseMoveEvent(event);
        return;
    }

    QPoint diff = geometry().bottomRight() - mapToGlobal(event->pos());
    m_isMovable = event->pos().y() < 40 && event->pos().x() > 100 && diff.x() > 180;
    if (m_isMovable)
    {
        setCursor(Qt::SizeAllCursor);
        return;
    }

    m_isResizeMode = diff.x() <= 6 && diff.y() <= 6;
    if (m_isResizeMode)
    {
        setCursor(Qt::SizeFDiagCursor);
        m_isResizeMode = true;
        return;
    }

    QWebEngineView::mouseMoveEvent(event);
}

void MainWin::mousePressEvent(QMouseEvent *event)
{
    m_offset = event->pos();
    QWebEngineView::mousePressEvent(event);
}

void MainWin::mouseReleaseEvent(QMouseEvent *event)
{
    m_offset = QPoint();
    QWebEngineView::mouseReleaseEvent(event);
}

void MainWin::linkClickedSlot(const QUrl &url)
{
    QDesktopServices::openUrl( url );
}
