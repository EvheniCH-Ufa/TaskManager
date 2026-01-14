#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QComboBox>


#include "DatabaseManager.h"
#include "deadlinechecker.h"
#include "TaskModel.h"


QT_BEGIN_NAMESPACE
    namespace Ui { class MainWindow; }

    class QAction;
    class QMenu;
    class QSystemTrayIcon;
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



  //  void setupUI();
    void setupMenu();
    void setupToolbar();
    void setupConnections();
    void setupTrayIcon();
    void setupShortcuts();

    void loadSettings();
    void saveSettings();

    // Модели
    TaskModel *m_taskModel;
    QSortFilterProxyModel *m_proxyModel;

    // Виджеты
    QTableView *m_tableView;
    QComboBox *m_filterCombo;
    QComboBox *m_colorSchemeCombo;

    // Действия
    QAction *m_actionNew;
    QAction *m_actionEdit;
    QAction *m_actionDelete;
    QAction *m_actionExport;
    QAction *m_actionExit;

    // Системный трей
    QSystemTrayIcon *m_trayIcon;

    // Сортировка
    int m_lastSortColumn;
    Qt::SortOrder m_lastSortOrder;
    QList<int> m_sortColumns;

    void applyMultiSort();












private slots:
    void on_addButton_clicked();
    void on_completeButton_clicked();
    void on_deleteButton_clicked();
    void on_refreshButton_clicked();
    void on_archiveButton_clicked();
    void on_deadlineFound(const QString &task_title);



  //  void onAddTask();
    void onEditTask();
  //  void onDeleteTask();
 //   void onMarkCompleted();
    void onExportCSV();
    void onFilterChanged(int index);
    void onColorSchemeChanged();
    void onShowContextMenu(const QPoint &pos);
    void onTaskDoubleClicked(const QModelIndex &index);
    void onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    // Обработчики горячих клавиш
    void onNewTaskShortcut();
    void onEditTaskShortcut();
    void onDeleteTaskShortcut();
    void onSaveShortcut();
    void onFindShortcut();





public slots:
    void on_actionAbout_triggered();

  //  void on_MainWindow_iconSizeChanged(const QSize &iconSize);
};
#endif // MAINWINDOW_H
