#include "testresultparser.h"
#include <QRegExp>
#include <qstringlist.h>
#include "testcase.h"
#include <QDebug>

TestResultParser::TestResultParser(QObject *parent) :
    QObject(parent), m_passed(0), m_failed(0), m_skipped(0), m_isParsed(false), m_return(0)
{
}

TestResultParser::~TestResultParser()
{
    qDeleteAll(m_testCases);
}

bool TestResultParser::parse(const QByteArray &result)
{
    m_isParsed = false;
    QString resultStr = result;

    QRegExp nameRegexp("Start testing of (.+)\\s\\*+\\r", Qt::CaseInsensitive, QRegExp::RegExp2);
    nameRegexp.setMinimal(true);
    if (nameRegexp.indexIn(resultStr) < 0)
        return false;
    m_testName = nameRegexp.cap(1);


    QRegExp configRegexp("Config:\\s+(.+)\\r", Qt::CaseInsensitive, QRegExp::RegExp2);
    configRegexp.setMinimal(true);
    if (configRegexp.indexIn(resultStr) < 0)
        return false;
    m_testConfig = configRegexp.cap(1);


    QStringList tests = resultStr.split(QRegExp("\\n"));
    QString test;
    for (int i = 2; i<tests.count()- (m_return == 0 ? 3 : 0) ; ++i)
    {
        QString line = tests.at(i);
        QString t = line.left( line.indexOf(QRegExp("\\s") ));
        if (t == "PASS" || t == "FAIL!" || t == "QDEBUG" || t == "XFAIL" || t == "RESULT" || t == "WARNING:" || t == "SKIP" || t == "QWARN" || t == "QFATAL" || t == "QSYSTEM:")
        {
            if (!test.isEmpty())
                parseTestCase(test);
            test = line;
        }
        else
            test.append("\n" + line);
    }
    parseTestCase(test);


    QRegExp statRegexp("Totals: (\\d+) passed, (\\d+) failed, (\\d+) skipped", Qt::CaseInsensitive, QRegExp::RegExp2);
    if (statRegexp.indexIn(resultStr) >= 0)
    {
        m_passed = statRegexp.cap(1).toInt();
        m_failed = statRegexp.cap(2).toInt();
        m_skipped = statRegexp.cap(3).toInt();
    }

    m_isParsed = true;
    emit parsed();
    return true;
}

int TestResultParser::passed() const
{
    return m_passed;
}

int TestResultParser::failed() const
{
    return m_failed;
}

int TestResultParser::skipped() const
{
    return m_skipped;
}

int TestResultParser::warning() const
{
    int warns = 0;
    foreach(TestCase *testCase, m_testCases)
        if (testCase->isWarning())
            ++warns;

    return warns;
}

QString TestResultParser::testName() const
{
    return m_testName;
}

QString TestResultParser::testConfig() const
{
    return m_testConfig;
}

bool TestResultParser::isParsed() const
{
    return m_isParsed;
}

QList<TestCase *> TestResultParser::testCases() const
{
    return m_testCases;
}

QList<TestCase *> TestResultParser::failedCases() const
{
    QList<TestCase *> fails;
    foreach(TestCase *testCase, m_testCases)
        if (testCase->isFailed())
            fails.append(testCase);

    return fails;
}


int TestResultParser::getReturn() const
{
    return m_return;
}

void TestResultParser::setReturn(int result)
{
    m_return = result;
}





bool TestResultParser::parseTestCase(const QString &rawResult)
{
    TestCase *testCase = new TestCase();
    if (testCase->parse(rawResult))
    {
        m_testCases.append(testCase);
    }
    else
    {
        delete testCase;
        return false;
    }
    
    return true;
}
