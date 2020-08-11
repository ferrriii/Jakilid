#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QRegExp>
#include <QFile>
#include <QDebug>
#include <QWebFrame>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDir dir("../ui/gui/UI");
    ui->webView->setUrl(QUrl::fromLocalFile(dir.absoluteFilePath("index.html")));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QDir dir("../ui/gui/UI/js");
    QStringList files = dir.entryList(QDir::Files);

    foreach (QString file, files)
    {
        QFile f(dir.absoluteFilePath(file));
        f.open(QIODevice::ReadOnly| QIODevice::Text);
        extractTr(f.readAll());
        f.close();
    }
    ui->txtTr->setPlainText(
                ui->webView->page()->mainFrame()->evaluateJavaScript("locale.generateLangScript();").toString()
                );

}

void MainWindow::extractTr(const QString &str)
{
    int pos = 0;
    QRegExp trReg("\\btr\\([^)]+\\)",Qt::CaseInsensitive, QRegExp::RegExp2);
    trReg.setMinimal(true);
    while ((pos = trReg.indexIn(str, pos)) != -1) {
         ui->txtTr->appendPlainText(trReg.cap(0));
         ui->webView->page()->mainFrame()->evaluateJavaScript(trReg.cap(0));
         pos += trReg.matchedLength();
     }
}
