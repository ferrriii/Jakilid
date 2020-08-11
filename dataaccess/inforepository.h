#ifndef INFOREPOSITORY_H
#define INFOREPOSITORY_H

#include <QUuid>
class Info;
class Database;
class QObject;


class InfoRepository
{
public:
    InfoRepository(Database *db);

    bool    save(Info *obj);
    bool    remove(Info *obj);
    bool    remove(const QUuid &id);
    int     removeByAccountId(const QUuid &id);
    bool    createTable();
    bool    upgradeSchem(qint32 from, qint32 to);

    bool    test();

private:
    Database *m_db;

//    bool    insert(Info *obj);
//    bool    update(Info *obj);
    bool    insertOrUpdate(const Info *obj);

};

#endif // INFOREPOSITORY_H
