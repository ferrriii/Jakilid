#include <QCoreApplication>
#include <QSharedMemory>
//#include <QMessageBox>
#include "websocketservice.h"
#include "clientmanager.h"
#include "jsonpservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    QCoreApplication::arguments()

    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("github.com/ferrriii/Jakilid"));
    QCoreApplication::setApplicationName(QStringLiteral("Jakilid Password Manager"));

    QSharedMemory mem(QStringLiteral("jakilid-shared-key"));
    if (!mem.create(32))
        return 0;       //already started

    ClientManager clientMngr;

    WebSocketService webSocketService(&clientMngr);
    webSocketService.listen(QHostAddress::LocalHost, 55887);

    JsonpService jsonpService(&clientMngr);
    jsonpService.listen(QHostAddress::LocalHost, 55886);

//    if (!webSocketService.isListening())
//        QMessageBox::critical(0, "jakilid", "jakilid service failed to start because port 55887 is in use by another process. Close the process and jakilid will automatically start to work.");

    return a.exec();
}
