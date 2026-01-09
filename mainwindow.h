#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <DatabaseManager.h>
#include "deadlinechecker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    DatabaseManager *m_dbManager_;
    DeadlineChecker *m_deadline_checker;

    void setupUI();
    void loadTasks();

private slots:
    void on_addButton_clicked();
    void on_completeButton_clicked();
    void on_deleteButton_clicked();
    void on_refreshButton_clicked();
    void on_archiveButton_clicked();
    void on_deadlineFound(const QString &task_title);

public slots:
    void on_actionAbout_triggered();

  //  void on_MainWindow_iconSizeChanged(const QSize &iconSize);
};
#endif // MAINWINDOW_H
