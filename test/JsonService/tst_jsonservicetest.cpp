#include <QString>
#include <QtTest>
#include <application/databaseservice.h>
#include <application/profileservice.h>
#include <domain/account.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../../ui/Service/jsonpservice.h"
#include "../../ui/Service/clientmanager.h"


class JsonServiceTest : public QObject
{
    Q_OBJECT
    
public:
    JsonServiceTest();
    ~JsonServiceTest();
    
private Q_SLOTS:
    void initTests();
    void lockUnlockTest();
    void sessionTest();
    void saveLoadRemoveTest();
    void labelsTest();
    void findTest();
    void settingsTest();
    void passCheckTest();

    void cleanupTestCase();

private:
    JsonpService *m_server;
    QNetworkAccessManager *m_network;

    void saveAcc(const Account *acc, const QString &sid);
    Account *loadAcc(const QString &id, const QString &sid);
    bool removeAcc(const QString &id, const QString &sid);
    void lock(const QString &sid);
    bool unLock(const QString &sid);
    bool isUnlocked(const QString &sid, bool expected = true);
    QJsonObject find(const QString &what, const QString &label, const QString &sid, int index = -1, int offset=0, bool findByUrl = false);
};

JsonServiceTest::JsonServiceTest()
{
}

JsonServiceTest::~JsonServiceTest()
{
}

void JsonServiceTest::initTests()
{
    //required by profile repository
    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain("unittest");
    QCoreApplication::setApplicationName("JsonServiceTest");

    m_network = new QNetworkAccessManager();

    cleanupTestCase();

    /*
    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./JsonServiceTest.db", "123456"));
    ProfileService profileService;
    Profile profile("JsonServiceTest", "./JsonServiceTest.db");
    QVERIFY(profileService.addProfile(&profile, "123456"));
    */

    ClientManager *clientMngr = new ClientManager(this);
    m_server = new JsonpService(clientMngr, this);
    QVERIFY(m_server->listen(QHostAddress::LocalHost, 6060));


    //create profile
    QUrlQuery queries;
    QJsonObject data;

    data.insert("db",  "./JsonServiceTest.db");
    data.insert("pass", "123456");
    data.insert("isNew", true);
    data.insert("name", "JsonServiceTest");
    data.insert("lang", "en-US");
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "createProfile");
    queries.addQueryItem("data",  QJsonDocument(data).toJson(QJsonDocument::Compact) );
    QUrl url("http://localhost:6060/");
    url.setQuery(queries);
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QByteArray dt = reply->readAll();
        QCOMPARE(dt, QByteArray("abc(true);"));
        reply->deleteLater();
    });

    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "Request time out!");


}

void JsonServiceTest::lockUnlockTest()
{
    //check unlock
    QNetworkReply *reply;
    QJsonObject data;
    QUrlQuery queries;

    isUnlocked("sid1", false);


    //unlock with wrong pass
    data.insert("profile", "JsonServiceTest");
    data.insert("pass", "wrong_pass");
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "authenticate");
    queries.addQueryItem("data",  QJsonDocument(data).toJson(QJsonDocument::Compact) );
    QUrl url("http://localhost:6060/sid1/");
    url.setQuery(queries);
    reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QCOMPARE(reply->readAll(), QByteArray("abc(false);"));
        reply->deleteLater();
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "Request time out!");


    unLock("sid1");

    isUnlocked("sid1");


    lock("sid1");


    isUnlocked("sid1", false);
}

void JsonServiceTest::sessionTest()
{
    isUnlocked("session1", false);

    unLock("session1");

    isUnlocked("session1");

    isUnlocked("session2", false);

    unLock("session2");

    isUnlocked("session2");

    lock("session1");

    isUnlocked("session1", false);

    isUnlocked("session2");

    lock("session2");

    isUnlocked("session2", false);
}


void JsonServiceTest::saveLoadRemoveTest()
{
    unLock("session3");


    Account testAcc;
    testAcc.setTitle("test account");
    testAcc.addInfo("key1", "value1");
    testAcc.addInfo("key2", "value2");


    saveAcc(&testAcc, "session3");

    Account *loadedAcc = loadAcc(testAcc.getId().toString(), "session3");
    QCOMPARE(loadedAcc->toJson() , testAcc.toJson());
    delete loadedAcc;

    removeAcc(testAcc.getId().toString(), "session3");

    loadedAcc = loadAcc(testAcc.getId().toString(), "session3");
    QVERIFY(!loadedAcc->isValid());
    QVERIFY(loadedAcc->toJson() != testAcc.toJson());
    delete loadedAcc;
}

void JsonServiceTest::labelsTest()
{

    QString sid = "session10";
    unLock(sid);

    Account acc1;
    acc1.setTitle("acc1");
    acc1.addLabel("label");
    saveAcc(&acc1, sid);

    Account acc2;
    acc2.setTitle("acc2");
    acc2.addLabel("label");
    acc2.addLabel("tag");
    saveAcc(&acc2, sid);


    QUrlQuery queries;

    queries.addQueryItem("cmd", "labels");
    queries.addQueryItem("callback", "abc");
    QUrl url("http://localhost:6060/" + sid + "/");
    url.setQuery(queries);
    QByteArray res;
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [&]() {
        res.append(reply->readAll());
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QTest::qVerify(timeout.elapsed() < 10000, "timeout.elapsed() < 10000", "Request time out!",__FILE__, __LINE__);

    res = res.mid(4, res.length() - 4 -2);

    QVariantList labels =  QJsonDocument::fromJson(res).array().toVariantList();
    QCOMPARE(labels.count(), 2);
    QVERIFY(labels.contains("tag"));
    QVERIFY(labels.contains("label"));

    removeAcc(acc1.getId().toString(), sid);
    removeAcc(acc2.getId().toString(), sid);
    reply->deleteLater();

}

void JsonServiceTest::findTest()
{
    QJsonObject results;

    unLock("session4");

    results = find("","", "session4");

    QVERIFY(results.contains("total"));
    QCOMPARE(results.value("total").toVariant().toInt(), 0);

    Account *acc1 = new Account();
    acc1->setId(QUuid("{11111111-3a0d-4a1a-8732-1feadce7da11}"));
    acc1->setTitle("test account1");
    acc1->addLabel("lbl1");
    acc1->addInfo("key1", "value1");
    acc1->addInfo("key2", "value2");

    Account *acc2 = new Account();
    acc2->setId(QUuid("{22222222-3a0d-4a1a-8732-1feadce7da22}"));
    acc2->setTitle("test account2");
    acc2->addInfo("k", "1");
    acc2->addInfo("a", "b");
    Info *i1 = new Info(acc2);
    i1->setKey("login url");
    i1->setValue("http://mail.yahoo.com/as/?lo=2");
    i1->setType(Info::KeyTypeURL);
    i1->setFormInput("txtUrl");
    acc2->addInfo(i1);


    saveAcc(acc1, "session4");
    saveAcc(acc2, "session4");

    results = find("","", "session4");
    QCOMPARE(results.value("total").toVariant().toInt(), 2);
    QCOMPARE(results.value("items").toArray().count(), 2);
    Account *loadedAcc1 = new Account(), *loadedAcc2 = new Account();
    QVERIFY(loadedAcc1->fromJson(results.value("items").toArray().at(0).toObject()));
    QVERIFY(loadedAcc2->fromJson(results.value("items").toArray().at(1).toObject()));
    //qDebug() << QJsonDocument(acc2->toJson().toObject()).toJson() << QJsonDocument(acc1->toJson().toObject()).toJson() << QJsonDocument(loadedAcc2->toJson().toObject()).toJson() << QJsonDocument(loadedAcc1->toJson().toObject()).toJson() << (acc2->toJson() == loadedAcc2->toJson()) << (acc1->toJson() == loadedAcc1->toJson()) << (acc2->toJson() == loadedAcc1->toJson()) << (acc1->toJson() == loadedAcc2->toJson());
    //BUG: loaded accounts order is unpredictable!
    QVERIFY((acc2->toJson() == loadedAcc2->toJson() && acc1->toJson() == loadedAcc1->toJson()) ||
            (acc2->toJson() == loadedAcc1->toJson() && acc1->toJson() == loadedAcc2->toJson()) );
//    QCOMPARE(acc2->toJson(), loadedAcc2->toJson());
//    QCOMPARE(acc1->toJson(), loadedAcc1->toJson());

//    QVERIFY(results.value("items").toArray().at(0).toObject().value("title").toString() == acc1.getTitle());
//    QVERIFY(results.value("items").toArray().at(0).toObject().value("id").toString() == acc1.getId().toString());
//    QVERIFY(results.value("items").toArray().at(1).toObject().value("title").toString() == acc21.getTitle());
//    QVERIFY(results.value("items").toArray().at(1).toObject().value("id").toString() == acc2.getId().toString());

    results = find("","", "session4", -1, 1);
    Account *loadedAcc5 = new Account();
    QCOMPARE(results.value("total").toVariant().toInt(), 2);
    QCOMPARE(results.value("items").toArray().count(), 1);
    QVERIFY(loadedAcc5->fromJson(results.value("items").toArray().at(0).toObject()));
    QVERIFY((acc1->toJson() == loadedAcc5->toJson()) ||
            (acc2->toJson() == loadedAcc5->toJson()) );
//    QCOMPARE(acc2->toJson(), loadedAcc2->toJson());

    results = find("","", "session4", 1, 1);
    QCOMPARE(results.value("total").toVariant().toInt(), 2);
    QCOMPARE(results.value("items").toArray().count(), 1);
    Account *loadedAcc3 = new Account();

    QVERIFY(loadedAcc3->fromJson(results.value("items").toArray().at(0).toObject()));
//    qDebug() << "loaaaaaaad" << QJsonDocument(loadedAcc2.toJson().toObject()).toJson() << QJsonDocument(acc2.toJson().toObject()).toJson();
//    QCOMPARE(acc2->toJson(), loadedAcc3->toJson());
    QVERIFY((acc1->toJson() == loadedAcc3->toJson()) ||
            (acc2->toJson() == loadedAcc3->toJson()) );

    Account *loadedAcc4 = new Account();
    results = find("http://mail.yahoo.com/","", "session4", -1, 0, true);
    QCOMPARE(results.value("total").toVariant().toInt(), 1);
    QCOMPARE(results.value("items").toArray().count(), 1);
    QVERIFY(loadedAcc4->fromJson(results.value("items").toArray().at(0).toObject()));
    QCOMPARE(acc2->toJson(), loadedAcc4->toJson());


    results = find("","lbl1", "session4");
    QCOMPARE(results.value("total").toVariant().toInt(), 1);
    QCOMPARE(results.value("items").toArray().count(), 1);
    Account *loadedAcc6 = new Account();

    QVERIFY(loadedAcc6->fromJson(results.value("items").toArray().at(0).toObject()));
    QVERIFY((acc1->toJson() == loadedAcc6->toJson()));

}


void JsonServiceTest::settingsTest()
{

    unLock("session5");

    QJsonObject data;
    QUrlQuery queries;

    data.insert("timeout", "60");
    data.insert("lang", "fa-IR");
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "setSettings");
    queries.addQueryItem("data",  QJsonDocument(data).toJson(QJsonDocument::Compact) );
    QUrl url("http://localhost:6060/session5/");
    url.setQuery(queries);
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QCOMPARE(reply->readAll(), QByteArray("abc(true);"));
        reply->deleteLater();
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "Request time out!");



    queries.clear();
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "getCurrentProfile");
    url.setUrl("http://localhost:6060/session5/");
    url.setQuery(queries);
    QByteArray res;
    reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [&]() {
        res.append(reply->readAll());
    });
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "Request time out!");


    res = res.mid(4, res.length() - 4 -2);
    QJsonObject profile = QJsonDocument::fromJson(res).object();

    QCOMPARE(profile.value("name").toString(), QString("JsonServiceTest"));
    QCOMPARE(profile.value("db").toString(),QString("./JsonServiceTest.db"));
    QCOMPARE(profile.value("timeout").toInt(), 60);
    QCOMPARE(profile.value("lang").toString(), QString("fa-IR"));
    reply->deleteLater();
}

void JsonServiceTest::passCheckTest()
{
    QUrlQuery queries;

    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "isPassStrong");
    queries.addQueryItem("data",  "12345678" );
    QUrl url("http://localhost:6060/");
    url.setQuery(queries);
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QByteArray dt = reply->readAll();
        dt = dt.mid(4, dt.length() - 4 -2);
        QVERIFY(dt.length() > 10 ); //error string is longer than 10
        reply->deleteLater();
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "Request time out!");


    queries.clear();
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "isPassStrong");
    queries.addQueryItem("data",  "1ss%gdAHG23478" );
    url.setUrl("http://localhost:6060/");
    url.setQuery(queries);
    reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QByteArray dt = reply->readAll();
        dt = dt.mid(4, dt.length() - 4 -2);
        QCOMPARE(dt, QByteArray("\"\""));
        reply->deleteLater();
    });
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "Request time out!");

}



void JsonServiceTest::cleanupTestCase()
{
    QNetworkReply *reply = m_network->get(QNetworkRequest(QUrl("http://localhost:6060/sid1/?callback=abc&cmd=lock")));
    reply->deleteLater();
    ProfileService profileService;
    profileService.removeProfile("JsonServiceTest");
    QFile::remove("./JsonServiceTest.db");
}

void JsonServiceTest::saveAcc(const Account *acc, const QString &sid)
{
    QUrlQuery queries;
    queries.removeQueryItem("cmd"); queries.removeQueryItem("data");
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "save");
    queries.addQueryItem("data",  QString(QJsonDocument(acc->toJson().toObject()).toJson(QJsonDocument::Compact)) );
    QUrl url("http://localhost:6060/" + sid + "/");
    url.setQuery(queries);
    QByteArray res;
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [&]() {
        res.append(reply->readAll());
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QVERIFY2(timeout.elapsed() < 10000, "Request time out!");

    QCOMPARE(res, QByteArray("abc(\"" + acc->getId().toString().toUtf8() + "\");"));
    reply->deleteLater();

}

Account *JsonServiceTest::loadAcc(const QString &id, const QString &sid)
{
    QUrlQuery queries;

    queries.removeQueryItem("cmd"); queries.removeQueryItem("data");
    queries.addQueryItem("cmd", "load");
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("data",  id );
    Account *loadedAcc = new Account(this);
    QUrl url("http://localhost:6060/" + sid + "/");
    url.setQuery(queries);
    QByteArray res;
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [&]() {
        res.append(reply->readAll());
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QTest::qVerify(timeout.elapsed() < 10000, "timeout.elapsed() < 10000", "Request time out!",__FILE__, __LINE__);

    res = res.mid(4, res.length() - 4 -2);

    loadedAcc->fromJson(QJsonDocument::fromJson(res).object());
    reply->deleteLater();

    return loadedAcc;
}

bool JsonServiceTest::removeAcc(const QString &id, const QString &sid)
{
    QUrlQuery queries;

    queries.removeQueryItem("cmd"); queries.removeQueryItem("data");
    queries.addQueryItem("cmd", "remove");
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("data",  id );
    QUrl url("http://localhost:6060/" + sid + "/");
    url.setQuery(queries);
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QCOMPARE(reply->readAll(), QByteArray("abc(true);"));
        reply->deleteLater();
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QTest::qVerify(timeout.elapsed() < 10000, "timeout.elapsed() < 10000", "Request time out!",__FILE__, __LINE__);


    return true;
}

void JsonServiceTest::lock(const QString &sid)
{
    QNetworkReply *reply = m_network->get(QNetworkRequest(QUrl("http://localhost:6060/" + sid + "/?callback=abc&cmd=lock")));
    reply->deleteLater();
}

bool JsonServiceTest::unLock(const QString &sid)
{
    QUrlQuery queries;
    QJsonObject data;

    data.insert("profile", "JsonServiceTest");
    data.insert("pass", "123456");
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", "authenticate");
    queries.addQueryItem("data",  QJsonDocument(data).toJson(QJsonDocument::Compact) );
    QUrl url("http://localhost:6060/" + sid + "/");
    url.setQuery(queries);
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QCOMPARE(reply->readAll(), QByteArray("abc(true);"));
        reply->deleteLater();
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QTest::qVerify(timeout.elapsed() < 10000, "timeout.elapsed() < 10000", "Request time out!",__FILE__, __LINE__);


    return true;
}

bool JsonServiceTest::isUnlocked(const QString &sid, bool expected)
{
    QNetworkReply *reply = m_network->get(QNetworkRequest(QUrl(QString("http://localhost:6060/") + sid + QString("/?callback=abc&cmd=isUnlocked"))));
    connect(reply, &QNetworkReply::finished,  [=]() {
        QCOMPARE(reply->readAll(),  QByteArray("abc(") + QByteArray(expected ? "true" : "false") + QByteArray(");") );
        reply->deleteLater();
    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QTest::qVerify(timeout.elapsed() < 10000, "timeout.elapsed() < 10000", "Request time out!",__FILE__, __LINE__);

    return true;
}

QJsonObject JsonServiceTest::find(const QString &what,const QString &label, const QString &sid, int index, int offset, bool findByUrl)
{
    QUrlQuery queries;
    QJsonObject data;

    if (findByUrl)
        data.insert("url", what);
    else
        data.insert("what", what);
    data.insert("index", index);
    data.insert("offset", offset);
    data.insert("label", label);
    queries.addQueryItem("callback", "abc");
    queries.addQueryItem("cmd", (findByUrl ? "findByUrl" : "find"));
    queries.addQueryItem("data",  QJsonDocument(data).toJson(QJsonDocument::Compact) );

    QJsonObject results;
    QUrl url("http://localhost:6060/" + sid + "/");
    url.setQuery(queries);
    QByteArray res;
    QNetworkReply *reply = m_network->get(QNetworkRequest( url ));
    connect(reply, &QNetworkReply::finished,  [&]() {
        res.append(reply->readAll());

    });
    QElapsedTimer timeout;
    timeout.restart();
    while (!reply->isFinished() && timeout.elapsed() < 10000)
        QCoreApplication::processEvents();
    QTest::qVerify(timeout.elapsed() < 10000, "timeout.elapsed() < 10000", "Request time out!",__FILE__, __LINE__);

    res = res.mid(4, res.length() - 4 -2);
    results = QJsonDocument::fromJson(res).object();
    reply->deleteLater();


    return results;
}

QTEST_MAIN(JsonServiceTest)

#include "tst_jsonservicetest.moc"
