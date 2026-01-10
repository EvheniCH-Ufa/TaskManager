#ifndef TASKMODEL_H
#define TASKMODEL_H

#include<QColor>
#include<QDateTime>
#include<QSqlTableModel>


class TaskModel : public QSqlTableModel
{
    Q_OBJECT
    Q_PROPERTY(int colorScheme READ colorScheme WRITE setColorScheme NOTIFY colorSchemeChanged)

public:
    enum Columns
    {
        ColId = 0, // id
        ColTitle,  // Title
        ColDescription, // desc
        ColDeadline,   // DeadLine
        ColPriority,   // Priority
        ColCompleted,  // Completed
        ColArchived,  // Archived
        ColCount       //
    };

    enum TaskStatus
    {
        StatusNormal,    // default
        StatusSoon,      // < 24 часа
        StatusUrgent,    // < 1 часа
        StatusExpired,   // prosrocheno
        StatusCompleted  // completed
    };

    enum ColorSchemes
    {
        StandartScheme = 0,
        DarkScheme
    };


    explicit TaskModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    ~TaskModel();


// Переопределяем методы модели
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const override;

    // Цветовые схемы
    int colorScheme() const;
    void setColorScheme(int scheme);

    // Фильтрация
    void filterByStatus(const QString &status);

signals:
    void colorSchemeChanged(int scheme);

private:
    int m_colorScheme;

    TaskStatus getTaskStatus(const QDateTime &deadline, bool completed) const;
    QColor getStatusColor(TaskStatus status) const;
    QString priorityToStars(int priority) const;
    void updateTaskColors();
};

#endif // TASKMODEL_H
