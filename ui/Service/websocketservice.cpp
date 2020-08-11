#include "websocketservice.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QtWebSockets/qwebsocket.h>
#include <application/client.h>
#include <application/profileservice.h>
#include <application/databaseservice.h>
#include "clientmanager.h"

WebSocketService::WebSocketService(ClientManager *clientManager, QObject *parent) :
    QWebSocketServer(QStringLiteral("Jakilid WebSocket Server"), QWebSocketServer::NonSecureMode, parent),
    m_clientMngrPntr(clientManager)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()) );
}

WebSocketService::~WebSocketService()
{
    qDebug() << "bye websock service";
    disconnect();   //signals
    qDeleteAll(m_sidSocketMap);
    if (!m_clientMngrPntr)  //ClientManager could be deleted by its parent destructor before our destructor so check whether it exists
        return;
    QList<QString> activeClients =  m_sidSocketMap.keys();
    foreach (QString sid, activeClients)
    {
        qDebug() << "del" << sid;
        m_clientMngrPntr->deleteClient(sid);
    }
}


void WebSocketService::onNewConnection()
{
    QWebSocket *socket = nextPendingConnection();

    QString sid = socket->requestUrl().path();// QUuid::createUuid().toString();

    Client *jakilidClient = m_clientMngrPntr->newClient(sid);
    connect(jakilidClient, SIGNAL(locked()), this, SLOT(onLocked()), Qt::UniqueConnection);
    connect(jakilidClient, SIGNAL(unLocked()), this, SLOT(onUnlocked()), Qt::UniqueConnection);


//    qDebug() << "new connection" << socket->requestUrl() << sid << jakilidClient;
    connect(socket, SIGNAL(textMessageReceived(QString)) , this, SLOT(processTextMessage(QString)) );
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()) );

    m_sidSocketMap.insert(sid, socket);
}

void WebSocketService::processTextMessage(const QString &message)
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (!socket)
        return;

    QJsonObject msg =  QJsonDocument::fromJson(message.toUtf8()).object();
    if (msg.isEmpty() /*|| there was error parsing message*/)
        return;     //ignore invalid messages

    QString sid = m_sidSocketMap.key(socket);
    Client *jakilidClient =  m_clientMngrPntr->client(sid);
    if (!jakilidClient)
    {
        qWarning("Jakilid client session has been deleted");
        return;
    }

//    qDebug() << "command" << sid << jakilidClient;
    QJsonValue responseData = performCommand(msg.value(QStringLiteral("cmd")).toString(),
                                             msg.value(QStringLiteral("data")),
                                             jakilidClient );
    if (responseData.isUndefined())
        return;     //do not response when there's no return value

    QJsonObject responseObj;
    responseObj.insert(QStringLiteral("responseTo"), msg.value(QStringLiteral("id")));
    responseObj.insert(QStringLiteral("data"), responseData);

    socket->sendTextMessage( QString::fromUtf8(QJsonDocument(responseObj).toJson(QJsonDocument::Compact)) );
}

void WebSocketService::socketDisconnected()
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (!socket)
        return;

    QString sid = m_sidSocketMap.key(socket);// pClient.property("sid").toString();
    m_sidSocketMap.remove(sid, socket);

//    qDebug() << "socket disconnected" << sid << "remaining sockets " << m_sidSocketMap.count(sid);
//    if (m_sidSocketMap.values(sid).count() == 0)
//    {
//        //there's no active websocket with that sid but the jakilidClient maybe still in use by other other
          //services like jsonp so since we do not longer have websocket that uses jakilidClient disconnect
          //its signal
//        Client *jakilidClient = m_clientMngr.client(sid);
//        disconnect(jakilidClient, SIGNAL(locked()), this, SLOT(onLocked()) );
//        disconnect(jakilidClient, SIGNAL(unLocked()), this, SLOT(onUnlocked()) );
//    }

    m_clientMngrPntr->deleteClient(sid);
    socket->deleteLater();
}

void WebSocketService::onLocked()
{
    Client *jakilidClient = static_cast<Client*> (sender());
    QString sid = m_clientMngrPntr->clientId(jakilidClient);

//    qDebug() << "locked" << sid << jakilidClient;
    sendEvent(QStringLiteral("locked"), sid);    //send locked event to all sockets with same session id
}

void WebSocketService::onUnlocked()
{
    Client *jakilidClient = static_cast<Client*> (sender());
    QString sid = m_clientMngrPntr->clientId(jakilidClient);

//    qDebug() << "unlocked" << sid << jakilidClient;
    sendEvent(QStringLiteral("unlocked"), sid);  //send locked event to all sockets with same session id
}

QJsonValue WebSocketService::performCommand(const QString &cmd, const QJsonValue &data, Client *jakilidClient)
{
    qDebug() << "ccc" << cmd;
    if (cmd == QLatin1String("find"))
    {
        QJsonObject dataObj = data.toObject();
        QString count = dataObj.value(QStringLiteral("index")).toVariant().toString();
        return jakilidClient->find(dataObj.value(QStringLiteral("what")).toString(),
                                   dataObj.value(QStringLiteral("label")).toString(),
                                count.isEmpty() ? -1 : count.toInt(),
                                dataObj.value(QStringLiteral("offset")).toVariant().toInt());
    }
    if (cmd == QLatin1String("labels"))
    {
        return jakilidClient->labels();
    }
    else if (cmd == QLatin1String("findByUrl"))
    {
        QJsonObject dataObj = data.toObject();
        QString count = dataObj.value(QStringLiteral("index")).toVariant().toString();
        return jakilidClient->findByUrl(dataObj.value(QStringLiteral("url")).toString(),
                                count.isEmpty() ? -1 : count.toInt(),
                                dataObj.value(QStringLiteral("offset")).toVariant().toInt());
    }
    else if (cmd == QLatin1String("load"))
    {
        return jakilidClient->load(data.toString());
    }
    else if (cmd == QLatin1String("authenticate"))
    {
        QJsonObject dataObj = data.toObject();
        return jakilidClient->authenticate(dataObj.value(QStringLiteral("profile")).toString(),
                                                 dataObj.value(QStringLiteral("pass")).toString());
    }
    else if (cmd == QLatin1String("isUnlocked"))
    {
        return jakilidClient->isUnlocked();
    }
    else if (cmd == QLatin1String("upgradeDb"))
    {
        return jakilidClient->upgradeDb();
    }
    else if (cmd == QLatin1String("getCurrentProfile"))
    {
        return jakilidClient->getCurrentProfile();
    }
    else if (cmd == QLatin1String("lock"))
    {
        jakilidClient->lock();
    }
    else if (cmd == QLatin1String("save"))
    {
        return jakilidClient->save(data);
    }
    else if (cmd == QLatin1String("remove"))
    {
        return jakilidClient->remove(data.toString());
    }
    else if (cmd == QLatin1String("setSettings"))
    {
        QJsonObject dataObj = data.toObject();
        return jakilidClient->setSettings(dataObj.value(QStringLiteral("timeout")).toVariant().toInt(),
                                            dataObj.value(QStringLiteral("lang")).toString());
    }
    else if (cmd == QLatin1String("changePass"))
    {
        QJsonObject dataObj = data.toObject();
        return jakilidClient->changePass(dataObj.value(QStringLiteral("curPass")).toString(),
                                            dataObj.value(QStringLiteral("newPass")).toString());
    }
    else if (cmd == QLatin1String("getAvailableProfiles"))
    {
        ProfileService profileService;
        return QJsonArray::fromStringList(profileService.allProfileNames());
    }
    else if (cmd == QLatin1String("isPassStrong"))
    {
        QString pass =data.toString();
        if (pass.length() < 8)
            return tr("Short passwords are easy to guess. Try one with at least 8 characters. How about a sentence?");

        static QStringList weakPasswords;
        if (weakPasswords.isEmpty())    //open passwor file once
        {
            QFile file(QStringLiteral("://common-passwords.txt"));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return QString();

            QTextStream in(&file);
            while (!in.atEnd())
                weakPasswords << in.readLine();
        }
        if (weakPasswords.contains(pass))
            return tr("This password would get cracked in less than a second! Please choose a different one.");

        return QString();
    }
    else if (cmd == QLatin1String("createProfile"))
    {
        QJsonObject dataObj = data.toObject();
        DatabaseService dbSrv;
        QString db = dataObj.value(QStringLiteral("db")).toString();
        QString pass = dataObj.value(QStringLiteral("pass")).toString();

        if (dataObj.value(QStringLiteral("isNew")).toVariant().toBool() && !dbSrv.createDatabase(db, pass))
            return QFile::remove(db) && false;  //rollback: delete just created new db file (if created)
        Profile profile(dataObj.value(QStringLiteral("name")).toString(), db);
        profile.setLang(dataObj.value(QStringLiteral("lang")).toString());
        ProfileService profileService;
        return profileService.addProfile(&profile, pass);
    }
    else if (cmd == QLatin1String("about"))
    {
        QJsonObject aboutObj;
        aboutObj.insert("version", QString::fromLocal8Bit(JAKILID_VERSION));
        QDate buildDate = QLocale("en_US").toDate(QString(__DATE__).simplified(), "MMM d yyyy");
        QString isoDate = QString::number(buildDate.year()) + "-" +
                          QString::number(buildDate.month()).rightJustified(2,'0') + "-" +
                          QString::number(buildDate.day()).rightJustified(2,'0');
        aboutObj.insert("buildDate", isoDate );
        return aboutObj;
    }
    else if (cmd == QLatin1String("ping"))
    {
        jakilidClient->preventLock();
    }

    return QJsonValue(QJsonValue::Undefined);   //invalid cmd
}

void WebSocketService::sendEvent(const QString &event, const QString &clientId)
{
    QJsonObject responseObj;
    responseObj.insert(QStringLiteral("event"), event);
    QString msgStr = QString::fromUtf8(QJsonDocument(responseObj).toJson(QJsonDocument::Compact));

    QList<QWebSocket*> sockets = m_sidSocketMap.values(clientId);
//    qDebug() << "send event to " << sockets.count() << "sid:" << clientId;
    foreach (QWebSocket *client, sockets)
        client->sendTextMessage( msgStr );
}
