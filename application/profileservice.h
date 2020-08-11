#ifndef PROFILESERVICE_H
#define PROFILESERVICE_H

#include <domain/profile.h>
#include <dataaccess/profilerepository.h>

class ProfileService
{
public:
    ProfileService();

    //all business logic methods related to profiles
    bool addProfile(const Profile *profile, const QString &pass);
    bool removeProfile(const QString &profileName);
//    bool changeProfileName(const QString &profileName, const QString &newName);
    QStringList allProfileNames() const;


private:
    ProfileRepository m_profileRepository;
};

#endif // PROFILESERVICE_H

