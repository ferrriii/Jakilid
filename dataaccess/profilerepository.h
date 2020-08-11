#ifndef PROFILEREPOSITORY_H
#define PROFILEREPOSITORY_H

#include <QSettings>
#include "domain/profile.h"

class ProfileRepository
{
public:
    ProfileRepository();
    ~ProfileRepository();
    Profile*loadByName(const QString &name);
    bool    save(const Profile *profile);
    bool    doesProfileExist(const QString &name);
    bool    removeByName(const QString &name);

    QStringList allProfileNames() const;


private:
    QSettings m_settings;

};

#endif // PROFILEREPOSITORY_H
