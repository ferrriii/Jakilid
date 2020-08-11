#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>
#include <QVariant>

/*!
 * \brief The Database class. a wrapper around QSqlDatabase for SQLCIPHER.
 */
class Database
{
public:
    explicit Database(const QString dbFile, const QString &pass, const QString &connectionName);
    explicit Database(const QString dbFile, const QString &connectionName);
    explicit Database(const QString &connectionName = QString());
    ~Database();

    bool         open(const QString &dbFile, const QString &pass);
    bool         open(const QString &pass);
    void         close();
    bool         changeKey(const QString &pass);
    QSqlDatabase*databse();
    QSqlQuery    query(const QString &strSqlQuery, const QList<QVariant> &args = QVariantList());
    int          queryCount() const;

    bool         isReady() const;
    QString      dbFile() const;


private:
    QString m_connectionName;
    QSqlDatabase *m_database;
    bool m_isReady;
    int m_queryCount;
    QString m_dbFile;

    bool         decrypt(const QString &pass);
};

#endif // DATABASE_H
