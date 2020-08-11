#include <QSqlQuery>
#include <QString>
#include <QtTest>
#include <application/databaseservice.h>
#include <dataaccess/accountrepository.h>
#include <domain/account.h>
#include <domain/info.h>
#include <domain/database.h>
#include <QDebug>


class AccountRepositoryTest : public QObject
{
    Q_OBJECT

public:
    AccountRepositoryTest();

private:
    AccountRepository accRepo;
    Database *db;

private Q_SLOTS:
    void initTestCase();
    void testCase1();
    void loadBenchmark();
    void addChangeInfo();
    void removeInfo();
    void deleteAccount();
    void labelsTest();
    void cleanupTestCase();
};

AccountRepositoryTest::AccountRepositoryTest()
{
}

void AccountRepositoryTest::initTestCase()
{
//    cleanupTestCase();  //remove old file (if any exists)
    QFile::remove("./AccountRepositoryTest.db");
    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./AccountRepositoryTest.db", "pass"));
    db = new Database("AccountRepositoryTest");
    QVERIFY(db->open/*AndEncrypt*/("./AccountRepositoryTest.db", "pass"));
    accRepo.setDatabase(db);
    QVERIFY(accRepo.isValid());
}

void AccountRepositoryTest::testCase1()
{
    QVERIFY(accRepo.load(QUuid::createUuid(), this) == 0 );
    QCOMPARE(accRepo.loadAll(this).count() , 0);
    QCOMPARE(accRepo.remove(QUuid::createUuid()), false);

    Account acc;
    QUuid id = acc.getId();
    QCOMPARE(accRepo.save(&acc), false);

    acc.setTitle("test acc");
    acc.addLabel("test label");
    //QVERIFY(!acc.isPersisted());
    QVERIFY(accRepo.save(&acc));
    //QVERIFY(acc.isPersisted());

    QVERIFY(acc.isSaved());
    QCOMPARE(acc.getId(), id);

    acc.addInfo("key", "value")->setFormInput("txtInput");
    acc.addInfo("login Url", "http://yahoo.com/a/b/c?a=1")->setType(Info::KeyTypeURL);
    QVERIFY(!acc.isSaved());
    //QCOMPARE(acc.getInfoPairs().at(0)->isPersisted(), false);
    QVERIFY(!acc.getInfoPairs().at(0)->isSaved());
    QVERIFY(accRepo.save(&acc));
    QVERIFY(acc.isSaved());
    //QCOMPARE(acc.getInfoPairs().at(0)->isPersisted(), true);
    QVERIFY(acc.getInfoPairs().at(0)->isSaved());

    Account *acc2 = accRepo.load(id, this);
    QVERIFY(acc2);
    QVERIFY(acc2->isSaved());
    //QVERIFY(acc2->isPersisted());
    QVERIFY(acc2->getCreatedDate() == acc.getCreatedDate());
    QVERIFY(acc2->getId() == acc.getId());
    QCOMPARE(acc2->getInfoPairs().count() , acc.getInfoPairs().count());
    QVERIFY(acc2->getLabels() == acc.getLabels());
    QVERIFY(acc2->getInfoPairs().at(0)->getKey() == acc.getInfoPairs().at(0)->getKey());
    QVERIFY(acc2->getInfoPairs().at(0)->getValue() == acc.getInfoPairs().at(0)->getValue());
    QVERIFY(acc2->getInfoPairs().at(0)->getId() == acc.getInfoPairs().at(0)->getId());
    QVERIFY(acc2->getInfoPairs().at(0)->getFormInput() == acc.getInfoPairs().at(0)->getFormInput());
    QVERIFY(acc2->getTitle() == acc.getTitle());
    QVERIFY(acc2->isSaved() == acc.isSaved());
    QVERIFY(acc2->toJson() == acc.toJson());

    QList<Account *> accounts = accRepo.loadAll(this);
    QCOMPARE(accounts.count(), 1);
    QVERIFY(accounts.at(0)->isSaved());
    QVERIFY(accounts.at(0)->getCreatedDate() == acc.getCreatedDate());
    QVERIFY(accounts.at(0)->getId() == acc.getId());
    QVERIFY(accounts.at(0)->getLabels() == acc.getLabels());
    QVERIFY(accounts.at(0)->getInfoPairs().count() == acc.getInfoPairs().count());
    QVERIFY(accounts.at(0)->getInfoPairs().at(0)->getKey() == acc.getInfoPairs().at(0)->getKey());
    QVERIFY(accounts.at(0)->getInfoPairs().at(0)->getValue() == acc.getInfoPairs().at(0)->getValue());
    QVERIFY(accounts.at(0)->getInfoPairs().at(0)->getId() == acc.getInfoPairs().at(0)->getId());
    QVERIFY(accounts.at(0)->getTitle() == acc.getTitle());
    QVERIFY(accounts.at(0)->isSaved() == acc.isSaved());
    qDeleteAll(accounts);


    accounts = accRepo.loadByUrl("http://yahoo.com",this);
    QCOMPARE(accounts.count(), 1);
    QVERIFY(accounts.at(0)->isSaved());
    QVERIFY(accounts.at(0)->getCreatedDate() == acc.getCreatedDate());
    QVERIFY(accounts.at(0)->getId() == acc.getId());
    QVERIFY(accounts.at(0)->getLabels() == acc.getLabels());
    QVERIFY(accounts.at(0)->getInfoPairs().count() == acc.getInfoPairs().count());
    QVERIFY(accounts.at(0)->getInfoPairs().at(0)->getKey() == acc.getInfoPairs().at(0)->getKey());
    QVERIFY(accounts.at(0)->getInfoPairs().at(0)->getValue() == acc.getInfoPairs().at(0)->getValue());
    QVERIFY(accounts.at(0)->getInfoPairs().at(0)->getId() == acc.getInfoPairs().at(0)->getId());
    QVERIFY(accounts.at(0)->getTitle() == acc.getTitle());
    QVERIFY(accounts.at(0)->isSaved() == acc.isSaved());
    qDeleteAll(accounts);


    accounts = accRepo.loadByUrl("http://www.yahoo.com",this);
    QCOMPARE(accounts.count(), 0);

    delete acc2;


}

void AccountRepositoryTest::loadBenchmark()
{
//    QVERIFY(accRepo.load(1, this) != 0 );

    Account acc;
    QUuid id = acc.getId();
    acc.setTitle("test acc");
    QVERIFY(accRepo.save(&acc));


    QElapsedTimer elapsed;
    elapsed.start();
    for (int i=0; i <50; ++i)
        accRepo.load(id, this);
    qDebug() << "average account load time:" << elapsed.elapsed()/50;
}

void AccountRepositoryTest::addChangeInfo()
{
//    Account *acc2 = accRepo.load(1, this);
    Account *acc2 = new Account(this);
//    QUuid id = acc2.getId();
    acc2->setTitle("test acc");
    acc2->addLabel("label1");
    QVERIFY(accRepo.save(acc2));


    acc2->addInfo("new key", "new value");
    QVERIFY(!acc2->isSaved());
    QVERIFY(accRepo.save(acc2));
    QVERIFY(acc2->isSaved());
    //QCOMPARE(acc2->getInfoPairs().at(0)->isPersisted(), true);
    QVERIFY(acc2->getInfoPairs().at(0)->isSaved());
    acc2->getInfoPairs().at(0)->setKey("changed key");
    acc2->getInfoPairs().at(0)->setValue("changed value");
    acc2->getInfoPairs().at(0)->setOrder(2);
    acc2->getInfoPairs().at(0)->setType(Info::KeyTypeTags);
    acc2->addInfo("key2", "value2");
    acc2->addLabel("label2");
    QVERIFY(!acc2->isSaved());
    QVERIFY(accRepo.save(acc2));
    QVERIFY(acc2->isSaved());


    Account *acc3 = accRepo.load(acc2->getId(), this);
    QVERIFY(acc3->getCreatedDate() == acc2->getCreatedDate());
    QVERIFY(acc3->getId() == acc2->getId());
    QVERIFY(acc3->getInfoPairs().count() == acc2->getInfoPairs().count());
    QVERIFY(acc3->getInfoPairs().at(0)->getKey() == acc2->getInfoPairs().at(0)->getKey());
    QVERIFY(acc3->getInfoPairs().at(0)->getValue() == acc2->getInfoPairs().at(0)->getValue());
    QVERIFY(acc3->getInfoPairs().at(0)->getId() == acc2->getInfoPairs().at(0)->getId());
    QVERIFY(acc3->getInfoPairs().at(1)->getKey() == acc2->getInfoPairs().at(1)->getKey());
    QVERIFY(acc3->getInfoPairs().at(1)->getValue() == acc2->getInfoPairs().at(1)->getValue());
    QVERIFY(acc3->getInfoPairs().at(1)->getId() == acc2->getInfoPairs().at(1)->getId());
    QVERIFY(acc3->getInfoPairs().at(1)->getOrder() == acc2->getInfoPairs().at(1)->getOrder());
    QVERIFY(acc3->getInfoPairs().at(1)->getType() == acc2->getInfoPairs().at(1)->getType());
    QVERIFY(acc3->getTitle() == acc2->getTitle());
    QVERIFY(acc3->getLabels() == acc2->getLabels());
    QVERIFY(acc3->isSaved() == acc2->isSaved());



    delete acc2;
    delete acc3;
}

void AccountRepositoryTest::removeInfo()
{
    Account accNoChange;
    accNoChange.setTitle("no change account");
    accNoChange.addInfo("key", "value");
    accNoChange.addLabel("tag");
    QVERIFY(accRepo.save(&accNoChange));

    Account acc;
    acc.setTitle("remove info test");
    acc.addLabel("lbl1");
    Info *i1 = acc.addInfo("a", "1");
    Info *i2 = acc.addInfo("b", "2");

    i2->setOrder(1);
    i1->setOrder(2);
    i1->setType(Info::KeyTypeComment);
    i2->setType(Info::KeyTypePassword);

    accRepo.save(&acc);

    QVERIFY(acc.isSaved());



    Account *acc2 = accRepo.load(acc.getId(), this);
    QVERIFY(acc2 != 0);
    QVERIFY(acc2->isSaved());

    int infoCnt = acc2->getInfoPairs().count();
    QVERIFY(acc2->removeInfo(acc2->getInfoPairs().at(0)) >0);   //remove i1
    QCOMPARE(acc2->getInfoPairs().count(),  infoCnt-1);

    acc2->setLabels(QStringList());
    QCOMPARE(acc2->isSaved(), false);


    QVERIFY(accRepo.save(acc2));
    QVERIFY(acc2->isSaved());

    Account *acc3 = accRepo.load(acc2->getId(), this);
    QVERIFY(acc3 != 0);
    QVERIFY(acc3->isSaved());
    QVERIFY(acc3->getCreatedDate() == acc2->getCreatedDate());
    QVERIFY(acc3->getId() == acc2->getId());
    QVERIFY(acc3->getInfoPairs().count() == acc2->getInfoPairs().count());
    QVERIFY(acc3->getInfoPairs().at(0)->getKey() == acc2->getInfoPairs().at(0)->getKey());
    QVERIFY(acc3->getInfoPairs().at(0)->getValue() == acc2->getInfoPairs().at(0)->getValue());
    QVERIFY(acc3->getInfoPairs().at(0)->getId() == acc2->getInfoPairs().at(0)->getId());
    QVERIFY(acc3->getInfoPairs().at(0)->getOrder() == 1);   //NOTE: info orders are re-sorted when they'are added to Account. acc3 only has 1 info so it's order is 1 instead of it's previous value (2)
    QVERIFY(acc3->getInfoPairs().at(0)->getType() == acc2->getInfoPairs().at(0)->getType());
    QVERIFY(acc3->getTitle() == acc2->getTitle());
    QVERIFY(acc3->isSaved() == acc2->isSaved());
    QVERIFY(acc3->getLabels() == acc2->getLabels());



    Account *accNoChangeReloaded = accRepo.load(accNoChange.getId(), this);
    QVERIFY(accNoChangeReloaded != 0);
    QVERIFY(accNoChangeReloaded->isSaved());
    QVERIFY(accNoChangeReloaded->getCreatedDate() == accNoChange.getCreatedDate());
    QVERIFY(accNoChangeReloaded->getId() == accNoChange.getId());
    QVERIFY(accNoChangeReloaded->getInfoPairs().count() == accNoChange.getInfoPairs().count());
    QVERIFY(accNoChangeReloaded->getInfoPairs().at(0)->getKey() == accNoChange.getInfoPairs().at(0)->getKey());
    QVERIFY(accNoChangeReloaded->getInfoPairs().at(0)->getValue() == accNoChange.getInfoPairs().at(0)->getValue());
    QVERIFY(accNoChangeReloaded->getInfoPairs().at(0)->getId() == accNoChange.getInfoPairs().at(0)->getId());
    QVERIFY(accNoChangeReloaded->getInfoPairs().at(0)->getOrder() == accNoChange.getInfoPairs().at(0)->getOrder());
    QVERIFY(accNoChangeReloaded->getInfoPairs().at(0)->getType() == accNoChange.getInfoPairs().at(0)->getType());
    QVERIFY(accNoChangeReloaded->getTitle() == accNoChange.getTitle());
    QVERIFY(accNoChangeReloaded->isSaved() == accNoChange.isSaved());
    QVERIFY(accNoChangeReloaded->getLabels() == accNoChange.getLabels());



    delete acc2;
    delete acc3;
    delete accNoChangeReloaded;

}

void AccountRepositoryTest::deleteAccount()
{
    Account acc_1;
    //QUuid id = acc_1.getId();
    acc_1.setTitle("test acc");
    QVERIFY(accRepo.save(&acc_1));

    Account acc_2;
    //QUuid id = acc_2.getId();
    acc_2.setTitle("test acc");
    QVERIFY(accRepo.save(&acc_2));


    QCOMPARE(accRepo.remove(QUuid::createUuid()), false);
    QVERIFY(accRepo.remove(acc_1.getId()));
    Account *acc = accRepo.load(acc_2.getId());
    QVERIFY(acc->isSaved());
    accRepo.remove(acc);
    QVERIFY(!acc->isSaved());
    //QCOMPARE(acc->isPersisted(), false);
//    QVERIFY(accRepo.remove(3));

    QList<Account*> accs= accRepo.loadAll(this);
    foreach(Account *a, accs)
        QVERIFY(accRepo.remove(a));

    QSqlQuery q = db->query("select count(*) from Info");
    QVERIFY(q.first());
    QVERIFY(q.value(0).isValid());
    QCOMPARE(q.value(0).toInt(), 0);

    q = db->query("select count(*) from Account");
    QVERIFY(q.first());
    QVERIFY(q.value(0).isValid());
    QCOMPARE(q.value(0).toInt(), 0);


}

void AccountRepositoryTest::labelsTest()
{
    Account acc_1;
    acc_1.setTitle("test acc");
    acc_1.addLabel("work");
    acc_1.addLabel("social");
    QVERIFY(accRepo.save(&acc_1));

    Account acc_2;
    acc_2.setTitle("test acc2");
    acc_2.addLabel("Social");
    acc_2.addLabel("bank");
    QVERIFY(accRepo.save(&acc_2));

    Account acc_3;
    acc_3.setTitle("test acc3");
    acc_3.addLabel("work");
    acc_3.addLabel("email");
    QVERIFY(accRepo.save(&acc_3));

    Account acc_4;
    acc_4.setTitle("test acc4");
    acc_4.addLabel("wifi");
    QVERIFY(accRepo.save(&acc_4));


    QStringList labels = accRepo.labels();
    QCOMPARE(labels.count(), 5);
    QVERIFY(labels.contains("work", Qt::CaseInsensitive));
    QVERIFY(labels.contains("social", Qt::CaseInsensitive));
    QVERIFY(labels.contains("bank", Qt::CaseInsensitive));
    QVERIFY(labels.contains("Email", Qt::CaseInsensitive));
    QVERIFY(labels.contains("wifi", Qt::CaseInsensitive));


    QElapsedTimer elapsed;
    elapsed.start();
    for (int i=0; i <1000; ++i)
        QStringList labels = accRepo.labels();
    qDebug() << "average get labels time:" << elapsed.elapsed();
}

void AccountRepositoryTest::cleanupTestCase()
{
    delete db;
    QFile::remove("./AccountRepositoryTest.db");
}

QTEST_APPLESS_MAIN(AccountRepositoryTest)

#include "tst_accountrepositorytest.moc"
