#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <application/databaseservice.h>
#include <application/profileservice.h>
#include <domain/account.h>
#include <../../ui/Service/websocketservice.h>
#include <../../ui/Service/clientmanager.h>
#include <QWebSocket>

class WebSocketServerTest : public QObject
{
    Q_OBJECT

public:
    WebSocketServerTest();
    ~WebSocketServerTest();

private Q_SLOTS:
    void initTestCase();
    void testCase1();
    void lockUnlockTest();
    void multipleClients();
    void saveLoadRemoveTest();
    void findTest();
    void settingsTest();
    void cleanupTestCase();
private:
    WebSocketService *m_server;
    QWebSocket m_client;
    bool m_received;
    QString m_msg;
    int m_unlockedSignals;
    int m_lockedSignals;
    int m_id;

    int send(const QString &cmd, const QJsonValue &data = QJsonValue());
    void waitForResponse();
    void makeSureThereIsNoResponse();
private slots:
    void clientConnected();
    void onTextMessageReceived(QString txt);
    void onError(QAbstractSocket::SocketError err);
};


WebSocketServerTest::WebSocketServerTest(): m_received(false), m_unlockedSignals(0),m_lockedSignals(0), m_id(0)
{
}

WebSocketServerTest::~WebSocketServerTest()
{
}

void WebSocketServerTest::initTestCase()
{
    //required by profile repository
    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain("unittest");
    QCoreApplication::setApplicationName("WebSocketServerTest");

    cleanupTestCase();

    /*
    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./WebSocketServerTest.db", "123456"));
    ProfileService profileService;
    Profile profile("WebSocketServerTest", "./WebSocketServerTest.db");
    QVERIFY(profileService.addProfile(&profile, "123456"));
    */

    connect(&m_client, SIGNAL(connected()), this, SLOT(clientConnected()));
    connect(&m_client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientConnected()));
    connect(&m_client, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
    ClientManager *clientMngr = new ClientManager(this);
    m_server = new WebSocketService(clientMngr, this);
    m_server->listen(QHostAddress::LocalHost, 55885);
    QVERIFY(m_server->isListening());


    m_client.open(QUrl("ws://localhost:55885/"));
    QElapsedTimer timeout;
    timeout.start();
    while (m_client.state() != QAbstractSocket::ConnectedState && timeout.elapsed() < 3000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 3000, "Connection refused!");


    int id;
    QJsonObject args;
    QJsonObject resObj;

    //try unlocking with wrong pass
    args.insert("db", "./WebSocketServerTest.db");
    args.insert("pass", "123456");
    args.insert("isNew", true);
    args.insert("name", "WebSocketServerTest");
    args.insert("lang", "en-US");
    id = send("createProfile", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toBool(), true);


}

void WebSocketServerTest::testCase1()
{

    //check state
    int id = send( "isUnlocked");
    waitForResponse();
    QJsonObject resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toVariant().toBool(), false);



    m_received =false;
    QVERIFY(m_client.sendTextMessage("hi"));
    makeSureThereIsNoResponse();


}

void WebSocketServerTest::lockUnlockTest()
{

    int id;
    QJsonObject args;
    QElapsedTimer timeout;
    QJsonObject resObj;

    //try unlocking with wrong pass
    args.insert("profile", "WebSocketServerTest");
    args.insert("pass", "wrong pass");

    id = send( "authenticate", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toBool(), false);


    //unlock with correct pass
    args.insert("pass", "123456");
    id = send( "authenticate", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toVariant().toBool(), true);


    //check if unlocked event received
    timeout.restart();
    while (m_unlockedSignals==0 && timeout.elapsed() < 3000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 3000, "no unlocked signal!");
    QCOMPARE(m_unlockedSignals, 1);
    QCOMPARE(m_lockedSignals, 0);


    //check if session is unlocked
    id = send( "isUnlocked");
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toVariant().toBool(), true);


    //lock
    id = send( "lock");
    makeSureThereIsNoResponse();


    //check if unlocked event received
    timeout.restart();
    while (m_lockedSignals==0 && timeout.elapsed() < 3000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 3000, "no locked signal!");
    QCOMPARE(m_unlockedSignals, 1);
    QCOMPARE(m_lockedSignals, 1);


    //check if session is locked
    id = send( "isUnlocked");
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toVariant().toBool(), false);


    //unlock with correct pass
    args.insert("pass", "123456");
    id = send( "authenticate", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toVariant().toBool(), true);

}

void WebSocketServerTest::multipleClients()
{
    int id;
    QJsonObject args;
    QJsonObject resObj;

    //try unlocking with wrong pass
    args.insert("profile", "WebSocketServerTest");
    args.insert("pass", "123456");

    id = send( "authenticate", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), id);
    QCOMPARE(resObj.value("data").toBool(), true);



    QElapsedTimer timeout;
    QWebSocket client2;
    bool cl2Connected= false;
    bool cl2Received = false;
    QString cl2Msg;
    connect(&client2, &QWebSocket::connected,  [&]() {
       cl2Connected = true;
    });
//    connect(&client2, SIGNAL(error(QAbstractSocket::SocketError)),  [=](QAbstractSocket::SocketError err) {
//        qDebug() << "client2 error occured!";
//    });
    connect(&client2, &QWebSocket::textMessageReceived,  [&](const QString &msg) {
        cl2Msg = msg;
        cl2Received = true;
    });

    client2.open(QUrl("ws://localhost:55885/"));
    timeout.restart();
    while (!cl2Connected && timeout.elapsed() < 3000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 3000, "client 2 failed to connect!");


    QJsonObject obj;
    obj.insert("id", 1);
    obj.insert("cmd", "isUnlocked");
    obj.insert("data", "" );
    client2.sendTextMessage( QString::fromUtf8(QJsonDocument(obj).toJson()) );

    timeout.restart();
    while (!cl2Received && timeout.elapsed() < 3000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 3000, "no response for client 2!");


    resObj = QJsonDocument::fromJson(cl2Msg.toUtf8()).object();
    QCOMPARE(resObj.value("responseTo").toVariant().toInt(), 1);
    QCOMPARE(resObj.value("data").toVariant().toBool(), true);

}


void WebSocketServerTest::saveLoadRemoveTest()
{
    QJsonObject resObj;

    Account *acc = new Account(this);
    acc->setTitle("test account");
    acc->addInfo("key", "value");

    send("save", acc->toJson());
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QVERIFY(resObj.value("data").toString() == acc->getId().toString() );


    send("load", "invalid-id");
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QJsonValue loadedAcc= resObj.value("data");
    QVERIFY(loadedAcc.toObject().isEmpty());


    send("load", acc->getId().toString());
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QJsonValue savedAcc= resObj.value("data");
    QVERIFY(savedAcc == acc->toJson());


    send("remove", acc->getId().toString());
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("data").toBool(), true);


    send("load", acc->getId().toString());
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    savedAcc= resObj.value("data");
    QVERIFY(savedAcc.toObject().isEmpty());

    delete acc;


}

void WebSocketServerTest::findTest()
{
    QJsonObject args;
    QJsonObject resObj;
    QJsonObject foundAccJson;

    //find
    send("find", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    foundAccJson= resObj.value("data").toObject();
    QCOMPARE(foundAccJson.value("total").toVariant().toInt(), 0);

    //save account
    Account acc;
    acc.setTitle("test account");
    acc.addInfo("key", "value");
    acc.addLabel("tag");
    acc.addLabel("label");

    send("save", acc.toJson());
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QVERIFY(resObj.value("data").toString() == acc.getId().toString() );


    //find account
    args.empty();
    args.insert("what", "");
    //args.insert("index", 0);
    //args.insert("offset", 10);
    send("find", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    foundAccJson= resObj.value("data").toObject();
    QCOMPARE(foundAccJson.value("total").toVariant().toInt(), 1);
    QCOMPARE(foundAccJson.value("items").toArray().count(), 1);
    QVERIFY(foundAccJson.value("items").toArray().at(0).toObject().value("title").toString() == QString("test account"));
    QVERIFY(foundAccJson.value("items").toArray().at(0).toObject().value("labels").toVariant().toStringList().count() == 2);
    QVERIFY(foundAccJson.value("items").toArray().at(0).toObject().value("labels").toVariant().toStringList().contains("tag"));
    QVERIFY(foundAccJson.value("items").toArray().at(0).toObject().value("labels").toVariant().toStringList().contains("label"));
    QVERIFY(foundAccJson.value("items").toArray().at(0).toObject().value("id").toString() == acc.getId().toString());



    args.empty();
    args.insert("what", "");
    args.insert("label", "non-existing");
    //args.insert("index", 0);
    //args.insert("offset", 10);
    send("find", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    foundAccJson= resObj.value("data").toObject();
    QCOMPARE(foundAccJson.value("total").toVariant().toInt(), 0);
    QCOMPARE(foundAccJson.value("items").toArray().count(), 0);


    send("labels");
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QJsonArray labels= resObj.value("data").toArray();
    QCOMPARE(labels.toVariantList().count(), 2);
    QVERIFY(labels.toVariantList().contains("tag"));
    QVERIFY(labels.toVariantList().contains("label"));

}




void WebSocketServerTest::settingsTest()
{
    QJsonObject args;
    QJsonObject resObj;
    QJsonObject foundAccJson;

    //setSettings
    args.insert("timeout", 60);
    args.insert("lang", "fa-IR");
    send("setSettings", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QCOMPARE(resObj.value("data").toBool(), true);


    //getSettings
    args.empty();
    send("getCurrentProfile", args);
    waitForResponse();
    resObj = QJsonDocument::fromJson(m_msg.toUtf8()).object();
    QJsonObject profile = resObj.value("data").toObject();
    QCOMPARE(profile.value("name").toString(), QString("WebSocketServerTest"));
    QCOMPARE(profile.value("db").toString(),QString("./WebSocketServerTest.db"));
    QCOMPARE(profile.value("timeout").toInt(), 60);
    QCOMPARE(profile.value("lang").toString(), QString("fa-IR"));

}


void WebSocketServerTest::cleanupTestCase()
{
    send("lock");
    ProfileService profileService;
    profileService.removeProfile("WebSocketServerTest");
    QFile::remove("./WebSocketServerTest.db");

}

int WebSocketServerTest::send(const QString &cmd, const QJsonValue &data)
{
    QJsonObject obj;

    ++m_id;
    obj.insert("id", m_id);
    obj.insert("cmd", cmd);
    obj.insert("data", data );

    m_received =false;
    m_client.sendTextMessage( QString::fromUtf8(QJsonDocument(obj).toJson()) );

    return m_id;
}

void WebSocketServerTest::waitForResponse()
{
    QElapsedTimer timeout;
    timeout.restart();
    while (!m_received && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "no response!");
}

void WebSocketServerTest::makeSureThereIsNoResponse()
{
    QElapsedTimer timeout;
    timeout.restart();
    while (!m_received && timeout.elapsed() < 3000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() >= 3000, "unexpected response received!");
    QVERIFY2(!m_received, "unexpected response received!");
}

void WebSocketServerTest::clientConnected()
{
    //m_client.sendTextMessage("{id: 1, data:'salam'}");
}

void WebSocketServerTest::onTextMessageReceived(QString txt)
{
    QJsonObject resObj = QJsonDocument::fromJson(txt.toUtf8()).object();
    if (resObj.value("event").toString().isEmpty())
    {
        m_msg= txt;
        m_received = true;
    }
    m_unlockedSignals +=  resObj.value("event").toString() == "unlocked" ? 1 : 0;
    m_lockedSignals +=  resObj.value("event").toString() == "locked" ? 1 : 0;

}

void WebSocketServerTest::onError(QAbstractSocket::SocketError err)
{
    QFAIL(QString("Socket error: %1").arg(err).toStdString().data() );
}


QTEST_MAIN(WebSocketServerTest)

#include "tst_websocketservertest.moc"

