#include <QString>
#include <QtTest>
#include <domain/profile.h>

class ProfileTest : public QObject
{
    Q_OBJECT
    
public:
    ProfileTest();
    
private Q_SLOTS:
    void testCase1();
    void variantTransform();
    void jsonTransform();
};

ProfileTest::ProfileTest()
{
}

void ProfileTest::testCase1()
{
    Profile profile;
    QCOMPARE(profile.isValid(), false);
    QCOMPARE(profile.lang() , QString("en-US"));
    profile.setName("name");
    profile.setDb("./db");
    profile.setTimeout(1000);
    QCOMPARE(profile.name(), QString("name"));
    QCOMPARE(profile.db(), QString("./db"));
    QCOMPARE(profile.timeout(), 1000);
    QCOMPARE(profile.isValid(), true);
    profile.setDb("");
    QCOMPARE(profile.isValid(), false); //a profile with no db is invalid

    Profile  profile2("name", "./db");
    profile2.setLang("fa-IR");
    QVERIFY(profile2.lang() == "fa-IR");
    QCOMPARE(profile2.name(), QString("name"));
    QCOMPARE(profile2.db(), QString("./db"));
    QVERIFY2(profile2.timeout() > 0, "defailt profile time out is 0!");
    QCOMPARE(profile2.isValid(), true);

}

void ProfileTest::variantTransform()
{
    Profile p1("profile name", "db");
    p1.setTimeout(444);
    Profile p2;

    QVERIFY(p2.fromVariant(p1.toVariant()));
    QVERIFY(p2.db() == p1.db());
    QVERIFY(p2.name() == p1.name());
    QVERIFY(p2.isValid() == p1.isValid());
    QVERIFY(p2.timeout() == p1.timeout());

}

void ProfileTest::jsonTransform()
{
    Profile p1("profile name", "db");
    p1.setTimeout(444);
    Profile p2;

    QVERIFY(p2.fromJson(p1.toJson()));
    QVERIFY(p2.db() == p1.db());
    QVERIFY(p2.name() == p1.name());
    QVERIFY(p2.isValid() == p1.isValid());
    QVERIFY(p2.timeout() == p1.timeout());
}

QTEST_APPLESS_MAIN(ProfileTest)

#include "tst_profiletest.moc"
