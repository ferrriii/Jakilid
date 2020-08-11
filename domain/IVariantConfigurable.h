#ifndef IVARIANTCONFIGURABLE_H
#define IVARIANTCONFIGURABLE_H


#include <QVariantMap>

class IVariantConfigurable
{
    virtual QVariant    toVariant() const = 0;
    virtual bool        fromVariant(const QVariant &json) = 0;
};

#endif // IVARIANTCONFIGURABLE_H
