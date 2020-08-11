#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>
class Client;

class ClientManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientManager(QObject *parent = 0);
    ~ClientManager();
    Client  *newClient(const QString &id);
    Client  *client(const QString &id);
    int     deleteClient(const QString &id);
    int     deleteClient(Client *client);
    QString clientId(Client *client);
    
signals:
    
public slots:


private:
    QMap<QString, Client*> m_clientMap;
    QMap<QString, int>     m_clientUsersMap;
    QMutex                 m_mutex;
    
};

#endif // CLIENTMANAGER_H

