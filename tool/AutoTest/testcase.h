#ifndef TESTCASE_H
#define TESTCASE_H

#include <QString>

class TestCase
{
public:
    TestCase();

    bool parse(const QString &rawResult);
    bool isFailed() const;
    bool isPassed() const;
    bool isSkipped() const;
    bool isWarning() const;
    bool isBenchmark() const;
    QString result() const;
    float  iterationSpeed() const;
    QString details() const;
    QString location() const;
    QString rawResults() const;
    bool isValid() const;
    QString name() const;

private:
    QString m_rawResult;
    bool m_isValid;
    float m_iterationSpeed;
    QString m_testCaseRet;
    QString m_testCaseName;

    QString m_details;
    QString m_location;
};

#endif // TESTCASE_H
