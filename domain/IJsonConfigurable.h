#ifndef IJSONCONFIGURABLE_H
#define IJSONCONFIGURABLE_H

#include <QJsonDocument>

class IJsonConfigurable
{
    virtual QJsonValue  toJson() const = 0;
    virtual bool        fromJson(const QJsonValue &json) = 0;
};

#endif // IJSONCONFIGURABLE_H
