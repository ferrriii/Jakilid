#ifndef WEBSOCKETSERVICE_H
#define WEBSOCKETSERVICE_H

#include <QObject>
#include <QMultiMap>
#include <QPointer>
#include <QtWebSockets/qwebsocketserver.h>
class QWebSocket;
class Client;
class ClientManager;

class WebSocketService : public QWebSocketServer
{
    Q_OBJECT
public:
    explicit WebSocketService(ClientManager *clientManager, QObject *parent=0);
    ~WebSocketService();
    
signals:
    
public slots:

private slots:
    void onNewConnection();
    void processTextMessage(const QString &message);
    void socketDisconnected();

    void onLocked();
    void onUnlocked();

private:
    QPointer<ClientManager> m_clientMngrPntr;
    ClientManager *m_clientMngr;
    QMultiMap<QString, QWebSocket*> m_sidSocketMap;


    QJsonValue performCommand(const QString &cmd, const QJsonValue &data, Client *jakilidClient);
    void sendEvent(const QString &event, const QString &clientId);
    
};

#endif // WEBSOCKETSERVICE_H
