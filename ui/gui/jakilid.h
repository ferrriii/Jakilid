#ifndef JAKILID_H
#define JAKILID_H

#include <QObject>
#include <application/sessionaccountservice.h>
#include <application/sessionservice.h>
class Account;
class Info;

class Jakilid : public QObject
{
    Q_OBJECT
public:
    Jakilid(QObject *parent = 0);
    ~Jakilid();

    Q_INVOKABLE bool    authenticate(const QString &profile, const QString &password);
    Q_INVOKABLE bool    isUnlocked() const;
    Q_INVOKABLE QString find(QString what = "", const QString &label="", int count = -1, int offset = 0);
    Q_INVOKABLE QString save(const QString &account);
    QString             save(const QJsonValue &account);
    Q_INVOKABLE QString load(const QString &id);
    Q_INVOKABLE bool    remove(const QString &id);
    Q_INVOKABLE QString getCurrentProfile();
    Q_INVOKABLE bool    setSettings(int timeout, const QString &lang);
    Q_INVOKABLE bool    changePass(const QString &oldPass, const QString &newPass);

public slots:
    void        lock();

private:
    SessionService m_sessionService;
    Session *m_session;

    SessionAccountService m_accountSessionService;


signals:
    void locked();
    void unLocked();
    
};

#endif // JAKILID_H
