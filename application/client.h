#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "sessionaccountservice.h"
#include "sessionservice.h"
class Account;
class Info;

class Client : public QObject
{
    Q_OBJECT
public:
    Client(QObject *parent = 0);
    ~Client();

    bool    authenticate(const QString &profile, const QString &password);
    bool    isUnlocked() const;
    QJsonObject find(QString what = "", const QString &label="", int count = -1, int offset = 0);
    QJsonObject findByUrl(QString url = "", int count = -1, int offset = 0);
    QString save(const QJsonValue &account);
    QJsonObject load(const QString &id);
    bool    remove(const QString &id);
    QJsonObject getCurrentProfile();
    bool    setSettings(int timeout, const QString &lang);
    bool    changePass(const QString &oldPass, const QString &newPass);
    QJsonArray labels();
    int     upgradeDb();


public slots:
    void        lock();
    void        preventLock();

private:
    SessionService m_sessionService;
    Session *m_session;

    SessionAccountService m_accountSessionService;


signals:
    void locked();
    void unLocked();
    
};

#endif // CLIENT_H
