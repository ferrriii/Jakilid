#include "accountrepository.h"
#include <domain/database.h>
#include <domain/account.h>
#include <domain/info.h>
#include "inforepository.h"
#include <QSqlQuery>
#include <QSqlError>

AccountRepository::AccountRepository() : m_db(0)
{
}

AccountRepository::AccountRepository(Database *db) : m_db(0)
{
     setDatabase(db);
}

Account *AccountRepository::load(const QUuid &id, QObject *parent)
{
    QString hexId = id.toString();
    QSqlQuery query = m_db->query(QStringLiteral("select [Account].ID, [Account].Title, [Account].Labels, [Account].[Created], [i].[ID] as iid, [i].[type] as typ, [i].[Key] as [key], [i].[Value] as [value], [i].[order] as ord, [i].[FormInput] from Account left join [Info] i on [Account].[ID] = [i].[Account] where [Account].[ID] = ? order by ord"), QList<QVariant>() << hexId);
    if (!query.first())
        return 0;


    Account *acc = new Account(parent);
    acc->setId(id);
    acc->setTitle(query.value(1).toString());
    acc->setLabels(query.value(2).toString().split(',', QString::SkipEmptyParts));
    acc->setCreatedDate(QDateTime::fromTime_t(query.value(3).toInt()).toLocalTime().date());    //TODO: instead of toLocalTime add time zone selection feature

    do
    {
        if (query.value(4).toString().isEmpty())
            continue;
        Info *info = new Info(acc);
        info->setId( query.value(4).toString() );
        info->setKey(query.value(6).toString());
        info->setValue(query.value(7).toString());
        info->setOrder(query.value(8).toInt());
        info->setFormInput(query.value(9).toString());
        info->setType((Info::KeyType) query.value(5).toInt());
        acc->addInfo(info);
        info->setSaved(true);
    }
    while (query.next());

    acc->setSaved(true);
    //WARNING: update, slow downs load!
    m_db->query(QStringLiteral("UPDATE Account set [Accessed] = strftime('%s', 'now'), [Usage]=[Usage]+1 WHERE [ID]=?"), QList<QVariant>() << hexId);
    return acc;
}

QList<Account *> AccountRepository::loadAll(QObject *parent)
{
    //ordering by id will sort all info for specific account next to each other BUT:
    //BUG: account id is not proper for ordering because id is random and could cause this: save(acc1), save(acc2) but get acc2 then acc1 when loading
    QSqlQuery query = m_db->query(QStringLiteral("select [Account].ID, [Account].Title, [Account].Labels, [Account].[Created], [i].[ID] as iid, [i].[type] as typ, [i].[Key] as [key], [i].[Value] as [value], [i].[order] as ord, [i].[FormInput] from Account left join [Info] i on [Account].[ID] = [i].[Account] order by [Account].[Accessed] DESC, [Account].[Usage] DESC, [Account].[Modified] DESC, [Account].[ID], ord"));

    return loadAllbyQuery(query, parent);
}

QList<Account *> AccountRepository::loadByUrl(const QString &url, QObject *parent)
{
    if (url.isEmpty())
        return QList<Account *>();

    //NOTE: order by most used accounts first instead of recent accounts
    QSqlQuery query = m_db->query(QStringLiteral("select [Account].ID, [Account].Title, [Account].Labels, [Account].[Created], [i].[ID] as iid, [i].[type] as typ, [i].[Key] as [key], [i].[Value] as [value], [i].[order] as ord, [i].[FormInput] from Account left join [Info] i on [Account].[ID] = [i].[Account] WHERE [Account].[ID] in (select distinct [Info].[Account] from [Info] where [Info].[Type] = ? and [Info].[Value] LIKE ? ) order by [Account].[Usage] DESC, [Account].[ID], ord"),
                                 QVariantList() << Info::KeyTypeURL << QString(url).prepend('%').append('%'));								 
    return loadAllbyQuery(query, parent);
}

bool AccountRepository::save(Account *obj)
{
    if (!obj->isValid())
        return false;
    if (!obj->isChanged())
    {
        if (!insertOrUpdate(obj))
            return false;
    }
    if (deleteRemovedInfo(obj) <= -1)
        return false;

    InfoRepository infoRepository(m_db);
    QList<Info *> infoPairs = obj->getInfoPairs();
    for (int i = 0; i < infoPairs.size(); ++i)
    {
        if ( !infoRepository.save(infoPairs.at(i)) )
            return false;
    }

    obj->setSaved(true);
    return true;
}

bool AccountRepository::remove(Account *obj)
{
    bool ret = remove(obj->getId());
    if (ret)
    {
        obj->setSaved(false);
    }
    return ret;
}

bool AccountRepository::remove(const QUuid &id)
{
    if (id.isNull())
        return true;

    InfoRepository infoRepository(m_db);
    if (infoRepository.removeByAccountId(id) <= -1)
        return false;

    QSqlQuery query = m_db->query(QStringLiteral("DELETE FROM Account where [Id] = ?"), QList<QVariant>() << id.toString() );

    if (!query.isActive())
        return false;
    else
        return query.numRowsAffected();
}

bool AccountRepository::createTable()
{
    m_db->query("CREATE TABLE [Account] ("
                "[ID] TEXTz PRIMARY KEY DEFAULT (lower(hex(randomblob(16))) ) NOT NULL,"
                "[Title] TEXT,"
                "[Labels] TEXT,"
                "[Image] INTEGER,"
                "[Hash] TEXT DEFAULT (lower(hex(randomblob(16))) ) NOT NULL,"
                "[Revision] INTEGER DEFAULT 0,"
                "[Usage] INTEGER DEFAULT 0,"
                "[Created] INTEGER DEFAULT (strftime('%s', 'now')) NOT NULL,"
                "[Modified] INTEGER DEFAULT (strftime('%s', 'now')) NOT NULL,"
                "[Accessed] INTEGER DEFAULT (strftime('%s', 'now')) NOT NULL"
                ")");

    return true;
}

bool AccountRepository::upgradeScheme(qint32 from, qint32 to)
{
    QStringList upgradeSQLs;
    upgradeSQLs << "ALTER TABLE [Account] ADD COLUMN [Labels] TEXT;";     //upgrade to db version 2

    QSqlQuery q;
    bool nret = true;
    for (int i = from; i < to && nret; i++)
    {
        q = m_db->query(upgradeSQLs.at(i-1));
        nret = nret && (q.lastError().type() == QSqlError::NoError);
    }

    return nret;
}

bool AccountRepository::test()
{
    QSqlQuery query = m_db->query(QStringLiteral("select [ID], [Title], [Labels], [Image], [Hash], [Revision], [Usage], [Created], [Modified], [Accessed] from [Account]"), QList<QVariant>() );
    return (query.lastError().type() == QSqlError::NoError);
}

QStringList AccountRepository::labels()
{
    QStringList labels;
    QSqlQuery query = m_db->query(QStringLiteral("select Distinct [Account].Labels from Account WHERE [Account].Labels != ''"), QList<QVariant>() );
    if (!query.first())
        return labels;


    QStringList tmpList;
    do
    {
        tmpList = query.value(0).toString().split(',', QString::SkipEmptyParts);
        foreach (const QString &label, tmpList)
        {
            if (!labels.contains(label, Qt::CaseInsensitive) )
                labels.append(label);
        }
    }
    while (query.next());
    labels.sort(Qt::CaseInsensitive);

    return labels;
}

void AccountRepository::setDatabase(Database *db)
{
    m_db = db;
}

Database *AccountRepository::getDatabase()
{
    return m_db;
}

bool AccountRepository::isValid() const
{
    return m_db != 0;
}



bool AccountRepository::insertOrUpdate(const Account *obj)
{
    //created date is read-only
    //hash and revison should be reset before sync
    //http://stackoverflow.com/questions/3634984/insert-if-not-exists-else-update
    QSqlQuery query= m_db->query(QStringLiteral("INSERT OR REPLACE INTO [Account]"
                                 "("
                                 "ID,"
                                 "Title,"
                                 "Labels,"
                                 //"Hash,"
                                 "Revision,"
                                 "Modified"
                                 ") values"
                                 "("
                                 "?,"
                                 "?,"
                                 "?,"
                                 //"lower(hex(randomblob(16))),"
                                 "ifnull((select [Revision] from [Account] where id =?),-1)+1,"
                                 "strftime('%s', 'now')"
                                 ")"),
                                 QList<QVariant>() << obj->getId().toString() << obj->getTitle() << obj->getLabels().join(',') << obj->getId().toString());

    if (!query.isActive())
        return false;
    return true;
}

int AccountRepository::deleteRemovedInfo(Account *obj)
{
    QList<QVariant> existingIds;
    QString idStr;
    foreach (Info *info, obj->getInfoPairs())
    {
        existingIds.append( info->getId().toString() );
        idStr.append("?, ");
    }
    idStr = idStr.left(idStr.length()-2);
    if (idStr.isEmpty())
    {
        InfoRepository infoRepo(m_db);
        return infoRepo.removeByAccountId(obj->getId());
    }

    existingIds.prepend( obj->getId().toString() );
    QSqlQuery query= m_db->query("DELETE FROM [INFO] WHERE [Account]=? AND [ID] NOT IN (" + idStr + ")", existingIds);
    if (!query.isActive())
        return -1;

    return query.numRowsAffected();
}

QList<Account *> AccountRepository::loadAllbyQuery(QSqlQuery &query, QObject *parent)
{
    QList<Account *> accounts;
    if (!query.first())
        return accounts;

    Account *acc = 0;
    QString hexId;

    QString infoId;
    do
    {
        if (hexId != query.value(0).toString())
        {
            hexId = query.value(0).toString();
            if (acc)
            {
                acc->setSaved(true);
                accounts.append(acc);   //add previous
            }
            acc = new Account(parent);
            acc->setId(hexId);
            acc->setTitle(query.value(1).toString());
            acc->setLabels(query.value(2).toString().split(',', QString::SkipEmptyParts));
            acc->setCreatedDate(QDateTime::fromTime_t(query.value(3).toInt()).toLocalTime().date());
        }

        infoId = query.value(4).toString();
        if (infoId.isEmpty())
            continue;
        Info *info = new Info(acc);
        info->setId(infoId);
        info->setKey(query.value(6).toString());
        info->setValue(query.value(7).toString());
        info->setOrder(query.value(8).toInt());
        info->setFormInput(query.value(9).toString());
        info->setType((Info::KeyType) query.value(5).toInt());
        acc->addInfo(info);
        info->setSaved(true);
    }
    while (query.next());

    if (acc)
    {
        acc->setSaved(true);
        accounts.append(acc);
    }

    return accounts;
}
