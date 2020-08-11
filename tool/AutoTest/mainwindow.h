#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QItemSelection>
#include <QMainWindow>

class TestLauncherModel;
class Movie;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QString m_currentDir;

    QStringList selectedTests();
    TestLauncherModel *m_testModel;

    QMovie *m_loadingMovie;

private slots:

    void showFailureDetail(QItemSelection,QItemSelection);
    void on_execTest_clicked();
    void on_selectTestFolders_clicked();
    void showTestsInDir(const QString &dir);
    void updateStat();
};

#endif // MAINWINDOW_H
