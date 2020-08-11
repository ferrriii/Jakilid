#include <QString>
#include <QtTest>
#include <dataaccess/profilerepository.h>
#include <domain/profile.h>
#include <QDebug>

class ProfileRepositoryTest : public QObject
{
    Q_OBJECT
    
public:
    ProfileRepositoryTest();
    
private:
private Q_SLOTS:
    void testCase1();
};

ProfileRepositoryTest::ProfileRepositoryTest()
{
}

void ProfileRepositoryTest::testCase1()
{
    //required by profile repository
    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain("unittest");
    QCoreApplication::setApplicationName("profilerepositorytester");

    ProfileRepository profileRepo;
    QCOMPARE(profileRepo.doesProfileExist("some_not_existing_profile_name"), false);
    int cnt = profileRepo.allProfileNames().count();

    Profile profile("some_not_existing_profile_name", "db");
    profile.setLang("fa-IR");

    QVERIFY(profileRepo.save(&profile));
    QCOMPARE(profileRepo.allProfileNames().count(), cnt+1);

    Profile *p2 = profileRepo.loadByName("some_not_existing_profile_name");

    QVERIFY(p2->isValid() == profile.isValid());
    QVERIFY(p2->db() == profile.db());
    QVERIFY(p2->lang() == profile.lang());
    QVERIFY(p2->name() == profile.name());

    QVERIFY(profileRepo.removeByName("some_not_existing_profile_name"));
    QCOMPARE(profileRepo.allProfileNames().count(), cnt);

    delete p2;
    p2 = profileRepo.loadByName("some_not_existing_profile_name");
    QVERIFY(p2 == 0);
}

QTEST_MAIN(ProfileRepositoryTest)

#include "tst_profilerepositorytest.moc"
