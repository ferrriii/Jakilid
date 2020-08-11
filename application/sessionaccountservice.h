#ifndef SESSIONACCOUNTSERVICE_H
#define SESSIONACCOUNTSERVICE_H

#include <QString>
#include <dataaccess/accountrepository.h>
#include <domain/profile.h>

class Account;
class Session;
class Info;
class SessionAccountService
{
public:
    SessionAccountService();
    SessionAccountService(Session *session);
    QList<Account *>    find(const QString &what = "", const QString &label= "", int count = -1, int offset = 0, int *total = 0);
    QList<Account *>    findByUrl(const QString &url = "", int count = -1, int offset = 0, int *total = 0);

    QStringList     labels();

    bool        save(Account *account);
    Account    *load(const QUuid &id);
    bool        remove(const QUuid &id);
    Profile    *profile();

    void        setSession(Session *session);
    Session    *getSession();
    bool        isValid() const;
    bool        setProfileSettings(int timeout, const QString &lang);

    void        preventSessionLockout();

private:
    Info *findInfoById(QList<Info *> infoList, int id);
    Session *m_session;

    AccountRepository m_accountRespository;

    float calcAccountRank(Account *acc, const QString &what);
    float letterPairSimilarity(const QString &s1, const QString &s2);

signals:

};

#endif // SESSIONACCOUNTSERVICE_H
