#include "jakilidui.h"
#include <QApplication>
#include <QFileDialog>
#include <QClipboard>
//#include <application/databaseservice.h>
#include <QStandardPaths>
#include <QTextStream>
#include <QSettings>

JakilidUI::JakilidUI(QWidget *parent) :
    QObject(parent), m_parentWidget(parent)
{
}

//QStringList JakilidUI::getAvailableProfiles() const
//{
//    return m_profileService.allProfileNames();
//}

QString JakilidUI::browseSaveFile(const QString &curFile)
{
	//TODO: set default selected file according to curFile using QFileDialog::selectFile()
    return QFileDialog::getSaveFileName(m_parentWidget, tr("Select a new file"), curFile, tr("Jakilid Database (*.jkdb)"));
}

QString JakilidUI::browseOpenFile(const QString &curFile)
{
	//TODO: set default selected file according to curFile using QFileDialog::selectFile()
    return QFileDialog::getOpenFileName(m_parentWidget, tr("Select an existing file"), curFile, tr("Jakilid Database (*.jkdb)"));
}


QString JakilidUI::isValidDbFile(const QString &db, bool isNew)
{
    if (!isNew && !QFile::exists(db))
        return QString(tr("Specified file does not exist!"));
    QFile file(db);
    if (!file.open(QIODevice::ReadWrite))
        return QString(tr("Can not access file."));
    if (isNew && !file.remove())
        return QString(tr("Can not access file."));

    return "";
}

QString JakilidUI::preferedDatabasePath() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    dir.append(QStringLiteral("/Jakilid"));
    QDir jakilid;
    jakilid.mkpath(dir);
    QString dbFile = dir + QStringLiteral("/Jakilid.jkdb");
    for (int i = 1; QFile::exists(dbFile); ++i)
        dbFile = dir + QStringLiteral("/Jakilid_%1.jkdb").arg(i);

    return dbFile;
}



bool JakilidUI::isMaximized() const
{
    return m_parentWidget->windowState() == Qt::WindowMaximized;
}

QString JakilidUI::preferredLanguage() const
{
//    ProfileRepository profileRepo;
//    QStringList existingProfiles = m_profileService.allProfileNames();
//    if (existingProfiles.count() > 0)
//        return profileRepo.loadByName(existingProfiles.at(0))->lang();  //use existing profiles language
//    else
    QSettings settings;
    return settings.value(QStringLiteral("language"),
                   QLocale::system().name().replace("_","-")).toString();

//    return QLocale::system().name().replace("_","-");   //use system language
}

void JakilidUI::setPreferredLanguage(const QString &lang)
{
    QSettings settings;
    settings.setValue(QStringLiteral("language"), lang);
    settings.sync();
}

QString JakilidUI::browser() const
{
    return QStringLiteral("Jakilid");
}

void JakilidUI::quit()
{
    QApplication::quit();
}

void JakilidUI::copyToClipboard(const QString &txt)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(txt);
}

void JakilidUI::minimize()
{
    m_parentWidget->showMinimized();
}

void JakilidUI::maximize()
{
    m_parentWidget->showMaximized();
}

void JakilidUI::restore()
{
    m_parentWidget->showNormal();
}
