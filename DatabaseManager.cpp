#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

bool DatabaseManager::InitDatabase()
{
    m_db_ = QSqlDatabase::addDatabase("QSQLITE");
    m_db_.setDatabaseName("tasks.db");

    if (!m_db_.open())
    {
        qDebug() << "DB error: " << m_db_.lastError().text();
        return  false;
    }
    return  CreateTables();
}

bool DatabaseManager::CreateTables()
{
    QSqlQuery query;
    QString sql = R"(
           CREATE TABLE IF NOT EXISTS tasks (
               id INTEGER PRIMARY KEY AUTOINCREMENT,
               title TEXT NOT NULL,
               description TEXT,
               created DATETIME DEFAULT CURRENT_TIMESTAMP,
               deadline DATETIME,
               priority INTEGER DEFAULT 3,
               completed BOOLEAN DEFAULT 0,
               archived BOOLEAN DEFAULT 0
           )
       )";

   return query.exec(sql);
}

bool DatabaseManager::AddTask(const QString &title, const QString &desc, const QDateTime &deadline, int priority)
{
    QSqlQuery query;
    query.prepare(R"(
           INSERT INTO tasks (title, description, deadline, priority)
           VALUES (:title, :desc, :deadline, :priority)
       )");

    query.bindValue(":title", title);
    query.bindValue(":desc", desc);
    query.bindValue(":deadline", deadline);
    query.bindValue(":priority", priority);

    return query.exec();
}

QList<Task> DatabaseManager::GetAllTasks()
{
    QList<Task> result;

    QSqlQuery query;

    if (! query.exec("SELECT * FROM tasks ORDER BY deadline"))
    {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        QString sss = query.lastError().text();
        return result;
    }

    while (query.next())
    {
        Task task;
        task.id = query.value("id").toInt();
        task.title = query.value("title").toString();
        task.description = query.value("description").toString();
        task.created = query.value("created").toDateTime();
        task.deadline = query.value("deadline").toDateTime();
        task.priority = query.value("priority").toInt();
        task.completed = query.value("completed").toBool();
        task.archived = query.value("archived").toBool();

        result.append(task);
    }
    return result;
}

bool DatabaseManager::DeleteTask(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM tasks WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec();
}

bool DatabaseManager::MarkCompleted(int id, bool completed)
{
    QSqlQuery query;
    query.prepare("UPDATE tasks SET completed = :completed WHERE id = :id AND archived = false");
    query.bindValue(":id", id);
    query.bindValue(":completed", completed);

    bool result = query.exec();

/*    if (query.numRowsAffected() > 0) {
            qDebug() << "Статус задачи" << id << "обновлен на" << completed;
            return true;
        } else {
            // Либо задача не найдена, либо она уже в архиве
            qDebug() << "Задача" << id << "не найдена или заархивирована";
            return false;
        }
  */

    return result;
}

bool DatabaseManager::MarkArhived(int id, bool archived)
{
    QSqlQuery query;
    query.prepare("UPDATE tasks SET archived = :archived WHERE id = :id  AND completed = false");
    query.bindValue(":id", id);
    query.bindValue(":archived", archived);

    return query.exec();
}

