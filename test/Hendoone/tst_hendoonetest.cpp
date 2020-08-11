#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <application/databaseservice.h>
#include <application/profileservice.h>
#include <domain/account.h>
#include <../../ui/gui/hendoone.h>

class HendooneTest : public QObject
{
    Q_OBJECT
    
public:
    HendooneTest();
    
private:
    Hendoone *m_hendoone;
private Q_SLOTS:
    void initTestCase();
    void lockAndUnlockTest();
    void saveLoadTest();
    void findTest();
    void removeTest();
    void profileTest();
    void cleanupTestCase();
};

HendooneTest::HendooneTest()
{
}

void HendooneTest::initTestCase()
{
    //required by profile repository
    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain("unittest");
    QCoreApplication::setApplicationName("SessionAccountServiceTest");

    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./HendooneTest.db", "123456"));

    ProfileService profileService;
    Profile profile("HendooneTest", "./HendooneTest.db");
    QVERIFY(profileService.addProfile(&profile, "123456"));

    m_hendoone = new Hendoone(this);

}

void HendooneTest::lockAndUnlockTest()
{
    QSignalSpy signalLocked(m_hendoone, SIGNAL(locked()));
    QSignalSpy signalUnlocked(m_hendoone, SIGNAL(unLocked()));

    QVERIFY(!m_hendoone->authenticate("HendooneTest", "wrong pass"));
    QVERIFY(!m_hendoone->isUnlocked());
    QVERIFY(m_hendoone->authenticate("HendooneTest", "123456"));
    QVERIFY(m_hendoone->isUnlocked());
    QCOMPARE(signalLocked.count(), 0);
    QCOMPARE(signalUnlocked.count(), 1);



    m_hendoone->lock();
    QVERIFY(!m_hendoone->isUnlocked());
    QCOMPARE(signalLocked.count(), 1);
    QCOMPARE(signalUnlocked.count(), 1);

    QVERIFY(!m_hendoone->authenticate("not_existing_profile", "password"));
    QVERIFY(!m_hendoone->isUnlocked());
    QCOMPARE(signalLocked.count(), 1);
    QCOMPARE(signalUnlocked.count(), 1);


    QVERIFY(m_hendoone->authenticate("HendooneTest", "123456"));

}

void HendooneTest::saveLoadTest()
{
    Account *acc = new Account(this);
    acc->setTitle("test account");
    acc->addInfo("key", "value");


    QString id = m_hendoone->save(QJsonDocument(acc->toJson().toObject()).toJson());
    QVERIFY(!id.isEmpty());
    QString loadedAcc = m_hendoone->load(id);
    QVERIFY(!loadedAcc.isEmpty());

//    acc->setId(id);
//    acc->getInfoPairs().at(0)->setId(1);
    //acc->setSaved(true);
    //acc->setIsPersisted(true);
    //acc->getInfoPairs().at(0)->setIsPersisted(true);
    QVERIFY(QJsonDocument(acc->toJson().toObject()).toJson(QJsonDocument::Compact) == loadedAcc);

    delete acc;

}


void HendooneTest::findTest()
{
    QJsonObject queryResults =  QJsonDocument::fromJson(m_hendoone->find(QString("test")).toUtf8()).object();
    QVERIFY(queryResults.value("total").toDouble() == 1);
    QVERIFY(queryResults.value("items").toArray().count() == 1);
    QVERIFY(queryResults.value("items").toArray().at(0).toObject().value("title") == QString("test account"));

}

void HendooneTest::removeTest()
{
    Account *acc = new Account(this);
    acc->setTitle("test account");
    acc->addInfo("key", "value");
    QString id = m_hendoone->save(QJsonDocument(acc->toJson().toObject()).toJson());

    QVERIFY(m_hendoone->remove(id));
    QString loadedAcc = m_hendoone->load(id);
    QVERIFY(loadedAcc.isEmpty());

}

void HendooneTest::profileTest()
{
    QVERIFY(QJsonDocument::fromJson(m_hendoone->getCurrentProfile().toUtf8()).toVariant().toMap().value("name").toString() == QString("HendooneTest"));
    QVERIFY(QJsonDocument::fromJson(m_hendoone->getCurrentProfile().toUtf8()).toVariant().toMap().value("db").toString() == QString("./HendooneTest.db"));
    QVERIFY(QJsonDocument::fromJson(m_hendoone->getCurrentProfile().toUtf8()).toVariant().toMap().value("timeout").toInt() == 600);
    QVERIFY(m_hendoone->setSettings(500, "fa-IR"));
    QVERIFY(QJsonDocument::fromJson(m_hendoone->getCurrentProfile().toUtf8()).toVariant().toMap().value("timeout").toInt() == 500);
    QVERIFY(QJsonDocument::fromJson(m_hendoone->getCurrentProfile().toUtf8()).toVariant().toMap().value("lang").toString() == "fa-IR");
    m_hendoone->lock();
    QVERIFY(m_hendoone->authenticate("HendooneTest", "123456"));
    QVERIFY(QJsonDocument::fromJson(m_hendoone->getCurrentProfile().toUtf8()).toVariant().toMap().value("timeout").toInt() == 500);

}

void HendooneTest::cleanupTestCase()
{
    m_hendoone->lock();
    ProfileService profileService;
    profileService.removeProfile("HendooneTest");
    QFile::remove("./HendooneTest.db");

}


QTEST_MAIN(HendooneTest)

#include "tst_hendoonetest.moc"
