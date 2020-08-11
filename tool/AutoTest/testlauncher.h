#ifndef TESTLAUNCHER_H
#define TESTLAUNCHER_H

#include <QObject>
#include <QElapsedTimer>
class QProcess;
class TestResultParser;

class TestLauncher : public QObject
{
    Q_OBJECT
public:
    explicit TestLauncher(const QString &testExe, QObject *parent = 0);
    ~TestLauncher();
    
    QString testExecutable() const;
    void setTestExecutable(const QString &testExecutable);

    bool isRunning() const;

    qint64 executionTime() const;


    TestResultParser *results() const;

signals:
    void finished();
    
public slots:
    void start();
    void terminate();

private slots:
    void readTestOutput();
    void finished(int result);
    void started();

private:
    QProcess *m_testProcess;
    TestResultParser *m_testResultParser;

    QByteArray m_rawResults;
    QString m_testExecutable;

    QElapsedTimer m_elapsed;
    qint64 m_executionTime;

    bool m_isRunning;

};

#endif // TESTLAUNCHER_H
