#include "profile.h"
#include <QJsonObject>

Profile::Profile() : m_lang(QStringLiteral("en-US")), m_timeout(600)
{
}

Profile::Profile(const QString &name, const QString &db) : m_name(name), m_db(db), m_lang(QStringLiteral("en-US")), m_timeout(600)
{

}

Profile::~Profile()
{

}

QVariant Profile::toVariant() const
{
    QVariantMap i;
    i.insert(QStringLiteral("name"), name());
    i.insert(QStringLiteral("db"), db());
    i.insert(QStringLiteral("timeout"), timeout());
    i.insert(QStringLiteral("lang"), lang());
    return i;
}

bool Profile::fromVariant(const QVariant &json)
{
    QVariantMap infoMap = json.toMap();
    if (infoMap.isEmpty())
        return false;
    setName(infoMap.value(QStringLiteral("name")).toString());
    setDb(infoMap.value(QStringLiteral("db")).toString());
    setTimeout(infoMap.value(QStringLiteral("timeout")).toInt());
    setLang(infoMap.value(QStringLiteral("lang")).toString());
    return true;
}

QJsonValue Profile::toJson() const
{
    QJsonObject i;
    i.insert(QStringLiteral("name"), name());
    i.insert(QStringLiteral("db"), db());
    i.insert(QStringLiteral("timeout"), timeout());
    i.insert(QStringLiteral("lang"), lang());
    return i;
}

bool Profile::fromJson(const QJsonValue &json)
{
    QJsonObject infoMap = json.toObject();
    if (infoMap.isEmpty())
        return false;
    setName(infoMap.value(QStringLiteral("name")).toString());
    setDb(infoMap.value(QStringLiteral("db")).toString());
    setTimeout(infoMap.value(QStringLiteral("timeout")).toDouble());
    setLang(infoMap.value(QStringLiteral("lang")).toString());
    return true;
}

QString Profile::name() const
{
    return m_name;
}

void Profile::setName(const QString &name)
{
    m_name = name;
}
QString Profile::db() const
{
    return m_db;
}

void Profile::setDb(const QString &db)
{
    m_db = db;
}
int Profile::timeout() const
{
    return m_timeout;
}

void Profile::setTimeout(int timeout)
{
    m_timeout = timeout;
}

bool Profile::isValid() const
{
    return !m_name.isEmpty() && !m_db.isEmpty();
}
QString Profile::lang() const
{
    return m_lang;
}

void Profile::setLang(const QString &lang)
{
    m_lang = lang;
}

