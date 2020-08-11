#ifndef SESSIONSERVICE_H
#define SESSIONSERVICE_H

#include <Domain/session.h>

class SessionService
{
public:
    SessionService();
    Session *createSession(const QString &profileName);

};

#endif // SESSIONSERVICE_H
