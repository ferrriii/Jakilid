#include <QString>
#include <QtTest>
#include <domain/info.h>
#include <domain/account.h>

class InfoTest : public QObject
{
    Q_OBJECT
    
public:
    InfoTest();
    
private Q_SLOTS:
    void testCase1();
    void variantTransform();
    void jsonTransform();
    void transformBenchmark();
};

InfoTest::InfoTest()
{
}

void InfoTest::testCase1()
{
    Info info;
//    QCOMPARE(info.getId(), -1);
    //QCOMPARE(info.isPersisted(), false);
    QCOMPARE(info.getOrder(), 0);
    QVERIFY(info.getAccount() == 0);
    QCOMPARE(info.isSaved(), false);
    QCOMPARE(info.isValid(), false);

    QUuid id = QUuid::createUuid();
    info.setId(id /*22*/);
    info.setKey("kEy");
    info.setOrder(2);
    info.setType(Info::KeyTypeEmail);
    info.setValue("Value");
    info.setFormInput("txtUserName");

    Account acc;
    info.setAccount(&acc);
    info.setSaved(true);

    QCOMPARE(info.getId(), id /*22*/);
    QCOMPARE(info.getKey(), QString("kEy"));
    QCOMPARE(info.getOrder(), 2);
    QCOMPARE(info.isSaved(), true);
    QCOMPARE(info.getType(), Info::KeyTypeEmail);
    QCOMPARE(info.getValue(), QString("Value"));
    QCOMPARE(info.getFormInput(), QString("txtUserName"));
    QCOMPARE(info.getAccount(), &acc);
}

void InfoTest::variantTransform()
{
    Info info;
//    info.setId(22);
    info.setKey("kEy");
    info.setOrder(2);
    info.setSaved(true);
    info.setType(Info::KeyTypeEmail);
    info.setValue("Value");
    info.setFormInput("txtUser");


    Info i2;
    QVERIFY(i2.fromVariant(info.toVariant()));
    QVERIFY(i2.getId() == info.getId());
    QVERIFY(i2.getKey() == info.getKey());
    QVERIFY(i2.getOrder() == info.getOrder());
    QVERIFY(i2.getType() == info.getType());
    QVERIFY(i2.getValue() == info.getValue());
    QVERIFY(i2.getFormInput() == info.getFormInput());
}

void InfoTest::jsonTransform()
{
    Info info;
//    info.setId(22);
    info.setKey("kEy");
    info.setOrder(2);
    info.setSaved(true);
    info.setType(Info::KeyTypeEmail);
    info.setValue("Value");
    info.setFormInput("txtUser");


    Info i2;
    QVERIFY(i2.fromJson(info.toJson()));
    QVERIFY(i2.getId() == info.getId());
    QVERIFY(i2.getKey() == info.getKey());
    QVERIFY(i2.getOrder() == info.getOrder());
    QVERIFY(i2.getType() == info.getType());
    QVERIFY(i2.getValue() == info.getValue());
    QVERIFY(i2.getFormInput() == info.getFormInput());
}

void InfoTest::transformBenchmark()
{
    Info info;
//    info.setId(22);
    info.setKey("kEy");
    info.setOrder(2);
    info.setSaved(true);
    info.setType(Info::KeyTypeEmail);
    info.setValue("Value");
    info.setFormInput("txtUser");

    QVariant variantInfo = info.toVariant();
    QJsonValue jsonInfo = info.toJson();
    Info info2;
    QVERIFY(info2.fromVariant(variantInfo));
    QVERIFY(info2.fromJson(jsonInfo));
    QElapsedTimer t;

    t.start();
    for (int i=0; i<10000; ++i)
        info.toVariant();
    qDebug() << "10000 toVariant (ms): "<< t.elapsed();


    t.restart();
    for (int i=0; i<10000; ++i)
        info.toJson();
    qDebug() << "10000 toJson (ms): "<< t.elapsed();


    t.restart();
    for (int i=0; i<10000; ++i)
        info2.fromVariant(variantInfo);
    qDebug() << "10000 fromVariant (ms): "<< t.elapsed();


    t.restart();
    for (int i=0; i<10000; ++i)
        info2.fromJson(jsonInfo);
    qDebug() << "10000 fromJson (ms): "<< t.elapsed();



}

QTEST_APPLESS_MAIN(InfoTest)

#include "tst_infotest.moc"
