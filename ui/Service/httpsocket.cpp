#include "httpsocket.h"
#include <QDebug>
#include <QRegExp>
#include <QUrlQuery>
#include <QTimer>

HttpSocket::HttpSocket(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    //TODO: remove timeout timer and let the user take care of it and keep this class minimal
    //connect(&m_timeout, SIGNAL(timeout()), this, SLOT(timedOut()));
    //m_timeout.setInterval(10000);
    //m_timeout.start();
}

HttpSocket::~HttpSocket()
{

}


void HttpSocket::sendBody(const QByteArray &body)
{
    this->write("HTTP/1.1 200 OK\n");

    this->write(QStringLiteral("Content-Length: %1\n\n").arg(body.length()).toUtf8());
    this->write(body);

//    if (!m_isKeepAlive)
    //        disconnectFromHost();
}

void HttpSocket::closeConnection()
{
    this->disconnectFromHost();
}

void HttpSocket::onReadyRead()
{
    m_rawReq.append(this->readAll());
    parseReq();
//    if (m_rawReq.length() > 1024)
//        emit httpError(413); //Request Entity Too Large
}

//void HttpSocket::timedOut()
//{
    //emit httpError(408);    //request timed out
    //this->disconnectFromHost();
//}

void HttpSocket::parseReq()
{
    //WARNING: static could cause errors in multi-threaded
    static QRegExp httpRegex(QStringLiteral("^([^\\s]+)\\s([^\\s]+)\\s(HTTP/(1.[01]))\\r?\\n(([^:\\r\\n]+)\\s*:\\s*([^\\r\\n]+)\\r?\\n)+\\r?\\n"), Qt::CaseInsensitive, QRegExp::RegExp2);
    static QRegExp httpRegexNoHeader(QStringLiteral("^([^\\s]+)\\s([^\\s]+)\\s(HTTP/(1.[01]))\\r?\\n\\r?\\n"), Qt::CaseInsensitive, QRegExp::RegExp2);

    bool noHeaders = false;
    if ((noHeaders = httpRegex.indexIn(m_rawReq) < 0) && httpRegexNoHeader.indexIn(m_rawReq) <0 )
        return ;   //http request header signature not found!
    //TODO: validate request for errors: 400

//    QByteArray m_method = noHeaders ?  httpRegexNoHeader.cap(1).toUtf8() : httpRegex.cap(1).toUtf8();
    QByteArray m_url = noHeaders ? httpRegexNoHeader.cap(2).toUtf8() : httpRegex.cap(2).toUtf8();
//    QByteArray m_httpVersion = noHeaders ? httpRegexNoHeader.cap(3).toUtf8() : httpRegex.cap(3).toUtf8();
    int headersLength = noHeaders ?  httpRegexNoHeader.matchedLength() : httpRegex.matchedLength();

    QNetworkRequest req;
    req.setUrl(QUrl::fromEncoded(m_url));
    if (!noHeaders)
    {
        extractHeaders(req, headersLength);
    }

    emit newRequest(req);
    m_rawReq.clear();
    //m_timeout.stop();
    //m_timeout.start();
    return;
}

void HttpSocket::extractHeaders(QNetworkRequest &req, int headersLength)
{
    static QRegExp headerRegex(QStringLiteral("([^:\\r\\n]+)\\s*:\\s*([^\\r\\n]+)"), Qt::CaseInsensitive, QRegExp::RegExp2);
    int index = headerRegex.indexIn(m_rawReq);
    while ( index >= 0 && index < headersLength)
    {
        req.setRawHeader(headerRegex.cap(1).toUtf8(),
                          headerRegex.cap(2).toUtf8());

        index = headerRegex.indexIn(m_rawReq, index + headerRegex.matchedLength());
    }
}
