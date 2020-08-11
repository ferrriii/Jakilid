#include <QString>
#include <QtTest>
#include <../../ui/Service/clientmanager.h>
#include <application/client.h>

class ClientManagerTest : public QObject
{
    Q_OBJECT

public:
    ClientManagerTest();

private Q_SLOTS:
    void testCase1();
};

ClientManagerTest::ClientManagerTest()
{
}

void ClientManagerTest::testCase1()
{
    ClientManager mngr;
    Client *c1 = mngr.newClient("test");
    Client *c2 = mngr.newClient("");
    QVERIFY(c1 != c2);

    Client *c11 = mngr.newClient("test");
    QVERIFY(c1 == c11);

    Client *c12 = mngr.client("test");
    QVERIFY(c1 == c12);

    Client *c22 = mngr.newClient("");
    QVERIFY(c2 == c22);

    Client *c23 = mngr.client("");
    QVERIFY(c2 == c23);

    QCOMPARE(mngr.clientId(c1), QString("test"));
    QCOMPARE(mngr.clientId(c2), QString(""));


    QSignalSpy c1spy(c1, SIGNAL(destroyed()) );
    QSignalSpy c2spy(c2, SIGNAL(destroyed()) );

    QCOMPARE(mngr.deleteClient("test"), 1);
    QCOMPARE(c1spy.count(), 0);
    QCOMPARE(mngr.deleteClient(c1), 0);
    QCOMPARE(c1spy.count(), 1);

    QCOMPARE(mngr.deleteClient(c2), 1);
    QCOMPARE(c2spy.count(), 0);
    QCOMPARE(mngr.deleteClient(""), 0);
    QCOMPARE(c2spy.count(), 1);

    QCOMPARE(c1spy.count(), 1);


    QCOMPARE(mngr.deleteClient("non_existing_id"), -1);
}

QTEST_APPLESS_MAIN(ClientManagerTest)

#include "tst_clientmanagertest.moc"
