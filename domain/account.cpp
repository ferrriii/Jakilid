#include "account.h"
#include <QJsonArray>
#include <QJsonObject>

Account::Account(QObject *parent) :
    QObject(parent), m_id(QUuid::createUuid()), m_isSaved(false), m_removedInfoCount(0)//, m_isPersisted(false)
{
    m_createdDate = QDate::currentDate();
}

Account::~Account()
{
    qDeleteAll(m_infoPairs);
}

QVariant Account::toVariant() const
{
    QVariantMap acc;
    acc.insert(QStringLiteral("id"), getId().toString() ) ;
    acc.insert(QStringLiteral("title"), getTitle());
    QString isoDate = QString::number(getCreatedDate().year()) + "-" +
                      QString::number(getCreatedDate().month()).rightJustified(2,'0') + "-" +
                      QString::number(getCreatedDate().day()).rightJustified(2,'0');
    acc.insert(QStringLiteral("created"),  isoDate); //ISO date format
    QVariantList info;

    foreach (Info *i, m_infoPairs)
    {
        info.append(i->toVariant());
    }

    acc.insert(QStringLiteral("infoPairs"), info);
    return acc;
}

bool Account::fromVariant(const QVariant &json)
{
    QVariantMap accMap = json.toMap();
    if (accMap.isEmpty())
        return false;

    QUuid id(accMap.value(QStringLiteral("id")).toString());
    if (!id.isNull())
        setId(id);

    setTitle(accMap.value(QStringLiteral("title")).toString());
    setCreatedDate(QDate::fromString(accMap.value(QStringLiteral("created")).toString(), QStringLiteral("yyyy-MM-dd")));

    QVariantList infoPairs = accMap.value(QStringLiteral("infoPairs")).toList();
    qDeleteAll(m_infoPairs);
    for (int i = 0; i < infoPairs.size(); ++i)
    {
        Info *info = new Info(this);
        if (info->fromVariant(infoPairs.at(i)))
            m_infoPairs.append(info);
        else
            delete info;

    }
    return true;
}

QJsonValue Account::toJson() const
{
    QJsonObject acc;
    acc.insert(QStringLiteral("id"), getId().toString() ) ;
    acc.insert(QStringLiteral("title"), getTitle());
    QString isoDate = QString::number(getCreatedDate().year()) + "-" +
                      QString::number(getCreatedDate().month()).rightJustified(2,'0') + "-" +
                      QString::number(getCreatedDate().day()).rightJustified(2,'0');
    acc.insert(QStringLiteral("created"),  isoDate ); //ISO date format
    QJsonArray info;

    foreach (Info *i, m_infoPairs)
    {
        info.append(i->toJson());
    }

    acc.insert(QStringLiteral("labels"), QJsonArray::fromStringList(m_labels) ) ;
    acc.insert(QStringLiteral("infoPairs"), info);

    return acc;
}

bool Account::fromJson(const QJsonValue &json)
{
    QJsonObject accMap = json.toObject();
    if (accMap.isEmpty())
        return false;

    QUuid id(accMap.value(QStringLiteral("id")).toString());
    if (!id.isNull())
        setId(id);

    m_labels = accMap.value(QStringLiteral("labels")).toVariant().toStringList();

    setTitle(accMap.value(QStringLiteral("title")).toString());
    setCreatedDate(QDate::fromString(accMap.value(QStringLiteral("created")).toString(), QStringLiteral("yyyy-MM-dd")));

    QJsonArray infoPairs = accMap.value(QStringLiteral("infoPairs")).toArray();
    qDeleteAll(m_infoPairs);
    for (int i = 0; i < infoPairs.size(); ++i)
    {
        Info *info = new Info(this);
        if (info->fromJson(infoPairs.at(i)))
            m_infoPairs.append(info);
        else
            delete info;

    }
    return true;
}

QUuid Account::getId() const
{
    return m_id;
}

QString Account::getTitle() const
{
    return m_title;
}


QDate Account::getCreatedDate() const
{
    return m_createdDate;
}

QDateTime Account::getModifiedDate() const
{
    return m_modifiedDate;
}

QDateTime Account::getAccessedDate() const
{
    return m_accessedDate;
}

QList<Info *> Account::getInfoPairs()
{
    return m_infoPairs;
}

Info *Account::addInfo(const QString &key, const QString &value)
{
    //WARNING: maximum info count is about 1000
    Info *i = new Info(this);
    i->setKey(key);
    i->setValue(value);
    i->setOrder(m_infoPairs.count()+1);

    m_infoPairs.append(i);
    return i;
}

void Account::addInfo(Info *info)
{
    info->setAccount(this);
    info->setOrder(m_infoPairs.count()+1);
    m_infoPairs.append(info);
}

int Account::removeInfo(Info *info)
{
    int cnt = m_infoPairs.removeAll(info);
    if (cnt)
        delete info;
    m_removedInfoCount += cnt;
    return cnt;
}

void Account::setId(const QUuid &id)
{
    m_isSaved = m_isSaved && (m_id == id);
    m_id = id;
}

void Account::setTitle(const QString &title)
{
    m_isSaved = m_isSaved && (m_title == title);
    m_title = title;
}

void Account::setCreatedDate(const QDate &date)
{
    m_isSaved = m_isSaved && (m_createdDate == date);
    m_createdDate = date;
}

bool Account::isValid() const
{
    return !getTitle().isEmpty() && !getId().isNull();
}


bool Account::isSaved() const
{
    bool ret = m_isSaved && (m_removedInfoCount == 0);

    for (int i = 0; ret && i< m_infoPairs.size(); ++i)
        ret = ret && m_infoPairs.at(i)->isSaved();

    return ret;
}

bool Account::isChanged() const
{
    return m_isSaved;
}


void Account::setSaved(bool saved)
{
    m_isSaved = saved;
    m_removedInfoCount = 0;
}
QStringList Account::getLabels() const
{
    return m_labels;
}

void Account::setLabels(const QStringList &lables)
{
    m_isSaved = false;
    m_labels = lables;
}

void Account::addLabel(const QString &label)
{
    //do not allow duplicate labels
    if (m_labels.contains(label, Qt::CaseInsensitive))
        return;
    m_isSaved = false;
    m_labels.append(label);
}

//bool Account::isPersisted() const
//{
//    return m_isPersisted;
//}

//void Account::setIsPersisted(bool isPersisted)
//{
//    m_isPersisted = isPersisted;
//}

