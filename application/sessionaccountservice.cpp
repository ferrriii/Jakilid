#include "sessionaccountservice.h"
#include <domain/session.h>
#include <domain/account.h>
#include <domain/database.h>
#include <profileservice.h>
#include <QStringList>
#include <QUrl>
#include <QDebug>


SessionAccountService::SessionAccountService(): m_session(0)
{

}

SessionAccountService::SessionAccountService(Session *session): m_session(0)
{
    setSession(session);
}

QList<Account *> SessionAccountService::find(const QString &what, const QString &label, int count, int offset, int *total)
{
    if (!isValid())
        return QList<Account *>();   //invalid session
    if (!m_session->isUnlocked())
        return QList<Account *>();   //session locked

    preventSessionLockout();
    QList<Account *> allAcc = m_accountRespository.loadAll();
    if (!what.isEmpty())
    {
        //calculate each account's rank relative to query (what)
        for (int i = 0; i<allAcc.size(); ++i)
        {
            if (calcAccountRank(allAcc.at(i), what) <= 0)
            {
                //remove items with no similarity
                delete allAcc.takeAt(i);
                --i;
            }
        }
        //sort by rank
        qSort(allAcc.begin(), allAcc.end(),
              [](const Account *a1, const Account *a2) -> bool {
            return a1->property("r").toFloat() > a2->property("r").toFloat();
        }
        );
        //remove irelevant results (rand < 0.2 max rank)
        float thresholdRank = 0;
        if (allAcc.count() > 0)
            thresholdRank = allAcc.at(0)->property("r").toFloat() * 0.5;
        thresholdRank = qMax(thresholdRank, 80.0f);   //remove results with less than 80% similarity (at least 80%)
        int thresholdIndex = -1;
        for (int i = 0; i<allAcc.size(); ++i)
        {
            if (allAcc.at(i)->property("r").toFloat() < thresholdRank )
            {
                thresholdIndex = i;
                break;
            }
        }
        if (thresholdIndex > -1)        //remove all results which their rank is below threshold (from thresholdIndex to end, because results is sorted desc)
            for (; allAcc.size() > thresholdIndex; )
                delete allAcc.takeLast();
    }
    //BUG: passing direct allAcc! save(acc1); save(acc2) but get acc2 then acc1 when loading because repository sort by acount id which is random!

    //remove items which don't contain label
    if (!label.isEmpty())
    {
        for (int i = 0; i<allAcc.size(); ++i)
        {
            if (!allAcc.at(i)->getLabels().contains(label, Qt::CaseInsensitive))
            {
                delete allAcc.takeAt(i);
                --i;
            }
        }
    }

    if (total)
        *total = allAcc.count();
    //chop allAcc according to count and offset
    if (offset > 0)
        for ( int i = 0; i != offset && allAcc.count() >0; ++i)
            delete allAcc.takeFirst();
    if (count > -1)
        for (; allAcc.size() > count; )
            delete allAcc.takeLast();

    return allAcc;
}

QList<Account *> SessionAccountService::findByUrl(const QString &url, int count, int offset, int *total)
{
    if (!isValid())
        return QList<Account *>();   //invalid session
    if (!m_session->isUnlocked())
        return QList<Account *>();   //session locked

    preventSessionLockout();
    //NOTE: if we had a powerfull repository we could load items with matching url using a criteria
    QList<Account *> allAcc = m_accountRespository.loadByUrl(QUrl(url).host());

    if (total)
        *total = allAcc.count();
    //chop allAcc according to count and offset
    if (offset > 0)
        for ( int i = 0; i != offset && allAcc.count() >0; ++i)
            delete allAcc.takeFirst();
    if (count > -1)
        for (; allAcc.size() > count; )
            delete allAcc.takeLast();

    return allAcc;
}

QStringList SessionAccountService::labels()
{
    if (!isValid())
        return QStringList();   //invalid session
    if (!m_session->isUnlocked())
        return QStringList();   //session locked

    preventSessionLockout();

    return m_accountRespository.labels();
}

bool SessionAccountService::save(Account *account)
{
    if (!isValid())
        return false;   //invalid session
    if (!m_session->isUnlocked())
        return false;   //session locked
    if (!account->isValid())
        return false;   //invalid account

    preventSessionLockout();
    QSqlDatabase *db = m_accountRespository.getDatabase()->databse();
    db->transaction();
    bool ret = m_accountRespository.save(account);
    if (ret)
    {
        return db->commit();
    }
    else
        db->rollback();

    return false;
}

Account *SessionAccountService::load(const QUuid &id)
{
    if (!isValid())
        return 0;   //invalid session
    if (!m_session->isUnlocked())
        return 0;   //session locked
    preventSessionLockout();

    Account *acc = 0;

    QSqlDatabase *db = m_accountRespository.getDatabase()->databse();
    db->transaction();
    //WARNING: while synchronous = OFF speeds up but there's a chance that the database file may become corrupted if the computer suffers a catastrophic crash (or power failure) before the data is written to the platter:
    //m_accountRespository.getDatabase()->query("PRAGMA synchronous = OFF");
    acc = m_accountRespository.load( id );
    if (acc)
        db->commit();
    else
        db->rollback();

    //m_accountRespository.getDatabase()->query("PRAGMA synchronous = ON");
    return acc;
}

bool SessionAccountService::remove(const QUuid &id)
{
    if (!isValid())
        return false;   //invalid session
    if (!m_session->isUnlocked())
        return false;   //session locked

    preventSessionLockout();
    QSqlDatabase *db = m_accountRespository.getDatabase()->databse();
    db->transaction();
    bool ret = m_accountRespository.remove( id );
    if (ret)
        return db->commit();
    else
        db->rollback();

    return false;
}

Profile *SessionAccountService::profile()
{
    if (!isValid())
        return 0;   //invalid session
    if (!m_session->isUnlocked())
        return 0;   //session locked

    preventSessionLockout();
    return m_session->profile();
}

void SessionAccountService::setSession(Session *session)
{
    m_session = session;
    if (m_session)
        m_accountRespository.setDatabase(m_session->database());
}

Session *SessionAccountService::getSession()
{
    return m_session;
}

bool SessionAccountService::isValid() const
{
    return m_session && m_session->isValid() && m_accountRespository.isValid();
}

bool SessionAccountService::setProfileSettings(int timeout, const QString &lang)
{
    if (!isValid())
        return false;   //invalid session
    if (!m_session->isUnlocked())
        return false;   //session locked

    Profile *prof = m_session->profile();
    prof->setTimeout(timeout);
    prof->setLang(lang);
    preventSessionLockout();    //calling this after setting timeout will cause session's lockout timer to be reset
    ProfileRepository profileRepo;
    return profileRepo.save(m_session->profile());
}

void SessionAccountService::preventSessionLockout()
{
    if (isValid())
        m_session->resetLockTimer();
}

float SessionAccountService::calcAccountRank(Account *acc, const QString &what)
{
    QString title= acc->getTitle();
    float r = letterPairSimilarity(title, what) * 2.0f;
    r += title.contains(what) ? 80+(float)((float)400 * what.length())/title.length() : 0; //add extra points if title contains query
    r += QString::localeAwareCompare(title, what/*, Qt::CaseInsensitive*/) == 0 ? 200 : 0; //plus extra point if title is equal to query
    QList<Info *> infoPairs = acc->getInfoPairs();
    QString value;
    foreach(Info *info, infoPairs)
    {
        value = info->getValue();
        r += letterPairSimilarity(value, what);
        r += value.contains(what) ? 75 + (float)((float)300 * what.length())/value.length() : 0; //add extra point is value contains query
        r += QString::compare(value, what, Qt::CaseInsensitive) == 0 ? 100 : 0; //add extra points if value is equal to query
    }

    //r += acc->getUsage();
    acc->setProperty("r", r);
    return r;
}

/* reference:http://www.catalysoft.com/articles/StrikeAMatch.html with some change*/
float SessionAccountService::letterPairSimilarity(const QString &s1, const QString &s2)
{
    QString s1_simple = s1.toLower().simplified();
    QString s2_simple = s2.toLower().simplified();

    QList<QString> s1_pairs;
    QList<QString> s2_pairs;


    QStringList s1Words=s1_simple.split(QRegExp(QStringLiteral("(\\s|\\,)")), QString::SkipEmptyParts);
    foreach (QString word, s1Words)
    {
        int s1PairsCount = word.length()-1; //BUG: remove -1 if you want to avoid 100% similarity bug for (vitamin a, vitamin b)

        for (int i=0; i< s1PairsCount; ++i)
            s1_pairs.append(word.mid(i,2));
    }

    QStringList s2Words = s2_simple.split(QRegExp(QStringLiteral("(\\s|\\,)")), QString::SkipEmptyParts);
    foreach (QString word, s2Words)
    {
        int s2PairsCount = word.length()-1; //BUG: remove -1 if you want to avoid 100% similarity bug for (vitamin a, vitamin b)

        for (int i=0; i< s2PairsCount; ++i)
            s2_pairs.append(word.mid(i,2));
    }

    int match = 0;

    foreach (QString pair, s1_pairs)
        if (s2_pairs.contains(pair))
            ++match;

    return 200.0f * match / (s1_pairs.count() + s2_pairs.count());
}
