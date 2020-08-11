/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treeitem.cpp

    A container for items of data supplied by the simple tree model.
*/


#include <QDebug>
#include <QColor>
#include "testmodeitem.h"

#include "testresultparser.h"
#include "testcase.h"




//! [0]
TestModeltem::TestModeltem(TestModeltem *parent) : QObject(parent), m_testResult(0), m_testCase(0)
{
    parentItem = parent;
}

TestModeltem::TestModeltem(TestResultParser *testResult, qint64 execTime, TestModeltem *parent) : QObject(parent), m_testResult(0), m_testCase(0)
{
    parentItem = parent;
    m_testResult = testResult;
    m_execTime = execTime;

    if (m_testResult->isParsed())
        addTestCases();
    else
        connect(m_testResult, SIGNAL(parsed()), this, SLOT(addTestCases()));
}

TestModeltem::TestModeltem(TestCase *testCase, TestModeltem *parent) : QObject(parent), m_testResult(0), m_testCase(0)
{
    parentItem = parent;
    m_testCase = testCase;
}
//! [0]

//! [1]
TestModeltem::~TestModeltem()
{
    qDeleteAll(childItems);
}
//! [1]

//! [2]
void TestModeltem::appendChild(TestModeltem *item)
{
    childItems.append(item);
}
//! [2]

//! [3]
TestModeltem *TestModeltem::child(int row)
{
    return childItems.value(row);
}
//! [3]

//! [4]
int TestModeltem::childCount() const
{
    return childItems.count();
}
//! [4]

//! [5]
int TestModeltem::columnCount() const
{
    return 9;//itemData.count();
}
//! [5]

//! [6]
QVariant TestModeltem::data(int column, int role) const
{
    if (!m_testCase && !m_testResult)
        return QVariant();

    QVariant value;
    if (role == Qt::DisplayRole)
    {
        switch (column) {
        case 0:
            value = (m_testCase ? m_testCase->name() : m_testResult->testName());
            if (m_testResult && !m_testResult->isParsed())
                value = "Not Parsed";
            break;
        case 1:
            value = (m_testCase ? "-" : m_testResult->testConfig());
            if (m_testResult && !m_testResult->isParsed())
                value = "Not Parsed";
            break;
        case 2:
            value = (m_testCase ? 0 : m_execTime);
            if (m_testResult && !m_testResult->isParsed())
                value = "";
            break;
        case 3:
            value = (m_testCase ? 0 : m_testResult->getReturn());
            if (m_testResult && !m_testResult->isParsed())
                value = "";
            break;
        case 4:
            value = (m_testCase ? m_testCase->isPassed() : m_testResult->passed());
            if (m_testResult && !m_testResult->isParsed())
                value = "";
            break;
        case 5:
            value = (m_testCase ? m_testCase->isFailed() : m_testResult->failed());
            if (m_testResult && !m_testResult->isParsed())
                value = "";
            break;
        case 6:
            value = (m_testCase ? m_testCase->isSkipped() : m_testResult->skipped());
            if (m_testResult && !m_testResult->isParsed())
                value = "";
            break;
        case 7:
            value = (m_testCase ? m_testCase->isWarning() : m_testResult->warning());
            if (m_testResult && !m_testResult->isParsed())
                value = "";
            break;
        case 8:
            value = (m_testCase ? m_testCase->iterationSpeed() : 0 );
            if (m_testResult && !m_testResult->isParsed())
                value = "";
            break;
        }

    }

    if (role == Qt::ForegroundRole)
    {
        QColor alert = QColor(255,0,0);
        QColor warning = QColor(255,160,50);
        QColor notice = QColor(255,220,00);
        QColor ok = QColor(0,200,0);

        if (column == 6 && data(6, Qt::DisplayRole).toInt() > 0)
            value = notice;
        if (column == 7 && data(7, Qt::DisplayRole).toInt() > 0)
            value = warning;

        if (column == 2 && m_testCase == 0)
        {
            int execTime = data(2, Qt::DisplayRole).toInt();
            if (execTime > 10000)
                value = QVariant::fromValue(alert);
            else if (execTime > 5000)
                value = QVariant::fromValue(warning);
            else if (execTime > 1000)
                value = QVariant::fromValue(notice);
            else
                value = QVariant::fromValue(ok);
        }

        if (column == 3 && m_testResult)
        {
            if (data(3,Qt::DisplayRole) != 0)
                value = QVariant::fromValue(alert);
        }

        if (column == 4 && data(4, Qt::DisplayRole).toInt() > 0 && data(5, Qt::DisplayRole).toInt() == 0)
                value = QVariant::fromValue(ok);
        if (column == 5 && data(5, Qt::DisplayRole).toInt() > 0 )
                value = QVariant::fromValue(alert);
    }

    return value;
}
//! [6]

//! [7]
TestModeltem *TestModeltem::parent()
{
    return parentItem;
}
qint64 TestModeltem::execTime() const
{
    return m_execTime;
}

void TestModeltem::setExecTime(const qint64 &execTime)
{
    m_execTime = execTime;
}
TestResultParser *TestModeltem::testResult() const
{
    return m_testResult;
}

//void TreeItem::setTestResult(TestResultParser *testResult)
//{
//    m_testResult = testResult;
//}
TestCase *TestModeltem::testCase() const
{
    return m_testCase;
}

//void TreeItem::setTestCase(TestCase *testCase)
//{
//    m_testCase = testCase;
//}




void TestModeltem::addTestCases()
{
    QList<TestCase *> testCases =  m_testResult->testCases();
    foreach(TestCase *testCase, testCases)
        childItems.append(new TestModeltem(testCase, this));

    if (testCases.count() >0)
        emit testCasesAdded();
}
//! [7]

//! [8]
int TestModeltem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TestModeltem*>(this));

    return 0;
}
//! [8]
