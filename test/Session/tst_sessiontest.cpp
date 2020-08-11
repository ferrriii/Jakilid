#include <QString>
#include <QtTest>
#include <domain/session.h>
#include <domain/database.h>
#include <domain/profile.h>
#include <application/databaseservice.h>

class SessionTest : public QObject
{
    Q_OBJECT
    
public:
    SessionTest();
    
private Q_SLOTS:
    void testCase1();
    void cleanup();
};

SessionTest::SessionTest()
{
}

void SessionTest::testCase1()
{
//    cleanup();
    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./SessionTest.db", "123456"));

    Profile *profile = new Profile("testProfile", "./SessionTest.db");
    QVERIFY(profile->isValid());
    QCOMPARE(profile->db(), QString("./SessionTest.db"));

    Session session(profile);   //session takes ownership of profile
    QSignalSpy signalLocked(&session, SIGNAL(locked()));
    QSignalSpy signalUnlocked(&session, SIGNAL(unlocked()));
    QCOMPARE(session.isUnlocked(), false);
    QCOMPARE(session.isValid(), true);
    QCOMPARE(session.database()->isReady(), false);

    //unlock
    QCOMPARE(session.unlock("123456"), true);
    QCOMPARE(signalLocked.count(), 0);
    QCOMPARE(signalUnlocked.count(), 1);
    QCOMPARE(session.database()->isReady(), true);
    //unlock again
    QCOMPARE(session.unlock("123456"), true);
    QCOMPARE(signalLocked.count(), 0);
    QCOMPARE(signalUnlocked.count(), 1);
    QCOMPARE(session.database()->isReady(), true);
    //lock
    session.lock();
    QCOMPARE(signalLocked.count(), 1);
    QCOMPARE(signalUnlocked.count(), 1);
    QCOMPARE(session.database()->isReady(), false);
    //lock again
    session.lock();
    QCOMPARE(signalLocked.count(), 1);
    QCOMPARE(signalUnlocked.count(), 1);
    //unlock after lock
    QCOMPARE(session.unlock("123456"), true);
    QCOMPARE(signalLocked.count(), 1);
    QCOMPARE(signalUnlocked.count(), 2);
    QCOMPARE(session.database()->isReady(), true);
    //change profile
    Profile *profile2 = new Profile("testProfile2", "./testprofile2.db");
    session.setProfile(profile2);   //session takes ownership of profile
    QCOMPARE(signalLocked.count(), 2);  //lock signal should be emitted
    QCOMPARE(signalUnlocked.count(), 2);
    QCOMPARE(session.isUnlocked(), false);
    QCOMPARE(session.isValid(), true);
    QCOMPARE(session.database()->isReady(), false);
    //invalid profile
    Profile *invalidProfile = new Profile();
    session.setProfile(invalidProfile); //session takes ownership of profile
    QCOMPARE(signalLocked.count(), 2);  //lock signal should be emitted
    QCOMPARE(signalUnlocked.count(), 2);
    QCOMPARE(session.isUnlocked(), false);
    QCOMPARE(session.isValid(), false);
    QCOMPARE(session.database()->isReady(), false);
}

void SessionTest::cleanup()
{
    QFile::remove("./SessionTest.db");
}

QTEST_MAIN(SessionTest)

#include "tst_sessiontest.moc"
