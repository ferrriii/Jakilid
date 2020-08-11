#ifndef JAKILIDUI_H
#define JAKILIDUI_H

#include <QObject>
#include <QStringList>
//#include <application/profileservice.h>

/*!
 * \brief The JakilidUI class provide API to communicate with OS. functionalities like minimize, maximize, close window, etc.
 */
class JakilidUI : public QObject
{
    Q_OBJECT
public:
    explicit JakilidUI(QWidget *parent = 0);
     Q_PROPERTY(bool isMaximized READ isMaximized)
    Q_PROPERTY(QString preferedDatabasePath READ preferedDatabasePath)
    Q_PROPERTY(QString preferredLanguage READ preferredLanguage)

    Q_INVOKABLE QString     browseSaveFile(const QString &curFile = QString());
    Q_INVOKABLE QString     browseOpenFile(const QString &curFile = QString());
    Q_INVOKABLE QString     isValidDbFile(const QString &db, bool isNew = true);
    Q_INVOKABLE void        setPreferredLanguage(const QString &lang);
    Q_INVOKABLE QString     browser() const;
    Q_INVOKABLE bool        isMaximized() const;
    Q_INVOKABLE QString     preferedDatabasePath() const;
    Q_INVOKABLE QString     preferredLanguage() const;

signals:
    
public slots:
    void quit();
    void copyToClipboard(const QString &txt);
    void minimize();
    void maximize();
    void restore();

private:
    QWidget *m_parentWidget;

};

#endif // JAKILIDUI_H
