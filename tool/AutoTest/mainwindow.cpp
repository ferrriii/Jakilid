#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include "testlauncher.h"
#include "testresultparser.h"
#include "testcase.h"
#include "testlaunchermodel.h"
#include "testmodeitem.h"

#include <QDir>
#include <QFileDialog>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_loadingMovie = new QMovie(this);
    m_loadingMovie->setFileName(":/loading.gif");
    m_loadingMovie->setScaledSize(QSize(32,32));
    m_loadingMovie->start();


    m_testModel = new TestLauncherModel(this);
    ui->treeView->setModel(m_testModel);
    ui->treeView->setColumnWidth(0,210);

    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(showFailureDetail(QItemSelection,QItemSelection)));

    showTestsInDir("./test/");
}

MainWindow::~MainWindow()
{
    delete m_loadingMovie;
    qDeleteAll(m_testModel->testLaunchers());
    delete ui;
}

QStringList MainWindow::selectedTests()
{
    QStringList tests;

    for (int i=0; i<ui->lstTest->count(); ++i)
        if (ui->lstTest->item(i)->checkState() == Qt::Checked)
            tests.append(ui->lstTest->item(i)->text() );

    return tests;
}


void MainWindow::showFailureDetail(QItemSelection, QItemSelection)
{
    QModelIndexList selectIndex =  ui->treeView->selectionModel()->selectedRows();


    const QString htmlTemplate = "<span style=\"color:%1; font-family:Lucida Console; font-size:13px;\">%3:&nbsp;</span>"
            "<span style=\"color:#1010ee; font-family:Lucida Console; font-size:12px;\">%4</span>"
            "<pre style=\"color:#909090\">%5</pre>";

//    ui->txtTestDetail->clear();
    ui->txtIssues->clear();
    foreach (QModelIndex select, selectIndex)
    {
        TestModeltem *item = static_cast<TestModeltem*>(select.internalPointer());
        QList<TestCase *> testCases;
        if (item->testResult())
            testCases = item->testResult()->testCases();
        else if (item->testCase())
            testCases << item->testCase();

        foreach(TestCase *test, testCases)
            if (test->details() != "")
            {
                QString color;
                QColor alert = QColor(255,0,0);
                QColor warning = QColor(255,160,50);
                QColor notice = QColor(255,220,00);

                if (test->isFailed())
                    color = alert.name();
                else if (test->isSkipped() )
                    color = notice.name();
                else if (test->isWarning())
                    color = warning.name();
                QString n = test->result();
                for (int i = n.length(); i<7; ++i)
                    n.append("&nbsp;");


                ui->txtIssues->append(htmlTemplate
                                      .arg(color)
                                      .arg(n)
                                      .arg(test->location())
                                      .arg(test->details() + "\n\n") ); // "<font color='#1010ee'><pre>" + test->result() + "\t" + test->failureLocation() + "\n" + test->failureDetails() + "\n\n</font>"
            }
    }
}

void MainWindow::on_execTest_clicked()
{
    qDeleteAll(m_testModel->testLaunchers());
    m_testModel->clear();

    foreach (QString testExe, selectedTests())
    {
        TestLauncher *test = new TestLauncher(m_currentDir + testExe, this);
        m_testModel->addTestLauncher(test);
        connect(test, SIGNAL(finished()), this, SLOT(updateStat()));
        test->start();
    }
    updateStat();
}

void MainWindow::on_selectTestFolders_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    if (dialog.exec())
        showTestsInDir(dialog.selectedFiles().at(0));
}

void MainWindow::showTestsInDir(const QString &dir)
{
    m_currentDir = dir;
    QDir d(dir);
    QStringList testFiless = d.entryList(QStringList() << "*.exe", QDir::Files, QDir::Name);
    ui->lstTest->clear();
    foreach(QString file, testFiless)
    {
        QListWidgetItem* item = new QListWidgetItem(file, ui->lstTest);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Checked); // AND initialize check state
        ui->lstTest->addItem(item);
    }
    ui->lblSelectedTests->setText(QString::number(testFiless.count()));
}

void MainWindow::updateStat()
{
    ui->lblSelectedTests->setText(QString::number(selectedTests().count()));

    QList<TestLauncher *> localTestLaunchers = m_testModel->testLaunchers();

    bool isRunning = false;
    int skipped = 0;
    int passed = 0;
    int failed = 0;
    int warning = 0;
    foreach (TestLauncher *testLauncher, localTestLaunchers)
    {
        isRunning = isRunning || testLauncher->isRunning();

        skipped += testLauncher->results()->skipped();
        passed += testLauncher->results()->passed();
        failed += testLauncher->results()->failed() + (testLauncher->results()->failed() == 0 && testLauncher->results()->getReturn() != 0 ? 1 : 0);
        warning += testLauncher->results()->warning();
    }

    if (isRunning)
    {
        ui->lblResults->setText("");
        ui->lblResults->setMovie(m_loadingMovie);
    }
    else
    {
        ui->lblResults->setMovie(0);
        if (failed)
            ui->lblResults->setText("<font color=red>Failed!</font>");
        else
            ui->lblResults->setText("<font color=green>Passed</font>");
    }
    QApplication::processEvents();
    ui->lblState->setText(isRunning ? "Running" : "Idle");
    ui->lblPassed->setText(QString::number(passed));
    ui->lblFailed->setText(QString::number(failed));
    ui->lblSkipped->setText(QString::number(skipped));
    ui->lblWarning->setText(QString::number(warning));

    QColor alert = QColor(255,0,0);
    QColor warningC = QColor(255,160,50);
    QColor notice = QColor(255,220,00);

    if (failed > 0)
        ui->lblFailed->setStyleSheet("Color: " + alert.name());
    else
        ui->lblFailed->setStyleSheet("");
    if (skipped > 0)
        ui->lblSkipped->setStyleSheet("Color: " + notice.name());
    else
        ui->lblSkipped->setStyleSheet("");
    if (warning > 0)
        ui->lblWarning->setStyleSheet("Color: " + warningC.name());
    else
        ui->lblWarning->setStyleSheet("");

}
