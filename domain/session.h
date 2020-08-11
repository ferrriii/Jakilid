#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QTimer>
#include "profile.h"
class Database;

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(Profile *profile, QObject *parent = 0);
    ~Session();

    Profile *profile() const;
    void setProfile(Profile *profile);

    Database *database();


    bool isValid() const;
    bool isUnlocked() const;

private:
    Database *m_dbConnection;
    Profile *m_profile;
    QTimer *m_lockTimer;

signals:
    void locked();
    void unlocked();

public slots:
    bool unlock(const QString &password);
    void lock();
    void resetLockTimer();

};

#endif // SESSION_H
