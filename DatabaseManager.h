#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include<QtSql/QSqlDatabase>
#include<QDateTime>


struct Task
{
    int id = -1;
    QString title;
    QString description;
    QDateTime created;
    QDateTime deadline;
    int priority = 3;
    bool completed = false;
    bool archived = false;
};


class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    bool InitDatabase();
    bool AddTask(const QString& title, const QString& desc, const QDateTime& deadline, int priority);
    QList<Task> GetAllTasks();
    bool DeleteTask(int id);
    bool MarkCompleted(int id, bool completed);
    bool MarkArhived(int id, bool archived);

//signals:
private:
    QSqlDatabase m_db_;
    bool CreateTables();

};

#endif // DATABASEMANAGER_H
