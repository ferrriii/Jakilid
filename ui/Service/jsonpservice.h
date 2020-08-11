#ifndef JSONPSERVICE_H
#define JSONPSERVICE_H

#include <QTcpServer>
#include <QNetworkRequest>
#include <QStringList>
#include <QPointer>
class ClientManager;
class Client;

class JsonpService : public QTcpServer
{
    Q_OBJECT
public:
    explicit JsonpService(ClientManager *clientManager, QObject *parent = 0);
    ~JsonpService();

signals:

public slots:

private slots:
    void onNewCommand(const QNetworkRequest &req);

protected:
    void incomingConnection(qintptr handle);

private:
    QPointer<ClientManager> m_clientMngrPntr;
    QStringList m_sids;

    QJsonValue performCommand(const QString &cmd, const QJsonValue &data, Client *jakilidClient);

};

#endif // JSONPSERVICE_H
