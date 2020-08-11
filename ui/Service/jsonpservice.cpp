#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QFile>
#include <application/client.h>
#include <application/profileservice.h>
#include <application/databaseservice.h>
#include "httpsocket.h"
#include "jsonpservice.h"
#include "clientmanager.h"

JsonpService::JsonpService(ClientManager *clientManager, QObject *parent) :
    QTcpServer(parent),
    m_clientMngrPntr(clientManager)
{
}

JsonpService::~JsonpService()
{
    if (!m_clientMngrPntr)  //NOTE: since do not take ownership of ClientManager, we used QPointer so whenever it was deleted (by its parent at destructor) we won't crash
        return;

    foreach(QString sid, m_sids)
        m_clientMngrPntr->deleteClient(sid);
}

void JsonpService::onNewCommand(const QNetworkRequest &req)
{
    HttpSocket *socket = qobject_cast<HttpSocket *>(sender());
    if (!socket)
        return;

    QString sid = req.url().path();

    Client *jakilidClient =  0;
    if (m_sids.contains(sid))
    {
        //we already created this sid just use it
        jakilidClient =  m_clientMngrPntr->client(sid);
        if (!jakilidClient) //just error handling, this should never happen
        {
            qWarning("Jakilid client session has been deleted");
            return;
        }
    }
    else
    {
        //we hadn't this sid before so let the clientManager knows we are using this sid (others maybe using too)
        jakilidClient = m_clientMngrPntr->newClient(sid);
        m_sids.append(sid);
        //TODO: call m_clientMngr->deleteClient(sid) some when
    }

    //NOTE: if it wasn't a disconnected connection we could connect jakilidclient signals
    //such as locked to inform user

    QUrlQuery queries(req.url());


    //NOTE: QJsonDocument is bullshit is doesn't parse values
    QJsonParseError err;
    QJsonValue data = QJsonDocument::fromJson(queries.queryItemValue("data", QUrl::FullyDecoded).replace('+', ' ').replace("%2B","+", Qt::CaseInsensitive).toUtf8(), &err).object();
    if (err.error != QJsonParseError::NoError)
    {
        QString d= queries.queryItemValue("data", QUrl::FullyDecoded).replace('+', ' ').replace("%2B","+", Qt::CaseInsensitive);
        data = d.mid(1, d.length()-2);
    }

    QJsonValue responseData = performCommand(queries.queryItemValue("cmd", QUrl::FullyDecoded).replace('+', ' ').replace("%2B","+", Qt::CaseInsensitive),
                                             data,
                                             jakilidClient );

    //NOTE: QJsonDocument is bullshit! it doesn't have constructor with QJsonValue argument
    QByteArray j;
    if (responseData.isObject())
        j = QJsonDocument(responseData.toObject()).toJson(QJsonDocument::Compact);
    else if (responseData.isArray())
        j = QJsonDocument(responseData.toArray()).toJson(QJsonDocument::Compact);
    else if (responseData.isString())
        j = "\"" + responseData.toVariant().toString().toUtf8() + "\"";
    else
        j = responseData.toVariant().toString().toUtf8();

    socket->sendBody( queries.queryItemValue("callback", QUrl::FullyDecoded).replace('+', ' ').replace("%2B","+", Qt::CaseInsensitive).toUtf8() + "(" + j + ");" );

    //TODO: disconnect or keep-alive ?!
}


void JsonpService::incomingConnection(qintptr handle)
{
    HttpSocket *socket = new HttpSocket(this);

//! [1] //! [2]
    if (!socket->setSocketDescriptor(handle))
    {
        //emit error(tcpSocket->error());
        delete socket;
        return;
    }

    //TODO: disconet socket on inactivity timeout
    //FIX: this could be done be wrting a wrapper class for HttpSocket that handles such errors
    connect(socket, SIGNAL(newRequest(QNetworkRequest)), this, SLOT(onNewCommand(QNetworkRequest)));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    //TODO: not a bad idea to keep sockets in a list to deletelater

//    qDebug() << "new connection" << socket;
}

QJsonValue JsonpService::performCommand(const QString &cmd, const QJsonValue &data, Client *jakilidClient)
{
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
