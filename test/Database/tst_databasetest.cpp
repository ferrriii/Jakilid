#include <QSqlQuery>
#include <QString>
#include <QtTest>
#include <domain/database.h>
#include <application/databaseservice.h>

class DatabaseTest : public QObject
{
    Q_OBJECT

public:
    DatabaseTest();

private Q_SLOTS:
    void initTestCase();
//    void encryptDb();
    void multipleDb();
    void changeKey();
    void errorOpening();
    void cleanupTestCase();
};

DatabaseTest::DatabaseTest()
{
}

void DatabaseTest::initTestCase()
{
    if (!QSqlDatabase::drivers().contains("QSQLCIPHER"))
        QFAIL("SQLCIPHER Qt sql driver plugin not loaded!");

    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./test.db", "password"));
    QVERIFY(dbSrv.createDatabase("./db1.db", "pass"));
    QVERIFY(dbSrv.createDatabase("./db2.db", "pass"));
    QVERIFY(dbSrv.createDatabase("./t2.db","old_pass"));

    Database db;
    QCOMPARE(db.isReady(), false);
    QCOMPARE(db.open("./test.db", "password"), true);
    QCOMPARE(db.isReady(), true);
    QSqlQuery q = db.query("select 1");
    QCOMPARE(q.first(), true);
    QCOMPARE(q.value(0).toInt(), 1);
    q = db.query("create table foo(bar);");
	q = db.query("insert into foo values('test');");
	db.close();
    QFile f("./test.db");
    f.open(QIODevice::ReadOnly);
    QByteArray signature = f.read(6);
    f.close();
    QVERIFY2(signature != "SQLite", "Database is not encrypted");
    QCOMPARE(db.open("./test.db", "password"), true);
    q = db.query("select bar from foo;");
    QCOMPARE(q.first(), true);
    QCOMPARE(q.value(0).toString(), QString("test"));
    db.close();

    QElapsedTimer t;
    t.start();
    for (int i = 0; i<10; ++i)
    {
        db.open("./test.db", "password");
        db.close();
    }
    qDebug() << "db open and benchmark: " << t.elapsed()/10 ;

}

void DatabaseTest::cleanupTestCase()
{

//    QFile::remove("./t1.db");
    QFile::remove("./db1.db");
    QFile::remove("./db2.db");
    QFile::remove("./test.db");
    QFile::remove("./t2.db");
}

//void DatabaseTest::encryptDb()
//{
//    Database db;
//    QCOMPARE(db.open("./t1.db", "123456"), true);
//    db.query("create table foo(bar);");
//    db.close();

//    QCOMPARE(db.open("./t1.db", "wrong_pass"), false);
//    QCOMPARE(db.isReady(), false);
//    QCOMPARE(db.open("./t1.db", "123456") , true);
//    QCOMPARE(db.isReady(), true);
//}

void DatabaseTest::multipleDb()
{
    Database db1("db1");
    Database db2("db2");

    db1.open("./db1.db","pass");
    db2.open("./db2.db", "pass");

    db1.query("create table db1(bar)");
    db2.query("create table db2(bar)");

    db1.query("insert into db1(bar) values('db1')");
    db2.query("insert into db2(bar) values('db2')");

    QSqlQuery q;
    q = db1.query("select bar from db1");
    QVERIFY(q.first());
    QVERIFY(q.value(0).toString() == "db1");

    q = db1.query("select bar from db2");
    QCOMPARE(q.first() , false);

    q = db2.query("select bar from db2");
    QVERIFY(q.first());
    QVERIFY(q.value(0).toString() == "db2");

    q = db2.query("select bar from db1");
    QCOMPARE(q.first() , false);
}

void DatabaseTest::changeKey()
{
    Database db;
    QCOMPARE(db.open/*AndEncrypt*/("./t2.db","old_pass"), true);
    db.query("create table foo(bar)");
    db.query("insert into foo values('abc')");
    db.close();
    QCOMPARE(db.isReady(), false);

    Database d("test");
    QCOMPARE(d.open("./t2.db", "old_pass"), true);
    QCOMPARE(d.changeKey("new_pass") , true);
    QCOMPARE(d.isReady(), true);    
    d.close();
    QCOMPARE(db.isReady(), false);

    Database d2("test2");
    QCOMPARE(d2.open("./t2.db", "new_pass"), true);
    QSqlQuery q = d2.query("select bar from foo");
    QVERIFY(q.first());
    QVERIFY(q.value(0).toString() == "abc");
    QCOMPARE(d2.changeKey("new_2_pass"), false);    //sqlcipher rule: changekey must be after openning the db
    d2.close();

    QCOMPARE(d2.open("./t2.db", "new_pass"), true);
    QVERIFY(d2.changeKey("new_2_pass"));
    q = d2.query("select bar from foo");
    QVERIFY(q.first());
    QVERIFY(q.value(0).toString() == "abc");



}

void DatabaseTest::errorOpening()
{
    Database db;
    QCOMPARE(db.open/*AndEncrypt*/("x:/path_file:access?error", "pass"), false);
    QCOMPARE(db.isReady(), false);
}

QTEST_APPLESS_MAIN(DatabaseTest)

#include "tst_databasetest.moc"
