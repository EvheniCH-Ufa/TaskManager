#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <DatabaseManager.h>
#include "deadlinechecker.h"
#include <TaskDialog.h>
#include <QDateTime>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());

    {
       qDebug() << "Settings file:" << settings.fileName();

       if (settings.contains("geometry"))
       {
           QByteArray geoData = settings.value("geometry").toByteArray();
           qDebug() << "Geometry data size:" << geoData.size();
           bool restored = restoreGeometry(geoData);
           qDebug() << "Restore successful:" << restored;
           qDebug() << "After restore size:" << size();
       }
       else
       {
           qDebug() << "No geometry saved in settings";
       }
    }

  //  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::on_actionAbout_triggered); связалась автоматом. если оставить, то дважды выходит

    m_dbManager_ = new DatabaseManager(this);
    m_dbManager_->InitDatabase();
    loadTasks();

    m_deadline_checker = new DeadlineChecker(m_dbManager_, this);
    connect(m_deadline_checker, &DeadlineChecker::deadlineFound, this, &MainWindow::on_deadlineFound);
    m_deadline_checker->start();

    setupUI();
}

MainWindow::~MainWindow()
{
    if (m_deadline_checker != nullptr && m_deadline_checker->isRunning())
    {
        m_deadline_checker->quit();
        m_deadline_checker->wait();
    }

    delete ui;
}

void MainWindow::setupUI()
{
    ui->m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->m_tableWidget->setColumnWidth(0,  50);
    ui->m_tableWidget->setColumnWidth(1, 150);
    ui->m_tableWidget->setColumnWidth(2, 250);
    ui->m_tableWidget->setColumnWidth(3, 100);
    ui->m_tableWidget->setColumnWidth(4,  80);
    ui->m_tableWidget->setColumnWidth(5,  65);
}

void MainWindow::loadTasks()
{
   ui->m_tableWidget->setRowCount(0);

   QList<Task> tasks = m_dbManager_->GetAllTasks();

   for (const Task &task : tasks)
   {
       int row = ui->m_tableWidget->rowCount();
       ui->m_tableWidget->insertRow(row);

       ui->m_tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(task.id)));
       ui->m_tableWidget->setItem(row, 1, new QTableWidgetItem(task.title));
       ui->m_tableWidget->setItem(row, 2, new QTableWidgetItem(task.description));

       QString deadline_str = task.deadline.toString("dd.MM.yyyy HH:mm");
       ui->m_tableWidget->setItem(row, 3, new QTableWidgetItem(deadline_str));

       QString priority_str;
       for (int i = 0; i < 5; i++) {
           priority_str += (i < task.priority) ? "★" : "☆";
       }
       ui->m_tableWidget->setItem(row, 4, new QTableWidgetItem(priority_str));

       //QString archived_str = ;
       ui->m_tableWidget->setItem(row, 5, new QTableWidgetItem(task.archived ? "✅" : "❌"));

       //QColor background_color = QTableWidgetItem::backgroundColor();
       //QColor background_color = ui->m_tableWidget->item(row, 0)->backgroundColor();
       QColor background_color = QColor(255, 255, 255);

       if (task.completed)
       {
           // Закрашиваем строку зеленоватым если выполнена
           background_color = QColor(199, 243, 207);
       }
       else if (task.archived)
       {
           // Закрашиваем строку серым архивный
           background_color = QColor(220, 220, 220);
       }
       else if (task.deadline < QDateTime::currentDateTime())
       {
           // Закрашиваем строку розовым просрочка
           background_color = QColor(255, 200, 200);
       }

       // Раскрашиваем строку
       for (int col = 0; col < 6; col++)
       {
           ui->m_tableWidget->item(row, col)->setBackground(background_color);
         //  ui->m_tableWidget->item(row, col)->setForeground(QColor(185, 240, 195));
       }
   }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());


  /*  QByteArray geo = saveGeometry();
        qDebug() << "Saving geometry, size:" << geo.size();

        QSettings settings;
        settings.setValue("geometry", geo);
        settings.sync(); // Принудительно записываем

        qDebug() << "Saved to:" << settings.fileName();

*/
    QMainWindow::closeEvent(event);
}

void MainWindow::on_addButton_clicked()
{
    TaskDialog dialog(this);

    if(dialog.exec() == QDialog::Accepted)
    {
        QString title = dialog.GetTitle();
        QString desc  = dialog.GetDescription();
        QDateTime deadline = dialog.GetDeadline();
        int priority = dialog.GetPriority();

        if (title.isEmpty())
        {
            QMessageBox::warning(this, "Ошибка", "Наименование задачи не может быть пустым!");
            return;
        }

        if (m_dbManager_->AddTask(title, desc, deadline, priority))
        {
            QMessageBox::information(this, "Успех", "Новая задача успешно добавлена!");
            loadTasks();
        }
        else
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось добавить новую задачу!");
        }
    }
}

void MainWindow::on_completeButton_clicked()
{
    int row = ui->m_tableWidget->currentRow();
    if (row < 0)
    {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для отметки");
        return;
    }
    int id = ui->m_tableWidget->item(row, 0)->text().toInt();

    if (m_dbManager_->MarkCompleted(id, true))
    {
        // Закрашиваем строку зеленоватым
        for (int col = 0; col < 6; col++) {
            ui->m_tableWidget->item(row, col)->setBackground(QColor(199, 243, 207));
           // ui->m_tableWidget->item(row, col)->setForeground(QColor(185, 240, 195));
        }
        QMessageBox::information(this, "Успех", "Задача отмечена как выполненная!");
    }
}

void MainWindow::on_deleteButton_clicked()
{
    int row = ui->m_tableWidget->currentRow();
    if (row < 0)
    {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для удаления");
        return;
    }

    int id = ui->m_tableWidget->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить выбранную задачу?") == QMessageBox::Yes)
    {
        if (m_dbManager_->DeleteTask(id))
        {
            loadTasks();
        }
    }
}

void MainWindow::on_refreshButton_clicked()
{
    loadTasks();

   /* QString msg_str = "";
    for (int i = 0; i < ui->m_tableWidget->columnCount(); ++i)
    {
        msg_str += "; " + QString::number(i) + "= " + QString::number(ui->m_tableWidget->columnWidth(i));
    }

    ui->statusBar->showMessage(msg_str);*/ // это для проверки ширины столбцов
}

void MainWindow::on_archiveButton_clicked()
{
    int row = ui->m_tableWidget->currentRow();
    if (row < 0)
    {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для архивации");
        return;
    }
    int id = ui->m_tableWidget->item(row, 0)->text().toInt();

    if (m_dbManager_->MarkArhived(id, true))
    {
        // Закрашиваем строку серым архивный
        for (int col = 0; col < 6; col++) {
            ui->m_tableWidget->item(row, col)->setBackground(QColor(220, 220, 220));
           // ui->m_tableWidget->item(row, col)->setForeground(QColor(150, 150, 150));
        }
        QMessageBox::information(this, "Успех", "Задача отмечена как выполненная!");
    }
}

void MainWindow::on_deadlineFound(const QString &task_title)
{
     statusBar()->showMessage("⚠ Срочно: " + task_title, 5000);
     QMessageBox::warning(this, "Срочная задача!", "Задача " + task_title + " скоро дедлайн!");
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "Информация", "О программе:\nтестовая программа с БД, многопоточностью.\n----------------");
}
