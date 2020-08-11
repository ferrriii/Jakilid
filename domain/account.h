#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QUuid>
#include "IVariantConfigurable.h"
#include "IJsonConfigurable.h"
#include "info.h"

class Account : public QObject, public IVariantConfigurable, public IJsonConfigurable
{
    Q_OBJECT
public:
    explicit    Account(QObject *parent = 0);
    ~Account();
    QVariant    toVariant() const;
    bool        fromVariant(const QVariant &json);
    QJsonValue  toJson() const;
    bool        fromJson(const QJsonValue &json);
    QUuid       getId() const;
    QString     getTitle() const;
    QDate       getCreatedDate() const;
    QDateTime   getModifiedDate() const;
    QDateTime   getAccessedDate() const;
    QList<Info*> getInfoPairs();


    Info*       addInfo(const QString &key, const QString &value);
    void        addInfo(Info *info);
    int         removeInfo(Info* info);
    void        setId(const QUuid &id);
    void        setTitle(const QString &title);
    void        setCreatedDate(const QDate &date);

    bool        isValid() const;
    bool        isSaved() const;
    bool        isChanged() const;
    void        setSaved(bool saved);
    
//    bool isPersisted() const;
//    void setIsPersisted(bool isPersisted);

    QStringList getLabels() const;
    void setLabels(const QStringList &labels);
    void addLabel(const QString &label);

public slots:

private:
    QUuid       m_id;
    QString     m_title;
    QDate       m_createdDate;
    QDateTime   m_modifiedDate;
    QDateTime   m_accessedDate;

    bool        m_isSaved;
    QList<Info*> m_infoPairs;
    QStringList m_labels;

    int         m_removedInfoCount;
//    bool        m_isPersisted;

signals:
    
    
};

#endif // ACCOUNT_H
