#include <QString>
#include <QtTest>
#include <domain/account.h>
#include <QDebug>

class AccountTest : public QObject
{
    Q_OBJECT

public:
    AccountTest();

private Q_SLOTS:
    void initTest();
    void addLabel();
    void setterGetters();
    void addInfo();
    void removeInfo();
    void changed();
    void variantTransform();
    void jsonTransform();
};

AccountTest::AccountTest()
{
}

void AccountTest::initTest()
{
    Account acc;
    QCOMPARE(acc.isValid(), false);
    QCOMPARE(acc.isSaved(), false);
    //QCOMPARE(acc.isPersisted(), false);
    QCOMPARE(acc.getCreatedDate(), QDate::currentDate());
    QCOMPARE(acc.getInfoPairs().count(), 0);
    QCOMPARE(acc.getTitle(), QString());
//    QCOMPARE(acc.getId(), -1);

}

void AccountTest::addLabel()
{
    Account acc;
    acc.setTitle("test");
    acc.addLabel("social");
    acc.addLabel("social");
    acc.addLabel("Work");
    acc.addLabel("bank");

    QCOMPARE(acc.getLabels().count(), 3);
    QVERIFY(acc.getLabels().contains("social", Qt::CaseInsensitive));
    QVERIFY(acc.getLabels().contains("bank", Qt::CaseInsensitive));
    QVERIFY(acc.getLabels().contains("work", Qt::CaseInsensitive));
}

void AccountTest::setterGetters()
{
    Account acc;

    QDate d(2010,1,1);
    acc.setCreatedDate(d);
    QVERIFY(acc.getCreatedDate() == d);
    QUuid id = QUuid::createUuid();
    acc.setId(id /*22*/);
    QVERIFY(acc.getId() == id /*22*/);
    acc.setTitle("title");
    QVERIFY(acc.getTitle() == "title");
    acc.setSaved(true);
    QVERIFY(acc.isSaved());
    acc.addLabel("abc");
    QVERIFY(acc.getLabels().count() == 1);
    QVERIFY(acc.getLabels().at(0) == "abc");
    QVERIFY(!acc.isSaved());
    acc.addLabel("test label");
    QVERIFY(acc.getLabels().count() == 2);
    QVERIFY(acc.getLabels().at(1) == "test label");
    acc.setLabels(QStringList() << "label");
    QVERIFY(acc.getLabels().count() == 1);
    QVERIFY(acc.getLabels().at(0) == "label");

}

void AccountTest::addInfo()
{
    Account acc;
    Info *info = acc.addInfo("test", "value");
    QVERIFY(info);
    QVERIFY(info->getAccount() == &acc);

    QVERIFY(acc.getInfoPairs().count() == 1);

    Info *i = acc.getInfoPairs().at(0);
    QVERIFY(i->getKey() == "test");
    QVERIFY(i->getValue() == "value");
//    QVERIFY(i->getId() == -1);
    //QVERIFY(!i->isPersisted());
    QVERIFY(i->getOrder() == 1);
    QVERIFY(i->getType() == Info::KeyTypeText);
    QVERIFY(i->getAccount() == &acc);

    Info i2;
    acc.addInfo(&i2);
    QVERIFY(acc.getInfoPairs().count() == 2);
    i = acc.getInfoPairs().at(1);
    QVERIFY(i == &i2);
    QVERIFY(i->getOrder() == 2);
    QVERIFY(i->getType() == Info::KeyTypeText);
    QVERIFY(i->getAccount() == &acc);


    Account a1;
    a1.setTitle("test account");
    Info *url = new Info(&a1);  //accout takes ownership of info
    url->setType(Info::KeyTypeURL);
    url->setKey("URL");
    url->setValue("https://mail.live.com/?login=2s");
    a1.addInfo(url);
}

void AccountTest::removeInfo()
{
    Account acc;
    Info *info = acc.addInfo("test", "value");

    QVERIFY(acc.getInfoPairs().count() == 1);

    acc.removeInfo(info);

    QVERIFY(acc.getInfoPairs().count() == 0);

}

void AccountTest::changed()
{
    Account acc;
    QCOMPARE(acc.isSaved(), false);
    //QCOMPARE(acc.isPersisted(), false);
    acc.setSaved(true);
    QCOMPARE(acc.isSaved(), true);
    acc.setTitle("a");
    QCOMPARE(acc.isSaved(), false);
    acc.setSaved(true);
    //acc.setId(33);
//    QCOMPARE(acc.isSaved(), false);
//    acc.setSaved(true);
    acc.addInfo("a","b");
    QCOMPARE(acc.isSaved(), false);     //whole account object with its info are not saved
    QCOMPARE(acc.isChanged(), true);    //add info doesn't change account itself
    acc.setCreatedDate(QDate(2022,5,5));
    QCOMPARE(acc.isSaved(), false);
}

void AccountTest::variantTransform()
{
    Account acc;
    //acc.setId(22);
    acc.setSaved(true);
    acc.setTitle("test acc");
    acc.setCreatedDate(QDate(2004,5,5));
    acc.addInfo("key", "value");

    Account acc2;
    QVERIFY(acc2.fromVariant(acc.toVariant()));
    QVERIFY(acc2.getCreatedDate() == acc.getCreatedDate());
    QCOMPARE(acc2.getId() , acc.getId());
    QVERIFY(acc2.getInfoPairs().count() == acc.getInfoPairs().count());
    QVERIFY(acc2.getInfoPairs().at(0)->getKey() == acc.getInfoPairs().at(0)->getKey());
    QVERIFY(acc2.getInfoPairs().at(0)->getValue() == acc.getInfoPairs().at(0)->getValue());
    QVERIFY(acc2.getInfoPairs().at(0)->getId() == acc.getInfoPairs().at(0)->getId());
    QVERIFY(acc2.getTitle() == acc.getTitle());
    QVERIFY(acc2.isSaved() == acc.isSaved());

}

void AccountTest::jsonTransform()
{
    Account acc;
    //acc.setId(22);
    acc.setSaved(true);
    acc.setTitle("test acc");
    acc.setCreatedDate(QDate(2004,5,5));
    acc.addInfo("key", "value");
    acc.addLabel("label");

    Account acc2;
    QVERIFY(acc2.fromJson(acc.toJson()));
    QVERIFY(acc2.getCreatedDate() == acc.getCreatedDate());
    QCOMPARE(acc2.getId() , acc.getId());
    QVERIFY(acc2.getInfoPairs().count() == acc.getInfoPairs().count());
    QVERIFY(acc2.getInfoPairs().at(0)->getKey() == acc.getInfoPairs().at(0)->getKey());
    QVERIFY(acc2.getInfoPairs().at(0)->getValue() == acc.getInfoPairs().at(0)->getValue());
    QVERIFY(acc2.getInfoPairs().at(0)->getId() == acc.getInfoPairs().at(0)->getId());
    QVERIFY(acc2.getTitle() == acc.getTitle());
    QVERIFY(acc2.isSaved() == acc.isSaved());
    QVERIFY(acc2.getLabels() == acc.getLabels());
}

QTEST_APPLESS_MAIN(AccountTest)

#include "tst_accounttest.moc"

