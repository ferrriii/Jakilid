//#include <QtGui>

#include "testmodeitem.h"
#include "testlaunchermodel.h"
#include "testresultparser.h"


TestLauncherModel::TestLauncherModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TestModeltem();
//    addTestLauncher(testLauncher);
//    rootItem->appendChild(new TreeItem(testLauncher->results(), testLauncher->executionTime(), rootItem));
}


TestLauncherModel::~TestLauncherModel()
{
    delete rootItem;
}


int TestLauncherModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TestModeltem*>(parent.internalPointer())->columnCount();
    else
        return 9;//rootItem->columnCount();
}

void TestLauncherModel::addTestLauncher(TestLauncher *testLauncher)
{
    beginResetModel();
    m_testLaunchers.append(testLauncher);
    TestModeltem *item = new TestModeltem(testLauncher->results(), testLauncher->executionTime(), rootItem);

    if (!testLauncher->results()->isParsed())
        connect(item, SIGNAL(testCasesAdded()), this, SLOT(refreshTestCases()));

    connect(testLauncher, SIGNAL(finished()), this, SLOT(refreshTestResult()));

    rootItem->appendChild(item);
    testLauncher->setProperty("child", rootItem->childCount()-1);
    endResetModel();
}

QList<TestLauncher *> TestLauncherModel::testLaunchers() const
{
    return m_testLaunchers;
}

void TestLauncherModel::clear()
{
    beginResetModel();
    m_testLaunchers.clear();
    delete rootItem;
    rootItem = new TestModeltem();
    endResetModel();
}

void TestLauncherModel::refreshTestCases()
{
    //TODO: insert sender tree item children test cases
//    beginResetModel();
//    endResetModel();

//    TreeItem *parent = static_cast<TreeItem*> (sender());

//    beginInsertRows(createIndex(0,0,parent), 0, parent->childCount());
//    endInsertRows();
}

void TestLauncherModel::refreshTestResult()
{
    //TODO: update sender tree item
    beginResetModel();
    TestLauncher *parent = static_cast<TestLauncher*> (sender());
    rootItem->child(parent->property("child").toInt())->setExecTime( parent->executionTime());

    endResetModel();
}


QVariant TestLauncherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    TestModeltem *item = static_cast<TestModeltem*>(index.internalPointer());

    return item->data(index.column(), role);
}


Qt::ItemFlags TestLauncherModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant TestLauncherModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section) {
        case 0:
            return "Name";
        case 1:
            return "Config";
        case 2:
            return "Exec Time";
        case 3:
            return "Return";
        case 4:
            return "Passed";
        case 5:
            return "Failed";
        case 6:
            return "Skipped";
        case 7:
            return "Warning";
        case 8:
            return "Iteration time";
        default:
            return QVariant();
        }
    }
//        return rootItem->data(section);

    return QVariant();
}


QModelIndex TestLauncherModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TestModeltem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TestModeltem*>(parent.internalPointer());

    TestModeltem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}


QModelIndex TestLauncherModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TestModeltem *childItem = static_cast<TestModeltem*>(index.internalPointer());
    TestModeltem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}


int TestLauncherModel::rowCount(const QModelIndex &parent) const
{
    TestModeltem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TestModeltem*>(parent.internalPointer());

    return parentItem->childCount();
}

