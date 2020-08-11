#include "testlauncher.h"
#include "testresultparser.h"
#include <QProcess>
#include <QDebug>

TestLauncher::TestLauncher(const QString &testExe,QObject *parent) :
    QObject(parent), m_testExecutable(testExe), m_executionTime(0), m_isRunning(false)
{
    m_testProcess = new QProcess(this);
    connect(m_testProcess, SIGNAL(readyReadStandardOutput ()), this, SLOT(readTestOutput()));
    connect(m_testProcess, SIGNAL(started()), this, SLOT(started()));
    connect(m_testProcess, SIGNAL(finished(int)), this, SLOT(finished(int)));

    m_testResultParser = new TestResultParser(this);
}

TestLauncher::~TestLauncher()
{
    delete m_testResultParser;
    delete m_testProcess;
}

QString TestLauncher::testExecutable() const
{
    return m_testExecutable;
}

void TestLauncher::setTestExecutable(const QString &testExecutable)
{
    m_testExecutable = testExecutable;
}

void TestLauncher::start()
{
    m_testProcess->terminate();
    m_testProcess->waitForFinished();
    m_rawResults = "";
    m_testProcess->start(m_testExecutable);
//    m_elapsed.restart();
    m_isRunning = true;
}

void TestLauncher::terminate()
{
    m_testProcess->terminate();
}

void TestLauncher::readTestOutput()
{
    m_rawResults.append(m_testProcess->readAll());
}

void TestLauncher::finished(int result)
{
    m_executionTime = m_elapsed.elapsed();
    m_testResultParser->setReturn(result);
    m_testResultParser->parse(m_rawResults);
    m_isRunning = false;
    emit finished();
}

void TestLauncher::started()
{
    m_elapsed.restart();
}



TestResultParser *TestLauncher::results() const
{
    return m_testResultParser;
}



qint64 TestLauncher::executionTime() const
{
    return m_executionTime;
}



bool TestLauncher::isRunning() const
{
    return m_isRunning;
}



