#include <QString>
#include <QtTest>
#include <application/databaseservice.h>
class DatabaseServiceTest : public QObject
{
    Q_OBJECT
    
public:
    DatabaseServiceTest();
    
private:
    DatabaseService m_dbSrv;
private Q_SLOTS:
    void initTestCase();
    void testCase1();
    void upgradeTest();
    void cleanupTestCase();
};

DatabaseServiceTest::DatabaseServiceTest()
{
}

void DatabaseServiceTest::initTestCase()
{
    if (!QSqlDatabase::drivers().contains("QSQLCIPHER"))
        QFAIL("SQLCIPHER Qt sql driver plugin not loaded!");

    QVERIFY(m_dbSrv.createDatabase("./dbservicetest.db", "123456"));
}

void DatabaseServiceTest::cleanupTestCase()
{
    QFile::remove("./dbservicetest.db");
}

void DatabaseServiceTest::testCase1()
{
    Database db("./dbservicetest.db", "123456", "conname");
    QVERIFY(db.isReady());
    QCOMPARE( m_dbSrv.dbVersion(&db), JAKILID_DB_VERSION_NUMBER);

    QVERIFY(!m_dbSrv.changeDatabasePassword(&db, "wrong pass", "new_pass"));
    db.close();
    QVERIFY(!m_dbSrv.changeDatabasePassword(&db, "wrong pass", "new_pass"));
    QVERIFY(m_dbSrv.changeDatabasePassword(&db, "123456", "new_pass"));
    QVERIFY(db.isReady());

    db.close();
    QVERIFY(db.open("new_pass"));
    QVERIFY(db.isReady());

}

void DatabaseServiceTest::upgradeTest()
{
    Database db("./dbservicetest.db", "new_pass", "conname");
    QVERIFY(db.isReady());

    QCOMPARE(m_dbSrv.upgradeDatabase(&db), 2);  //no upgrade required
    db.close();
    QCOMPARE(m_dbSrv.upgradeDatabase(&db), 0);  //failed to upgrade

    QCOMPARE(m_dbSrv.upgradeDatabase(0), 0);    //failed to upgrade
}

QTEST_APPLESS_MAIN(DatabaseServiceTest)

#include "tst_databaseservicetest.moc"
