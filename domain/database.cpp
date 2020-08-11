#include "database.h"
#include <QSqlQuery>
#include <QFile>

Database::Database(const QString dbFile, const QString &pass, const QString &connectionName): m_connectionName(connectionName), m_isReady(false), m_queryCount(0), m_dbFile(dbFile)
{
    m_database = new QSqlDatabase(QSqlDatabase::addDatabase(QStringLiteral("QSQLCIPHER"), m_connectionName));
    open(m_dbFile, pass);
}

Database::Database(const QString dbFile, const QString &connectionName): m_connectionName(connectionName), m_isReady(false), m_queryCount(0), m_dbFile(dbFile)
{
    m_database = new QSqlDatabase(QSqlDatabase::addDatabase(QStringLiteral("QSQLCIPHER"), m_connectionName));
}

Database::Database(const QString &connectionName): m_connectionName(connectionName), m_isReady(false), m_queryCount(0)
{
    m_database = new QSqlDatabase(QSqlDatabase::addDatabase(QStringLiteral("QSQLCIPHER"), m_connectionName));
}

Database::~Database()
{
    m_database->close();
    delete m_database;
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool Database::decrypt(const QString &pass)
{
    QSqlQuery query(*m_database);
    return query.exec("PRAGMA key = '" + QString(pass).replace("'", "\\'") + "';");
}

bool Database::open(const QString &pass)
{
    return open(m_dbFile, pass);
}



bool Database::open(const QString &dbFile, const QString &pass)
{
    if (!QFile::exists(dbFile))
    {
        close();
        return false;
    }

    //open db
    m_database->setDatabaseName(dbFile);
    if (!m_database->open())
    {
        close();
        return false;//qDebug() << "error opening database";
    }

    m_dbFile = dbFile;
    //decrypt db
    if (!decrypt(pass))
    {
        close();
        return false;   //error executing decrypt query
    }
    //check if decrypted
    QSqlQuery q = query(QStringLiteral("SELECT count(*) FROM sqlite_master;"));
    if (q.first() && q.value(0).isValid())
    {
        m_isReady = true;
        return true;
    }
    else
    {
        close();
        return false;
    }
}

void Database::close()
{
    m_database->close();
    m_isReady = false;
    m_queryCount = 0;
}



bool Database::changeKey(const QString &pass)
{
    if (!isReady())
        return false;
    //NOTE: 1 is maximum number of queries executed for openning and encryption/decryption
    if (m_queryCount > 1 )   //NOTE: sqlcipher rule: rekey must be run after pragma key and it can not be used to encrypted a standard SQLite database! It is only useful for changing the key on an existing
        return false;

    QSqlQuery qry(*m_database);
    qry.exec("PRAGMA rekey = '" + QString(pass).replace("'", "\\'") + "';");
    return true;
}

QSqlDatabase *Database::databse()
{
    return m_database;
}

/*!
 * \brief you can execute your queries with this method. if database is not open, this method throw error.
 * \param strSqlQuery is your query
 * \param args are query values
 * \return query that was built
 */
QSqlQuery Database::query(const QString &strSqlQuery, const QList<QVariant> &args)
{
    //    QMutexLocker locker(&m_mutex);

        //we could not open database here because we don't know db file name so throw exception if db is not open
    //    if ( !m_database.isOpen() )
    //        qDebug() << "query failed: db not open";// throw WappexException("Cannot open database.");

        QSqlQuery q("", *m_database);
        q.prepare(strSqlQuery);
        for (int i = 0; i < args.size(); ++i)
            q.addBindValue(args.at(i));

        q.exec();
        ++m_queryCount;
        return q;
}

int Database::queryCount() const
{
   return m_queryCount;
}

bool Database::isReady() const
{
    return m_isReady;
}

QString Database::dbFile() const
{
    return m_dbFile;
}
