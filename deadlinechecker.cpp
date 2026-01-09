#include "deadlinechecker.h"
#include "DatabaseManager.h"

#include <QDebug>
#include <QDateTime>

DeadlineChecker::DeadlineChecker(DatabaseManager *db_manager, QObject *parent)
    : QThread(parent)
    , dbManager_(db_manager)
    , m_running_(true)
{
}

void DeadlineChecker::stopChecking()
{
    m_running_ = false;
    quit();      // stop long_pool
    wait(1000);  // чутка подождем
}

void DeadlineChecker::run()
{
    checkDeadlines(); // proverka

    QTimer timer;
    connect(&timer, &QTimer::timeout, this, &DeadlineChecker::checkDeadlines);
    timer.start(30000);

    exec(); //long pool   
}

void DeadlineChecker::checkDeadlines()
{
    if (!dbManager_ || !m_running_)
    {
        return;
    }

    QList<Task> tasks_list = dbManager_->GetAllTasks();

    QDateTime current_datetime = QDateTime::currentDateTime();

    for(const auto &task : tasks_list)
    {
        if (!task.archived && !task.completed && task.deadline.isValid())
        {
            if (task.deadline > current_datetime && task.deadline <= current_datetime.addSecs(3600))
            {
                emit deadlineFound(task.title);
                qDebug() << "Срочная задача: " << task.title;
            }

            if (task.deadline < current_datetime)
            {
                qDebug() << "Просроченная задача: " << task.title;
            }
        }

    }
}
