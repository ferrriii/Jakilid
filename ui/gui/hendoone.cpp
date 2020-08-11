#include "hendoone.h"
#include <domain/account.h>
#include <application/databaseservice.h>
#include <QJsonObject>
#include <QJsonArray>

Hendoone::Hendoone(QObject *parent) :
    QObject(parent), m_session(0)
{
}

Hendoone::~Hendoone()
{
    if (m_session)
        delete m_session;
}



void Hendoone::lock()
{
    if (m_session)
        m_session->lock();
}


bool Hendoone::authenticate(const QString &profile, const QString &password)
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
        return m_session->unlock(password);
    else
        return false;   //requested profile does not exist!
}

bool Hendoone::isUnlocked() const
{
    if (!m_session)
        return false;
    return m_session->isUnlocked();
}

QString Hendoone::find(QString what, const QString &label, int count, int offset)
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

    return QString::fromUtf8(QJsonDocument(queryResultsObj).toJson(QJsonDocument::Compact));
}


QString Hendoone::load(const QString &id)
{
    Account *acc = m_accountSessionService.load(id);
    if (acc)
    {
        QString ac = QString::fromUtf8( QJsonDocument(acc->toJson().toObject()).toJson(QJsonDocument::Compact) );
        delete acc;
        return ac;
    }
    else
        return QString();
}

bool Hendoone::remove(const QString &id)
{
    return m_accountSessionService.remove(id);
}


QString Hendoone::getCurrentProfile()
{
    Profile *profile =  m_accountSessionService.profile();
    if (profile && profile->isValid())
        return QString::fromUtf8( QJsonDocument(profile->toJson().toObject()).toJson(QJsonDocument::Compact) ); //profile->toVariant();
    else
        return QString();
}

bool Hendoone::setSettings(int timeout, const QString &lang)
{
    return m_accountSessionService.setProfileSettings(timeout, lang);
}


bool Hendoone::changePass(const QString &oldPass, const QString &newPass)
{
    if (!m_session)
        return false;
    DatabaseService dbSrv;
    return dbSrv.changeDatabasePassword(m_session->database(), oldPass, newPass);
}



QString Hendoone::save(const QString &account)
{
    return save( QJsonDocument::fromJson(account.toUtf8()).object() );
//    if (!isUnlocked())
//        return 0;

//    QScopedPointer<Account> acc(new Account(this));
//    if (!acc.data()->fromJson( QJsonDocument::fromJson(account.toUtf8()).object() ))
//        return QString();

//    if (m_accountSessionService.save(acc.data()))
//        return acc.data()->getId().toString();
//    else
//        return QString();
}

QString Hendoone::save(const QJsonValue &account)
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
