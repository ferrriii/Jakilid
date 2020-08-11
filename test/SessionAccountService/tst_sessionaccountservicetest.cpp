#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <domain/account.h>
#include <domain/session.h>
#include <application/sessionaccountservice.h>
#include <application/profileservice.h>
#include <application/sessionservice.h>
#include <application/databaseservice.h>
#include <domain/database.h>
#include <QSqlError>

class SessionAccountServiceTest : public QObject
{
    Q_OBJECT

public:
    SessionAccountServiceTest();
private:
    SessionAccountService sas;
private Q_SLOTS:
    void initTest();
    void testCase1();
    void findTest();
    void findByUrl();
    void loadBenchmark();
    void findBenchMark();
    void cleanUp();
};

SessionAccountServiceTest::SessionAccountServiceTest()
{
}

void SessionAccountServiceTest::initTest()
{
    //NOTE: required by profile repository
    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain("unittest");
    QCoreApplication::setApplicationName("SessionAccountServiceTest");

    cleanUp();
    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./SessionAccountServiceTest.db", "123456")); //db should exists before creating profile

    ProfileService profileService;
    Profile profile("SessionAccountServiceTest", "./SessionAccountServiceTest.db");
    QVERIFY(profileService.addProfile(&profile, "123456"));

    SessionService sessionService;
    Session *session;

    session = sessionService.createSession("SessionAccountServiceTest");
    QVERIFY(session != 0);
    QVERIFY(session->isValid());
    QVERIFY(session->unlock("123456"));
    QVERIFY(session->isUnlocked());


    sas.setSession(session);
    QVERIFY(sas.isValid());

}

void SessionAccountServiceTest::testCase1()
{
    Account *acc = sas.load(QUuid::createUuid() );
    QVERIFY(acc == 0);
    QList<Account *> allAcc = sas.find();
    QCOMPARE(allAcc.count(), 0);
    QCOMPARE(sas.remove(QUuid::createUuid()), false);

    Account a1;
    a1.setTitle("test account");
    a1.addInfo("key", "value")->setFormInput("txtInput");

    //save
    QVERIFY(sas.save(&a1));

    //session lock test
    sas.getSession()->lock();
    QVERIFY(sas.load(a1.getId()) == 0);
    sas.getSession()->unlock("123456");

    //load
    acc = sas.load(a1.getId());
    QVERIFY(acc != 0);
    QCOMPARE(acc->getTitle(), a1.getTitle());
    QCOMPARE(acc->getId(), a1.getId());
    QCOMPARE(acc->getCreatedDate(), a1.getCreatedDate());
    QCOMPARE(acc->getInfoPairs().count(), a1.getInfoPairs().count());
    QCOMPARE(acc->getInfoPairs().at(0)->getKey(), a1.getInfoPairs().at(0)->getKey());
    QCOMPARE(acc->getInfoPairs().at(0)->getValue(), a1.getInfoPairs().at(0)->getValue());
    QCOMPARE(acc->getInfoPairs().at(0)->getOrder(), a1.getInfoPairs().at(0)->getOrder());
    QCOMPARE(acc->getInfoPairs().at(0)->getType(), a1.getInfoPairs().at(0)->getType());
    QCOMPARE(acc->getInfoPairs().at(0)->getFormInput(), a1.getInfoPairs().at(0)->getFormInput());

    //remove
    QVERIFY(sas.remove(a1.getId()));

    delete acc;

}

void SessionAccountServiceTest::findTest()
{
    Account a1, a2;
    a1.setTitle("test account");
    a1.addInfo("key", "value");
    a1.addLabel("label1");

    a2.setTitle("bank account");
    a2.addInfo("account number", "334455");
    a2.addLabel("label2");
    a2.addLabel("label3");

    //NOTE: account are sorted by modified date desc, so 2nd account (a2) should be first in results but
    //since a1 and a2 are saved with no delay their modified date may be same (date format is utc unixepoch in seconds)
    //so in this case a2 will be 2nd in results
    QVERIFY(sas.save(&a1));
    QElapsedTimer delay;
    delay.start();
    while (delay.elapsed() < 1100);
    QVERIFY(sas.save(&a2));

    QList<Account *> allAcc;
    allAcc = sas.find();
    QCOMPARE(allAcc.count(), 2);
    QCOMPARE(allAcc.at(0)->getTitle() , a2.getTitle());
    QCOMPARE(allAcc.at(0)->getLabels() , a2.getLabels());
    QCOMPARE(allAcc.at(1)->getTitle() , a1.getTitle());
    QCOMPARE(allAcc.at(1)->getLabels() , a1.getLabels());
    qDeleteAll(allAcc);


    allAcc = sas.find("","", 1,0);
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , QString("bank account"));
    qDeleteAll(allAcc);

    allAcc = sas.find("","", 1, 1);
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , QString("test account"));
    qDeleteAll(allAcc);

    allAcc = sas.find("bank");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , QString("bank account"));
    qDeleteAll(allAcc);

    allAcc = sas.find("test");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() ,QString("test account"));
    qDeleteAll(allAcc);

    allAcc = sas.find("bank account","", 1);
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , QString("bank account"));
    qDeleteAll(allAcc);

    allAcc = sas.find("bank account","", -1, 2);
    QCOMPARE(allAcc.count(), 0);

    allAcc = sas.find("33","", 1);
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , QString("bank account"));
    qDeleteAll(allAcc);

    allAcc = sas.find("334455");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , QString("bank account"));
    qDeleteAll(allAcc);

    allAcc = sas.find("test");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , QString("test account"));
    qDeleteAll(allAcc);

    //test labels
    allAcc = sas.find("","label1");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , a1.getTitle());
    QCOMPARE(allAcc.at(0)->getLabels().count() , 1);
    QCOMPARE(allAcc.at(0)->getLabels().at(0) ,QString("label1"));
    QCOMPARE(allAcc.at(0)->getLabels() , a1.getLabels());
    qDeleteAll(allAcc);

    //test labels
    allAcc = sas.find("","label3");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , a2.getTitle());
    QCOMPARE(allAcc.at(0)->getLabels().count() , 2);
    QCOMPARE(allAcc.at(0)->getLabels().at(0) ,QString("label2"));
    QCOMPARE(allAcc.at(0)->getLabels().at(1) ,QString("label3"));
    QCOMPARE(allAcc.at(0)->getLabels() , a2.getLabels());
    qDeleteAll(allAcc);

    //test labels
    allAcc = sas.find("","non existing label");
    QCOMPARE(allAcc.count(), 0);

}

void SessionAccountServiceTest::findByUrl()
{
    Account a1;
    a1.setTitle("test account");
    a1.addInfo("key", "value");
    Info *url = new Info(&a1);  //accout takes ownership of info
    url->setType(Info::KeyTypeURL);
    url->setKey("URL");
    url->setValue("https://mail.live.com/?login=2s");
    a1.addInfo(url);


    QVERIFY(sas.save(&a1));
    QElapsedTimer delay;
    delay.start();



    QList<Account *> allAcc;
    allAcc = sas.findByUrl("http://mail.live.com/sdd/eda/?s");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , a1.getTitle());
    QCOMPARE(allAcc.at(0)->getInfoPairs().count() , a1.getInfoPairs().count());
    QCOMPARE(allAcc.at(0)->getInfoPairs().at(0)->getKey() , a1.getInfoPairs().at(0)->getKey());
    QCOMPARE(allAcc.at(0)->getInfoPairs().at(0)->getValue() , a1.getInfoPairs().at(0)->getValue());
    QCOMPARE(allAcc.at(0)->getInfoPairs().at(0)->getType() , a1.getInfoPairs().at(0)->getType());
    qDeleteAll(allAcc);

    allAcc = sas.findByUrl("https://mail.live.com");
    QCOMPARE(allAcc.count(), 1);
    QCOMPARE(allAcc.at(0)->getTitle() , a1.getTitle());
    QCOMPARE(allAcc.at(0)->getInfoPairs().count() , a1.getInfoPairs().count());
    QCOMPARE(allAcc.at(0)->getInfoPairs().at(0)->getKey() , a1.getInfoPairs().at(0)->getKey());
    QCOMPARE(allAcc.at(0)->getInfoPairs().at(0)->getValue() , a1.getInfoPairs().at(0)->getValue());
    QCOMPARE(allAcc.at(0)->getInfoPairs().at(0)->getType() , a1.getInfoPairs().at(0)->getType());
    qDeleteAll(allAcc);

    allAcc = sas.findByUrl("http://mail.yahoo.com/sdd/eda/?s");
    QCOMPARE(allAcc.count(), 0);
    qDeleteAll(allAcc);

    allAcc = sas.findByUrl("www.mail.yahoo.com");
    QCOMPARE(allAcc.count(), 0);
    qDeleteAll(allAcc);
}

void SessionAccountServiceTest::loadBenchmark()
{
    Account *acc = new Account(this);
    QList<Account*> accounts;


    //empty db
    accounts = sas.find();
    foreach (Account *acc, accounts)
        QVERIFY(sas.remove(acc->getId()));
    qDeleteAll(accounts);


    Info *info1 = acc->addInfo("username", "user");
    Info *info2 = acc->addInfo("password", "pass");
    //acc->setId(-1);
    acc->setTitle(QString("account %1"));
    info1->setValue(QString("username"));
    info2->setValue(QString("password12344"));
    acc->addInfo("Login URL", "https://mail.live.com/?login=2s")->setType(Info::KeyTypeURL);
    QVERIFY(sas.save(acc));

    QUuid id = acc->getId();
    QVERIFY(sas.load( id ) != 0);

    QElapsedTimer elapsed;
    elapsed.start();
    for (int i=0; i<50; ++i)
        sas.load(id);

    qDebug() << "average account load time:" << elapsed.elapsed()/50;


    elapsed.restart();
    accounts = sas.find();
    qDebug() << "load all time:" << elapsed.elapsed();

    elapsed.restart();
    foreach (Account *acc, accounts)
        QVERIFY(sas.remove(acc->getId()));
    qDebug() << "Average Account Deletion Time: " << elapsed.elapsed()/50;
    qDeleteAll(accounts);
}

void SessionAccountServiceTest::findBenchMark()
{
    Account *acc = new Account(this);
    QList<Account*> accounts;

    QElapsedTimer elapsed;

    //empty db
    accounts = sas.find();
    foreach (Account *acc, accounts)
        QVERIFY(sas.remove(acc->getId()));
    qDeleteAll(accounts);
    accounts.empty();
    QVERIFY(sas.find().count() == 0);


    elapsed.start();
    acc->addLabel("bank");
    acc->addLabel("social");
    Info *info1 = acc->addInfo("username", "user");
    Info *info2 = acc->addInfo("password", "pass");
    Info *url = new Info(acc);  //accout takes ownership of info
    url->setType(Info::KeyTypeURL);
    url->setKey("URL");
    url->setValue("https://mail.live.com/?login=2s");
    acc->addInfo(url);
    int accountCount = 500;
    for (int i= 0; i<accountCount; ++i)
    {
        acc->setId(QUuid::createUuid());
        info1->setId(QUuid::createUuid());
        info2->setId(QUuid::createUuid());
        url->setId(QUuid::createUuid());
        //acc->setIsPersisted(false);
        //info1->setIsPersisted(false);
        //info2->setIsPersisted(false);
        acc->setTitle(QStringLiteral("account %1").arg(i));
        info1->setValue(QStringLiteral("user %1").arg(i));
        info2->setValue(QStringLiteral("pass %1").arg(i));
        QVERIFY2(sas.save(acc), QStringLiteral("%1th insert failed").arg(i).toStdString().data());
    }
    qDebug() << "Average New Account Save Time: " << elapsed.elapsed()/accountCount;
    delete acc;



    elapsed.restart();
    accounts = sas.findByUrl("http://mail.live.com/sdd/eda/?s");
    qDebug() << QString("Find by URL time (%1 account): ").arg(accountCount) << elapsed.elapsed();
    QCOMPARE(accounts.count() , accountCount);
    qDeleteAll (accounts);

    elapsed.restart();
    accounts = sas.find();
    qDebug() << QString("Find All time (%1 account): ").arg(accountCount) << elapsed.elapsed();
    QCOMPARE(accounts.count() , accountCount);
    qDeleteAll (accounts);

    elapsed.restart();
    accounts = sas.find(QString("account %1").arg(accountCount-1));
    qDebug() << QString("Exact title find time (in %1 accounts): ").arg(accountCount) << elapsed.elapsed();
    QVERIFY(accounts.count() >0);
    qDeleteAll (accounts);

    elapsed.restart();
    accounts = sas.find(QString("%1").arg(accountCount-1));
    qDebug() << QString("Title find time (in %1 accounts): ").arg(accountCount) << elapsed.elapsed();
    QVERIFY(accounts.count() >0);
    qDeleteAll (accounts);

    elapsed.restart();
    accounts = sas.find(QString("user %1").arg(accountCount-1));
    qDebug() << QString("Exact info value find time (in %1 accounts): ").arg(accountCount) << elapsed.elapsed();
    QVERIFY(accounts.count() >0);
    qDeleteAll (accounts);

    elapsed.restart();
    accounts = sas.find(QString("ss %1").arg(accountCount-1));
    qDebug() << QString("Info value find time (in %1 accounts): ").arg(accountCount) << elapsed.elapsed();
    QVERIFY(accounts.count() >0);
    qDeleteAll (accounts);

    elapsed.restart();
    accounts = sas.find("xxxx");
    qDebug() << QString("Not found find time (in %1 accounts): ").arg(accountCount) << elapsed.elapsed();
    QVERIFY(accounts.count() == 0);
    qDeleteAll (accounts);

    elapsed.restart();
    accounts = sas.find("", "social");
    qDebug() << QString("find all by labels time (in %1 accounts): ").arg(accountCount) << elapsed.elapsed();
    QVERIFY(accounts.count() >0);
    qDeleteAll (accounts);

    /*
    accounts = sas.find();
    elapsed.restart();
    foreach (Account *acc, accounts)
        QVERIFY(sas.remove(acc->getId()));
    qDebug() << "Average Account Deletion Time: " << elapsed.elapsed()/accountCount;
    qDeleteAll(accounts);
*/
}


void SessionAccountServiceTest::cleanUp()
{
    if (sas.getSession())
    {
        sas.getSession()->lock();
        QCOMPARE(sas.getSession()->isUnlocked(), false);
        QFile::remove(sas.getSession()->profile()->db());
    }
    ProfileService profileService;
    profileService.removeProfile("SessionAccountServiceTest");
}

QTEST_MAIN(SessionAccountServiceTest)

#include "tst_sessionaccountservicetest.moc"
