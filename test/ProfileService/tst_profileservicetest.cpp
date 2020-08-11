#include <QString>
#include <QtTest>
#include <application/profileservice.h>
#include <application/sessionservice.h>
#include <dataaccess/accountrepository.h>
#include <application/databaseservice.h>
#include <domain/account.h>
#include <domain/database.h>

class ProfileServiceTest : public QObject
{
    Q_OBJECT

public:
    ProfileServiceTest();


private Q_SLOTS:
    void testCase1();
    void sessionServiceTest();
    void cleanUp();
};

ProfileServiceTest::ProfileServiceTest()
{
}

void ProfileServiceTest::testCase1()
{
    //required by profile repository
    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain("unittest");
    QCoreApplication::setApplicationName("profilerepositoryservicetester");

    DatabaseService dbSrv;
    QVERIFY(dbSrv.createDatabase("./testProfileService.db", "123456")); //db should exists before creating profile
    ProfileService profileService;
    QCOMPARE(profileService.allProfileNames().count(), 0);
    Profile profile("testProfile", "./testProfileService.db");
    QVERIFY(profileService.addProfile(&profile, "123456"));
    QCOMPARE(profileService.allProfileNames().count(), 1);
    QCOMPARE(profileService.allProfileNames().at(0), QString("testProfile"));

    //use db
    Database db("testProfile");
    QVERIFY(db.open("./testProfileService.db", "123456"));
    AccountRepository accountRepo(&db);

    Account acc;
    acc.setTitle("test");
    QVERIFY(accountRepo.save(&acc));

    Account *acc2 = accountRepo.load(acc.getId());
    QVERIFY(acc2 != 0);
    QCOMPARE(acc2->getTitle(), acc.getTitle());
}

void ProfileServiceTest::sessionServiceTest()
{
    SessionService sessionService;
    Session *session;

    session = sessionService.createSession("not_existing_profile");
    QVERIFY(session == 0);

    session = sessionService.createSession("testProfile");
    QVERIFY(session != 0);
    QVERIFY(session->isValid());
    QVERIFY(!session->isUnlocked());
    QVERIFY(session->unlock("123456"));
    QCOMPARE(session->profile()->db(), QString("./testProfileService.db"));
    delete session;

}

void ProfileServiceTest::cleanUp()
{
    ProfileService profileService;
    QVERIFY(profileService.removeProfile("testProfile"));
    QCOMPARE(profileService.allProfileNames().count(), 0);
    QFile::remove("./testProfileService.db");
}

QTEST_MAIN(ProfileServiceTest)

#include "tst_profileservicetest.moc"
