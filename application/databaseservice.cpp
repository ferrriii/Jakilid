#include "databaseservice.h"
#include <dataaccess/accountrepository.h>
#include <dataaccess/inforepository.h>
#include <QFile>
#include <QSqlQuery>

DatabaseService::DatabaseService()
{
}

bool DatabaseService::createDatabase(const QString &dbPath, const QString &pass)
{
    Database database(QString::number(qrand()));    //create database with random connection name
    QFile f(dbPath);
    if (!f.open(QIODevice::ReadWrite))
        return false;
    f.close();
    if (!database.open(dbPath, pass))
        return false;   //error creating/openning db file

    //forward/backward compatibility
    database.query("CREATE TABLE [DataBase] ("
                "[Version] TEXT DEFAULT ('" + QString::fromLocal8Bit(JAKILID_VERSION) + "') NOT NULL,"
                "[Created] INTEGER DEFAULT (strftime('%s', 'now')) NOT NULL,"
                "[Platform] TEXT NULL,"
                "[Hash] TEXT DEFAULT (lower(hex(randomblob(16))) ) NOT NULL,"
                "[Revision] INTEGER DEFAULT 0 NOT NULL"
                ")");
    database.query(QStringLiteral("INSERT INTO [DataBase]([Platform]) VALUES(?);"), QVariantList() << QStringLiteral("Win %1").arg(QSysInfo::windowsVersion()) );
    setDbVersion(&database);
    AccountRepository accountRepository(&database);
    InfoRepository infoRepository(&database);
    return  accountRepository.createTable() &&
            accountRepository.test() &&
            infoRepository.createTable() &&
            infoRepository.test();
}

bool DatabaseService::changeDatabasePassword(Database *db, const QString &oldPass, const QString &newPass)
{
    if (!db)
        return false;
    //make sure oldPass is right
    //NOTE: we use a seperate Database class instance for oldpass checking because if oldpass was wrong, the db remains unchanged
    Database oldPassTestDb(db->dbFile(), QString::number(qrand()));
    if (!oldPassTestDb.open(oldPass))
        return false;   //wrong old pass or whatever oldPassTestDb says
    oldPassTestDb.close();

    if (!db->changeKey(newPass))
    {
        //maybe db is not open or it's not first query on db (read more at Database::changeKey)
        db->close();
        db->open(oldPass);
        return db->changeKey(newPass);
    }else
        return true;
}


/*!
 * \brief DatabaseService::upgradeDatabase
 * \param db
 * \return 0: failed to upgrade
 *         1: successfully upgraded
 *         2: no upgrade required
 *         3: don't know how to upgrade (db is newer than current version)
 */
int DatabaseService::upgradeDatabase(Database *db)
{
    //NOTE: don't make previous versions stop working
    if (!db || !db->isReady())
        return 0;


    qint32 dbVer = dbVersion(db);
    int curVer = JAKILID_DB_VERSION_NUMBER;

    if (dbVer <= 0)
    {
        return 0;   //failed to upgrade
    }
    else if (dbVer == curVer)
    {
        return 2;    //no upgrade required
    }
    else if (dbVer > curVer)
    {
        return 3;   //don't know how to upgrade (db is newer than current version) probably that's ok, we assume higher versions have maximum compatibility
    }

    //upgrade db:

    QSqlDatabase *qdb = db->databse();
    qdb->transaction();

    AccountRepository accountRepository(db);
    InfoRepository infoRepository(db);

    bool nret = accountRepository.upgradeScheme(dbVer, curVer) && infoRepository.upgradeSchem(dbVer, curVer);


    if (nret && accountRepository.test() && infoRepository.test())
    {
        setDbVersion(db);
        qdb->commit();
    }
    else
        qdb->rollback();

    return nret ? 1 : 0;
}

qint32 DatabaseService::dbVersion(Database *db)
{
    if (!db->isReady())
        return 0;

    QSqlQuery qry(*db->databse());
    qry.exec(QStringLiteral("PRAGMA user_version;"));
    if (!qry.first())
        return 0;   //WARNING: this should never happen!

    return qry.value(0).toDouble();
}

void DatabaseService::setDbVersion(Database *db)
{
    db->query(QStringLiteral("PRAGMA user_version = %1;").arg(JAKILID_DB_VERSION_NUMBER));
}
