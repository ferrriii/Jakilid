#ifndef TESTRESULTPARSER_H
#define TESTRESULTPARSER_H

#include <QObject>
class TestCase;

class TestResultParser : public QObject
{
    Q_OBJECT
public:
    explicit TestResultParser(QObject *parent = 0);
    ~TestResultParser();

    bool parse(const QByteArray &result);
    
    int passed() const;
    int failed() const;    
    int skipped() const;
    int warning() const;
    QString testName() const;
    QString testConfig() const;

    bool isParsed() const;
    
    QList<TestCase *> testCases() const;
    QList<TestCase *> failedCases() const;
    
    int getReturn() const;
    void setReturn(int result);

signals:
    //    void testFailed(TestCase *testCase);
//    void testPassed(TestCase *testCase);
    void parsed();
    
public slots:
    
private:
    QList<TestCase *> m_testCases;
    int m_passed;
    int m_failed;
    int m_skipped;
    QString m_testName;
    bool m_isParsed;
    QString m_testConfig;
    int m_return;
    
    bool parseTestCase(const QString &rawResult);
    
};

#endif // TESTRESULTPARSER_H
