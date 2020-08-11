#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

class Account;
class Database;
class QObject;
#include <QList>
#include <QUuid>
#include <QSqlQuery>

class AccountRepository
{
public:
    AccountRepository();
    AccountRepository(Database *db);

    Account*load(const QUuid &id, QObject *parent = 0 );
    QList<Account *> loadAll(QObject *parent = 0);
    QList<Account *> loadByUrl(const QString &url, QObject *parent = 0);
//  QList<Account*> load(Criteria crit);
    bool    save(Account *obj);
    bool    remove(Account *obj);
    bool    remove(const QUuid &id);
//  bool    remove(Criteria crit);
    bool    createTable();
    bool    upgradeScheme(qint32 from, qint32 to);

    bool    test();

    QStringList labels();

    void    setDatabase(Database *db);
    Database*getDatabase();
    bool    isValid() const;
private:
    Database*m_db;

    //bool    insert(Account *obj);
    //bool    update(Account *obj);
    bool    insertOrUpdate(const Account *obj);
    int     deleteRemovedInfo(Account *obj);

    QList<Account *> loadAllbyQuery(QSqlQuery &query, QObject *parent = 0);
};

#endif // ACCOUNTREPOSITORY_H
