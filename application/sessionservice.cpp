#include "sessionservice.h"
#include <dataaccess/profilerepository.h>
#include <Domain/session.h>

SessionService::SessionService()
{
}

Session *SessionService::createSession(const QString &profileName)
{
    ProfileRepository profileRepository;
    Profile *profile =  profileRepository.loadByName(profileName);
    if (profile == 0)
        return 0;   //profile does not exist
    if (!profile->isValid())
    {
        delete profile;
        return 0;   //invalid profile
    }

    Session *session = new Session(profile);    //NOTE: session takes ownership of profile
//    if (!pass.isNull())                       //do not unlock session because it has not been created , its signals has not been connected
//        session->unlock(pass);
    return session;
}
