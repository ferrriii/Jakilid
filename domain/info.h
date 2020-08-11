#ifndef INFO_H
#define INFO_H

#include <QString>
#include <QUuid>
#include "IVariantConfigurable.h"
#include "IJsonConfigurable.h"
class Account;

class Info : public IVariantConfigurable, public IJsonConfigurable
{
public:
    explicit Info();
    explicit Info(Account *acc);
    virtual ~Info();

    enum KeyType
    {
        KeyTypeText,        //html corresponding input type = text
        KeyTypePassword,    //html corresponding input type = password
        KeyTypeComment,     //html corresponding input type = textarea
        KeyTypeEmail,       //html corresponding input type = email
        KeyTypeURL,         //html corresponding input type = url
        KeyTypeTags         //html corresponding input type = text
    };

    QVariant  toVariant() const;
    bool    fromVariant(const QVariant &json);
    QJsonValue toJson() const;
    bool    fromJson(const QJsonValue &json);
    QUuid   getId() const;
    KeyType getType() const;
    QString getKey() const;
    QString getValue() const;
    int     getOrder() const;
    const Account*getAccount() const;

    QString getFormInput() const;

    void    setId(const QUuid &id);
    void    setAccount(const Account *acc);
    void    setType(KeyType type);
    void    setType(int type);
    void    setKey(const QString &key);
    void    setValue(const QString &value);
    void    setOrder(int order);
    void    setFormInput(const QString &getFormInput);

    bool    isSaved() const;
    void    setSaved(bool isSaved);
    bool    isValid() const;

//    bool isPersisted() const;
//    void setIsPersisted(bool isPersisted);


private:
    const Account *m_account;
    QUuid   m_id;
    KeyType m_type;
    QString m_key;
    QString m_value;
    int     m_order;
    QString m_formInput;

    bool    m_isSaved;
//    bool        m_isPersisted;
};

#endif // INFO_H
