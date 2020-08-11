#include "testcase.h"
#include <QRegExp>
#include <QDebug>

TestCase::TestCase() : m_isValid(false), m_iterationSpeed(0)
{
}

bool TestCase::parse(const QString &rawResult)
{
    m_rawResult = rawResult;
    m_isValid = false;

//    qDebug() << "tttttt: " << rawResult;
    QRegExp configRegexp("(PASS|FAIL!|XFAIL|RESULT|SKIP|WARNING|QWARN|QFATAL|QDEBUG|QSYSTEM)\\s*:\\s+(.+)(\\s|\\r)", Qt::CaseInsensitive, QRegExp::RegExp2);
    configRegexp.setMinimal(true);
    if (configRegexp.indexIn(m_rawResult) < 0)
        return false;
    m_testCaseRet  = configRegexp.cap(1);
    m_testCaseName = configRegexp.cap(2);
//    qDebug() << "ret: " << m_testCaseRet;
    if (m_testCaseRet == "QWARN" || m_testCaseRet == "QDEBUG" || m_testCaseRet == "QSYSTEM")
        m_details =  rawResult.right(rawResult.length() - configRegexp.matchedLength()).trimmed();

    QRegExp failureRegexp("(FAIL!|XFAIL|SKIP|WARNING|QDEBUG|QSYSTEM)\\s*:\\s+([^\\s]+)\\s+([\\s\\S]+)\\n(.+)\\s+:\\s+failure location", Qt::CaseInsensitive, QRegExp::RegExp2);
    failureRegexp.setMinimal(true);
    if (failureRegexp.indexIn(m_rawResult) >= 0)
    {
        m_location = failureRegexp.cap(4).trimmed();
        m_details = failureRegexp.cap(3).trimmed();
    }


    QRegExp resultRegexp("([^\\s]+) msecs per iteration \\(total: \\d+, iterations: \\d+\\)", Qt::CaseInsensitive, QRegExp::RegExp2);
    if (resultRegexp.indexIn(m_rawResult) >= 0)
        m_iterationSpeed  = resultRegexp.cap(1).toFloat();

    m_isValid = true;
    return true;
}

bool TestCase::isFailed() const
{
    return m_testCaseRet == "FAIL!" || m_testCaseRet == "XFAIL" || m_testCaseRet == "QFATAL" || m_testCaseRet == "QSYSTEM";
}

bool TestCase::isPassed() const
{
    return !isFailed() && !isSkipped();
}

bool TestCase::isSkipped() const
{
    return m_testCaseRet == "SKIP";
}

bool TestCase::isWarning() const
{
    return m_testCaseRet == "WARNING" || m_testCaseRet == "QWARN";
}

bool TestCase::isBenchmark() const
{
    return m_testCaseRet == "RESULT";
}

QString TestCase::result() const
{
    return m_testCaseRet;
}

float TestCase::iterationSpeed() const
{
    return m_iterationSpeed;
}

QString TestCase::details() const
{
    return m_details;
}

QString TestCase::location() const
{
    return m_location;
}

QString TestCase::rawResults() const
{
    return m_rawResult;
}

bool TestCase::isValid() const
{
    return m_isValid;
}

QString TestCase::name() const
{
    return m_testCaseName;
}
