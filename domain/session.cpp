#include "session.h"
#include "database.h"


Session::Session(Profile *profile, QObject *parent) : QObject(parent), m_dbConnection(0), m_profile(0)
{
    m_lockTimer = new QTimer(this);
    connect(this, SIGNAL(unlocked()), m_lockTimer, SLOT(start()));
    connect(m_lockTimer, SIGNAL(timeout()), this, SLOT(lock()));
    connect(this, SIGNAL(locked()), m_lockTimer, SLOT(stop()));

    setProfile(profile);
}

Session::~Session()
{
    delete m_lockTimer;
    if (m_dbConnection)
        delete m_dbConnection;
    if (m_profile)
        delete m_profile;
}

Profile *Session::profile() const
{
    return m_profile;
}

void Session::setProfile(Profile *profile)
{   
//    if (!profile->isValid())
//        return; //false

    lock();
    if (m_dbConnection) //delete old connection (if any)
        delete m_dbConnection;
    if (m_profile)      //delete old profile (if any)
        delete m_profile;
    m_profile = profile;
    if (m_profile)
    {
        m_dbConnection = new Database(QString::number(qrand())/*m_profile->name()*/);
        m_lockTimer->setInterval(m_profile->timeout() * 1000);
    }
    else
        m_dbConnection = 0;
}

Database *Session::database()
{
    return m_dbConnection;
}

bool Session::isValid() const
{
    return m_profile && m_profile->isValid() && m_dbConnection != 0;
}

bool Session::unlock(const QString &password)
{
    if (!isValid())
        return false;

    if (m_dbConnection->isReady())
        return true;

    if (m_dbConnection->open(m_profile->db(), password))
    {
        emit unlocked();
        return true;
    }else
        return false;
}

void Session::lock()
{
    if (!isValid())
        return;
    if (!m_dbConnection->isReady())
        return;
    m_dbConnection->close();
    emit locked();
}


bool Session::isUnlocked() const
{
    if (!isValid())
        return false;

    return m_dbConnection->isReady();
}

void Session::resetLockTimer()
{
    m_lockTimer->stop();
    if (isUnlocked())
    {
        m_lockTimer->setInterval(m_profile->timeout() * 1000);
        m_lockTimer->start();
    }
}

