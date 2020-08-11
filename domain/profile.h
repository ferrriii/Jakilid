#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QDateTime>
#include "IVariantConfigurable.h"
#include "IJsonConfigurable.h"

class Profile : public IVariantConfigurable, public IJsonConfigurable
{
public:
    explicit Profile();
    explicit Profile(const QString &name, const QString &db);
    virtual ~Profile();

    QVariant    toVariant() const;
    bool        fromVariant(const QVariant &json);
    QJsonValue  toJson() const;
    bool        fromJson(const QJsonValue &json);

    
    QString name() const;
    void setName(const QString &name);

    QString db() const;
    void setDb(const QString &db);


    int timeout() const;
    void setTimeout(int timeout);

    bool isValid() const;

    QString lang() const;
    void setLang(const QString &lang);

private:
    QString m_name;
    QString m_db;
    QString m_lang;
    int     m_timeout;
    
};

#endif // PROFILE_H
