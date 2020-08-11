#include "profileservice.h"
#include <dataaccess/accountrepository.h>
#include <dataaccess/inforepository.h>
#include <domain/database.h>
#include <QStringList>

ProfileService::ProfileService()
{
}

bool ProfileService::addProfile(const Profile *profile, const QString &pass)
{
    if (!profile->isValid())
        return false;   //Invalid Profile

    if (m_profileRepository.doesProfileExist(profile->name()))
        return false;   //duplicate profile

    //do not allow profile creation for invalid db file or when user doesn't know db's password
    Database database(QString::number(qrand())/*profile->name()*/);
    if (!database.open(profile->db(), pass))
        return false; //wrong password, error creating/openning db file

    if (!m_profileRepository.save(profile))
        return false;   //error saving profile

    return true;
}


bool ProfileService::removeProfile(const QString &profileName)
{
    return m_profileRepository.removeByName(profileName);
}


QStringList ProfileService::allProfileNames() const
{
    return m_profileRepository.allProfileNames();
}
