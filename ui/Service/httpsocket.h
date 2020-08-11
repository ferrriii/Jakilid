#ifndef HTTPSOCKET_H
#define HTTPSOCKET_H

#include <QNetworkRequest>
#include <QTcpSocket>

/*!
 * \brief The HttpSocket class is a simple tcpSocket which implement a minimal HTTP request parser. This class receives and parses
 * HTTP requests and whenever a request is available emits newRequest() signal. on certain HTTP errors (400) this class emits
 * httpError() signal but do not close or sent anything to the client by itself. sendBody() could be used to reply to the client.
 * the closeConnection() slot simply calls QTcpSocket::disconnectFromHost() method.
 */
class HttpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit HttpSocket(QObject *parent = 0);
    virtual ~HttpSocket();
    virtual void sendBody(const QByteArray &body);

signals:
    void newRequest(const QNetworkRequest &req);
    void httpError(int code);

public slots:
    void closeConnection();

private slots:
    virtual void onReadyRead();
//    void timedOut();

protected:
    QByteArray m_rawReq;
    //QTimer m_timeout;

    virtual void parseReq();
    virtual void extractHeaders(QNetworkRequest &req, int headersLength);

    
};

#endif // HTTPSOCKET_H
