#include "inforepository.h"
#include "domain/database.h"
#include "domain/account.h"
#include "domain/info.h"
#include <QSqlError>
#include <QSqlQuery>

InfoRepository::InfoRepository(Database *db) : m_db(db)
{
}

bool InfoRepository::save(Info *obj)
{
    if (!obj->isValid())
        return false;
    if (obj->isSaved())
        return true;

    obj->setSaved(insertOrUpdate(obj));

    return obj->isSaved();
}

bool InfoRepository::remove(Info *obj)
{
    bool ret = remove(obj->getId());
    if (ret)
    {
        obj->setSaved(false);
    }
    return ret;
}

bool InfoRepository::remove(const QUuid &id)
{
    if (id.isNull())
        return true;

    QSqlQuery query = m_db->query(QStringLiteral("DELETE from Info WHERE [ID]=?"), QList<QVariant>() <<
                               id.toString() );
    if (!query.isActive())
        return false;

    return query.numRowsAffected();
}

int InfoRepository::removeByAccountId(const QUuid &id)
{
    if (id.isNull())
        return 0;

    QSqlQuery query = m_db->query(QStringLiteral("DELETE from Info WHERE [Account]=?"), QList<QVariant>() <<
                               id.toString() );
    if (!query.isActive())
        return -1;

    return query.numRowsAffected();
}

bool InfoRepository::createTable()
{
    m_db->query("CREATE TABLE [Info] ("
                "[ID] TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16))) ) NOT NULL,"
                "[Account] TEXT NOT NULL,"
                "[Key] TEXT NOT NULL,"
                "[Value] TEXT,"
                "[Order] INTEGER DEFAULT 0 NULL,"
                "[FormInput] TEXT,"
                "[Type] INTEGER DEFAULT 0 NULL"
                ")");
    //NOTE: following index increase search time by about 14% but decrease save time about 10%
    m_db->query("CREATE INDEX account_hash_idx ON [Info] ([Account]);");
    return true;
}

bool InfoRepository::upgradeSchem(qint32 from, qint32 to)
{
    Q_UNUSED(from);
    Q_UNUSED(to);
    //for current db version(2) nothing has been changed in info table
    return true;
}

bool InfoRepository::test()
{
    QSqlQuery query = m_db->query(QStringLiteral("select [ID], [Account], [Key], [Value], [Order], [FormInput], [Type] from [Info]"), QList<QVariant>() );
    return (query.lastError().type() == QSqlError::NoError);
}


bool InfoRepository::insertOrUpdate(const Info *obj)
{
    //http://stackoverflow.com/questions/3634984/insert-if-not-exists-else-update
    QSqlQuery query= m_db->query(QStringLiteral("INSERT OR REPLACE INTO [Info]"
                                 "("
                                 "[ID],"
                                 "[Account],"
                                 "[Type],"
                                 "[Key],"
                                 "[Value],"
                                 "[Order],"
                                 "[FormInput]"
                                 ") values"
                                 "("
                                 "?,"
                                 "?,"
                                 "?,"
                                 "?,"
                                 "?,"
                                 "?,"
                                 "?"
                                 ")"),
                                 QList<QVariant>() <<
                                 obj->getId().toString() <<
                                 obj->getAccount()->getId().toString() <<
                                 obj->getType() <<
                                 obj->getKey() <<
                                 obj->getValue() <<
                                 obj->getOrder() <<
                                 obj->getFormInput());

    if (!query.isActive())
        return false;
    return true;
}
