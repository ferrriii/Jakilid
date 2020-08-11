#include "clientmanager.h"
#include <application/client.h>

ClientManager::ClientManager(QObject *parent) :
    QObject(parent)
{
}

ClientManager::~ClientManager()
{
    qDeleteAll(m_clientMap);
}

Client *ClientManager::newClient(const QString &id)
{
    QMutexLocker lock(&m_mutex);
    Client *c = m_clientMap.value(id);
    if (!c)
    {
        c = new Client(this);
        m_clientMap.insert(id, c);
        m_clientUsersMap.insert(id, 0);
    }

    int users = m_clientUsersMap.value(id);
    users++;
    m_clientUsersMap.insert(id, users);

    return c;
}

Client *ClientManager::client(const QString &id)
{
    QMutexLocker lock(&m_mutex);
    return m_clientMap.value(id);
}

int ClientManager::deleteClient(const QString &id)
{
    QMutexLocker lock(&m_mutex);
    Client *c = m_clientMap.value(id);
    if (!c)
        return -1;

    int users = m_clientUsersMap.value(id);
    users--;
    if (users <= 0)
    {
        m_clientMap.remove(id);
        m_clientUsersMap.remove(id);
        delete c;
        return 0;
    }else
    {
        m_clientUsersMap.insert(id, users);
        return users;
    }
}

int ClientManager::deleteClient(Client *client)
{
    QString id =QUuid::createUuid().toString();
    m_mutex.lock();
    QString sid = m_clientMap.key(client, id );
    m_mutex.unlock();

    if (id == sid)
        return -1;

    return deleteClient(sid);
}

QString ClientManager::clientId(Client *client)
{
    QMutexLocker lock(&m_mutex);
    return m_clientMap.key(client);
}
