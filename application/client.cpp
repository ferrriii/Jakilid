#include "client.h"
#include <domain/account.h>
#include "databaseservice.h"
#include <QJsonObject>
#include <QJsonArray>

Client::Client(QObject *parent) :
    QObject(parent), m_session(0)
{
}

Client::~Client()
{
    if (m_session)
        delete m_session;
}



void Client::lock()
{
    if (m_session)
        m_session->lock();
}

void Client::preventLock()
{
//    m_accountSessionService.preventSessionLockout();
    if (m_session)
        m_session->resetLockTimer();
}


bool Client::authenticate(const QString &profile, const QString &password)
{
    //NOTE: we only have one active session so when unlocking new session, the old one
    //will get deleted and replaced. exception: if requested profile is same as old session's
    //profile do not create new session and replace old session for performance. this is common
    //due to session's auto lock mechanism
    if (m_session && m_session->profile() && m_session->profile()->name() != profile)
    {
        //new profile, delete old (if any)
        Session *session = m_sessionService.createSession(profile);
        if (session)
        {
            //profile unlocked and replaced with old session
            delete m_session;
            m_session = session;
            connect(m_session, SIGNAL(locked()), this, SIGNAL(locked()));
            connect(m_session, SIGNAL(unlocked()), this, SIGNAL(unLocked()));
            m_accountSessionService.setSession(m_session);
        }else
            return false;   //requested profile does not exist!
    }
    else if (!m_session)
    {
        //no session has been created yet, let's create one
        m_session = m_sessionService.createSession(profile);
        if (m_session)
        {
            connect(m_session, SIGNAL(locked()), this, SIGNAL(locked()));
            connect(m_session, SIGNAL(unlocked()), this, SIGNAL(unLocked()));
            m_accountSessionService.setSession(m_session);
        }
    }

    if (m_session)
        return m_session->unlock(password); //false: invalid password
    else
        return false;   //requested profile does not exist!
}

bool Client::isUnlocked() const
{
    if (!m_session)
        return false;
    return m_session->isUnlocked();
}

QJsonObject Client::find(QString what, const QString &label, int count, int offset)
{
    int total = 0;
    QList<Account *> accounts =  m_accountSessionService.find(what, label, count, offset, &total);
    QJsonArray accList;
    foreach (Account *acc, accounts)
        accList.append(acc->toJson());

    QJsonObject queryResultsObj;
    queryResultsObj.insert(QStringLiteral("total"), total);
    queryResultsObj.insert(QStringLiteral("items"), accList);

    qDeleteAll(accounts);

    return queryResultsObj;
}

QJsonObject Client::findByUrl(QString url, int count, int offset)
{
    int total = 0;
    QList<Account *> accounts =  m_accountSessionService.findByUrl(url, count, offset, &total);
    QJsonArray accList;
    foreach (Account *acc, accounts)
        accList.append(acc->toJson());

    QJsonObject queryResultsObj;
    queryResultsObj.insert(QStringLiteral("total"), total);
    queryResultsObj.insert(QStringLiteral("items"), accList);

    qDeleteAll(accounts);

    return queryResultsObj;
}


QJsonObject Client::load(const QString &id)
{
    Account *acc = m_accountSessionService.load(id);
    if (acc)
    {
        QJsonObject ac = acc->toJson().toObject();
        delete acc;
        return ac;
    }
    else
        return QJsonObject();
}

bool Client::remove(const QString &id)
{
    return m_accountSessionService.remove(id);
}


QJsonObject Client::getCurrentProfile()
{
    Profile *profile =  m_accountSessionService.profile();
    if (profile && profile->isValid())
        return profile->toJson().toObject();
    else
        return QJsonObject();
}

bool Client::setSettings(int timeout, const QString &lang)
{
    return m_accountSessionService.setProfileSettings(timeout, lang);
}


bool Client::changePass(const QString &oldPass, const QString &newPass)
{
    if (!m_session)
        return false;
    DatabaseService dbSrv;
    return dbSrv.changeDatabasePassword(m_session->database(), oldPass, newPass);
}

QJsonArray Client::labels()
{
    if (!m_session)
        return QJsonArray();
    return QJsonArray::fromStringList(m_accountSessionService.labels());
}

int Client::upgradeDb()
{
    if (!m_session)
        return 0;
    DatabaseService dbSrv;
    return dbSrv.upgradeDatabase(m_session->database());
}




QString Client::save(const QJsonValue &account)
{
    if (!isUnlocked())
        return 0;

    Account acc;
    if (!acc.fromJson(account ) )
        return QString();

    if (m_accountSessionService.save(&acc))
        return acc.getId().toString();
    else
        return QString();
}
