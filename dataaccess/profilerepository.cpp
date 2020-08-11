#include "profilerepository.h"
#include <QStringList>

ProfileRepository::ProfileRepository()
{
}

ProfileRepository::~ProfileRepository()
{
}

Profile *ProfileRepository::loadByName(const QString &name)
{
    QVariantMap profiles = m_settings.value(QStringLiteral("profiles")).toMap();
    QVariant profile = profiles.value(name);
    if (!profile.isValid())
        return 0;
    Profile *p = new Profile();
    p->fromVariant(profile);
    return p;
}

bool ProfileRepository::save(const Profile *profile)
{
    QVariantMap profiles = m_settings.value(QStringLiteral("profiles")).toMap();
    profiles.insert(profile->name() , profile->toVariant());

    m_settings.setValue(QStringLiteral("profiles"), profiles);
    m_settings.sync();
    return true;
}

bool ProfileRepository::doesProfileExist(const QString &name)
{
    QVariantMap profiles = m_settings.value(QStringLiteral("profiles")).toMap();
    return profiles.contains(name);
}

bool ProfileRepository::removeByName(const QString &name)
{
    QVariantMap profiles = m_settings.value(QStringLiteral("profiles")).toMap();
    int ret = profiles.remove(name);
    if (ret)
    {
        m_settings.setValue(QStringLiteral("profiles"), profiles);
        m_settings.sync();
    }
    return ret;
}

QStringList ProfileRepository::allProfileNames() const
{
    QVariantMap profiles = m_settings.value(QStringLiteral("profiles")).toMap();
    return profiles.keys();
}
