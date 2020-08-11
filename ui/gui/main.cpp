#include <QApplication>
#include <QSharedMemory>
#include <QProcess>
#include <windows.h>
#include "mainwin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("Jakilid"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("Jakilid.com"));
    QCoreApplication::setApplicationName(QStringLiteral("Jakilid Password Manager"));

    QSharedMemory mem(QStringLiteral("jakilid-shared-key"));
    if (mem.create(32))
    {
        //jakilid service has not been started, start it
        mem.detach();
        QProcess::startDetached(QStringLiteral("JakilidService"));
    }
	
    CreateMutexA(0, 0, "JakilidMutex");	//for using in setup software

    MainWin w;
    w.show();

    return a.exec();
}
