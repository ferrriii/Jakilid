#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <application/databaseservice.h>
#include <application/profileservice.h>
#include <domain/account.h>
#include <../../ui/gui/jakilid.h>

class JakilidTest : public QObject
{
    Q_OBJECT
    
public:
    JakilidTest();
    
private:
    Jakilid *m_jakilid;
private Q_SLOTS:
    void initTestCase();
    void lockAndUnlockTest();
    void saveLoadTest();
    void findTest();
    void removeTest();
    void profileTest();
    void cleanupTestCase();
};

JakilidTest::JakilidTest()
{
}

void JakilidTest::initTestCase()
{
    //required by profile repository
    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain("unittest");
    QCoreApplication::setApplicationName("SessionAccountServiceTest");

    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./JakilidTest.db", "123456"));

    ProfileService profileService;
    Profile profile("JakilidTest", "./JakilidTest.db");
    QVERIFY(profileService.addProfile(&profile, "123456"));

    m_jakilid = new Jakilid(this);

}

void JakilidTest::lockAndUnlockTest()
{
    QSignalSpy signalLocked(m_jakilid, SIGNAL(locked()));
    QSignalSpy signalUnlocked(m_jakilid, SIGNAL(unLocked()));

    QVERIFY(!m_jakilid->authenticate("JakilidTest", "wrong pass"));
    QVERIFY(!m_jakilid->isUnlocked());
    QVERIFY(m_jakilid->authenticate("JakilidTest", "123456"));
    QVERIFY(m_jakilid->isUnlocked());
    QCOMPARE(signalLocked.count(), 0);
    QCOMPARE(signalUnlocked.count(), 1);



    m_jakilid->lock();
    QVERIFY(!m_jakilid->isUnlocked());
    QCOMPARE(signalLocked.count(), 1);
    QCOMPARE(signalUnlocked.count(), 1);

    QVERIFY(!m_jakilid->authenticate("not_existing_profile", "password"));
    QVERIFY(!m_jakilid->isUnlocked());
    QCOMPARE(signalLocked.count(), 1);
    QCOMPARE(signalUnlocked.count(), 1);


    QVERIFY(m_jakilid->authenticate("JakilidTest", "123456"));

}

void JakilidTest::saveLoadTest()
{
    Account *acc = new Account(this);
    acc->setTitle("test account");
    acc->addInfo("key", "value");


    QString id = m_jakilid->save(QJsonDocument(acc->toJson().toObject()).toJson());
    QVERIFY(!id.isEmpty());
    QString loadedAcc = m_jakilid->load(id);
    QVERIFY(!loadedAcc.isEmpty());

//    acc->setId(id);
//    acc->getInfoPairs().at(0)->setId(1);
    //acc->setSaved(true);
    //acc->setIsPersisted(true);
    //acc->getInfoPairs().at(0)->setIsPersisted(true);
    QVERIFY(QJsonDocument(acc->toJson().toObject()).toJson(QJsonDocument::Compact) == loadedAcc);

    delete acc;

}


void JakilidTest::findTest()
{
    QJsonObject queryResults =  QJsonDocument::fromJson(m_jakilid->find(QString("test")).toUtf8()).object();
    QVERIFY(queryResults.value("total").toDouble() == 1);
    QVERIFY(queryResults.value("items").toArray().count() == 1);
    QVERIFY(queryResults.value("items").toArray().at(0).toObject().value("title") == QString("test account"));

}

void JakilidTest::removeTest()
{
    Account *acc = new Account(this);
    acc->setTitle("test account");
    acc->addInfo("key", "value");
    QString id = m_jakilid->save(QJsonDocument(acc->toJson().toObject()).toJson());

    QVERIFY(m_jakilid->remove(id));
    QString loadedAcc = m_jakilid->load(id);
    QVERIFY(loadedAcc.isEmpty());

}

void JakilidTest::profileTest()
{
    QVERIFY(QJsonDocument::fromJson(m_jakilid->getCurrentProfile().toUtf8()).toVariant().toMap().value("name").toString() == QString("JakilidTest"));
    QVERIFY(QJsonDocument::fromJson(m_jakilid->getCurrentProfile().toUtf8()).toVariant().toMap().value("db").toString() == QString("./JakilidTest.db"));
    QVERIFY(QJsonDocument::fromJson(m_jakilid->getCurrentProfile().toUtf8()).toVariant().toMap().value("timeout").toInt() == 600);
    QVERIFY(m_jakilid->setSettings(500, "fa-IR"));
    QVERIFY(QJsonDocument::fromJson(m_jakilid->getCurrentProfile().toUtf8()).toVariant().toMap().value("timeout").toInt() == 500);
    QVERIFY(QJsonDocument::fromJson(m_jakilid->getCurrentProfile().toUtf8()).toVariant().toMap().value("lang").toString() == "fa-IR");
    m_jakilid->lock();
    QVERIFY(m_jakilid->authenticate("JakilidTest", "123456"));
    QVERIFY(QJsonDocument::fromJson(m_jakilid->getCurrentProfile().toUtf8()).toVariant().toMap().value("timeout").toInt() == 500);

}

void JakilidTest::cleanupTestCase()
{
    m_jakilid->lock();
    ProfileService profileService;
    profileService.removeProfile("JakilidTest");
    QFile::remove("./JakilidTest.db");

}


QTEST_MAIN(JakilidTest)

#include "tst_jakilidtest.moc"
