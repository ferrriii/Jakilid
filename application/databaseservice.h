#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <domain/database.h>
#include <QString>

class DatabaseService
{
public:
    DatabaseService();
    bool createDatabase(const QString &dbPath, const QString &pass);
    bool changeDatabasePassword(Database *db, const QString &oldPass, const QString &newPass);
    int  upgradeDatabase(Database *db);
    qint32 dbVersion(Database *db);
private:
    void setDbVersion(Database *db);
};

#endif // DATABASESERVICE_H

