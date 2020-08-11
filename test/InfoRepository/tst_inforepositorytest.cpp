#include <QSqlQuery>
#include <QString>
#include <QtTest>
#include <domain/account.h>
#include <domain/info.h>
#include <domain/database.h>
#include <dataaccess/inforepository.h>
#include <QDebug>
#include <application/databaseservice.h>

class InfoRepositoryTest : public QObject
{
    Q_OBJECT
    
public:
    InfoRepositoryTest();
    
private Q_SLOTS:
    void testCase1();
    void removeTest();
    void cleanUp();
};

InfoRepositoryTest::InfoRepositoryTest()
{
}

void InfoRepositoryTest::testCase1()
{
//    cleanUp();  //delete db file (if remained from last execution)
    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./InfoRepositoryTest.db", "123456"));

    Database db("InfoRepositoryTest");
    QVERIFY(db.open("./InfoRepositoryTest.db", "123456"));
    InfoRepository infoRepo(&db);


    Account acc;
    //acc.setId(1);
    Info info(&acc);
    info.setKey("key");
    info.setValue("value");
    info.setFormInput("txtUser");
    QVERIFY(infoRepo.save(&info));
    QVERIFY(info.isSaved());    
    info.setKey("changed key");
    QCOMPARE(info.isSaved(), false);
    infoRepo.save(&info);
    QVERIFY(info.isSaved());

    QVERIFY(infoRepo.remove(&info));
    QCOMPARE(info.isSaved(), false);

    QCOMPARE(infoRepo.removeByAccountId(QUuid::createUuid()), 0);
    QCOMPARE(infoRepo.remove(QUuid::createUuid()), false);
    QVERIFY(infoRepo.save(&info));
    QVERIFY(info.isSaved());

    QVERIFY(infoRepo.remove(info.getId()));


}

void InfoRepositoryTest::removeTest()
{
    Database db("InfoRepositoryTest");
    QVERIFY(db.open("./InfoRepositoryTest.db", "123456"));
    InfoRepository infoRepo(&db);

    Account acc;
//    acc.setId(5);
    Info *i1 = new Info();
    i1->setKey("a");
    i1->setValue("1");

    acc.addInfo(i1);    //account takes ownership of i1
    acc.addInfo("b", "2");

    QVERIFY(infoRepo.save(acc.getInfoPairs().at(0)));
    QVERIFY(infoRepo.save(acc.getInfoPairs().at(1)));

    QCOMPARE(infoRepo.removeByAccountId(acc.getId()), 2);

    QSqlQuery q = db.query("select count(*) from Info");
    QVERIFY(q.first());
    QVERIFY(q.value(0).isValid());
    QCOMPARE(q.value(0).toInt(), 0);
}

void InfoRepositoryTest::cleanUp()
{
    QFile::remove("./InfoRepositoryTest.db");
}

QTEST_MAIN(InfoRepositoryTest)

#include "tst_inforepositorytest.moc"
