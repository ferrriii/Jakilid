#include "info.h"
#include "account.h"
#include <QJsonObject>

Info::Info(): m_account(0),  m_id(QUuid::createUuid()), m_type(KeyTypeText), m_order(0), m_isSaved(false)//, m_isPersisted(false)
{
}

Info::Info(Account *acc) : m_account(acc),  m_id(QUuid::createUuid()), m_type(KeyTypeText), m_order(0), m_isSaved(false)//, m_isPersisted(false)
{
}

Info::~Info()
{

}

QVariant Info::toVariant() const
{
    QVariantMap i;
    i.insert(QStringLiteral("id"), getId().toString() );
    i.insert(QStringLiteral("key"), getKey());
    i.insert(QStringLiteral("value"), getValue());
    i.insert(QStringLiteral("type"), getType());
    i.insert(QStringLiteral("order"), getOrder());
    i.insert(QStringLiteral("formInput"), getFormInput());
    return i;
}

bool Info::fromVariant(const QVariant &json)
{
    QVariantMap infoMap = json.toMap();
    if (infoMap.isEmpty())
        return false;

    QUuid id(infoMap.value("id").toString());
    if (!id.isNull())
        setId(id);

    setKey(infoMap.value(QStringLiteral("key")).toString());
    setValue(infoMap.value(QStringLiteral("value")).toString());
    setType(infoMap.value(QStringLiteral("type")).toInt());
    setOrder(infoMap.value(QStringLiteral("order")).toInt());
    setFormInput(infoMap.value(QStringLiteral("formInput")).toString());

    return true;
}

QJsonValue Info::toJson() const
{
    QJsonObject i;
    i.insert(QStringLiteral("id"), getId().toString() );
    i.insert(QStringLiteral("key"), getKey());
    i.insert(QStringLiteral("value"), getValue());
    i.insert(QStringLiteral("type"), getType());
    i.insert(QStringLiteral("order"), getOrder());
    i.insert(QStringLiteral("formInput"), getFormInput());
    return i;
}

bool Info::fromJson(const QJsonValue &json)
{
    QJsonObject i = json.toObject();
    if (i.isEmpty())
        return false;

    QUuid id(i.value(QStringLiteral("id")).toString());
    if (!id.isNull())
        setId(id);

    setKey(i.value(QStringLiteral("key")).toString());
    setValue(i.value(QStringLiteral("value")).toString());
    setType(i.value(QStringLiteral("type")).toDouble());
    setOrder(i.value(QStringLiteral("order")).toDouble());
    setFormInput(i.value(QStringLiteral("formInput")).toString());

    return true;
}

QUuid Info::getId() const
{
    return m_id;
}


Info::KeyType Info::getType() const
{
    return m_type;
}

QString Info::getKey() const
{
    return m_key;
}

QString Info::getValue() const
{
    return m_value;
}

int Info::getOrder() const
{
    return m_order;
}

const Account *Info::getAccount() const
{
    return m_account;
}

void Info::setAccount(const Account *acc)
{
    m_isSaved = m_isSaved && (m_account == acc);
    m_account = acc;
}

void Info::setId(const QUuid &id)
{
    m_isSaved = m_isSaved && (m_id == id);
    m_id = id;
}

void Info::setType(Info::KeyType type)
{
    m_isSaved = m_isSaved && (m_type == type);
    m_type = type;
}

void Info::setType(int type)
{
    KeyType t = static_cast<KeyType>(type);
    setType(t);
}

void Info::setKey(const QString &key)
{
    m_isSaved = m_isSaved && (m_key == key);
    m_key = key;
}

void Info::setValue(const QString &value)
{
    m_isSaved = m_isSaved && (m_value == value);
    m_value = value;
}

void Info::setOrder(int order)
{
    m_isSaved = m_isSaved && (m_order == order);
    m_order = order;
}

bool Info::isSaved() const
{
    return m_isSaved;
}

void Info::setSaved(bool isSaved)
{
    m_isSaved = isSaved;
}

bool Info::isValid() const
{
    return m_account && getKey() != "" && !getId().isNull();
}
QString Info::getFormInput() const
{
    return m_formInput;
}

void Info::setFormInput(const QString &formInput)
{
    m_formInput = formInput;
}


//bool Info::isPersisted() const
//{
//    return m_isPersisted;
//}

//void Info::setIsPersisted(bool isPersisted)
//{
//    m_isPersisted = isPersisted;
//}
