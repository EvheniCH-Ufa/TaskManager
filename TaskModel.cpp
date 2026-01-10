#include "TaskModel.h"

#include <QSqlRecord>
#include <QBrush>
#include <QFont>
#include <QDebug>

TaskModel::TaskModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
    , m_colorScheme(0)
{
    // Настраиваем таблицу

    setTable("tasks"); // nasleduem ot QSqlTableModel
   /* Связывает модель с конкретной таблицей в базе данных, работает с таблицей под названием tasks:
   - Модель теперь знает, из какой таблицы брать данные
   - Автоматически получает структуру таблицы (поля, типы)
   - Может выполнять SQL-запросы к этой таблице
   Это метод родительского класса*/

   setEditStrategy(QSqlTableModel::OnRowChange);
   // 1. OnRowChange - сохраняет при переходе на другую строку
   // Пользователь редактирует ячейку → уходит со строки → изменения сохраняются

   // 2. OnFieldChange - сохраняет сразу при редактировании каждой ячейки
   // Пользователь изменил значение → сразу запись в БД

   // 3. OnManualSubmit - ручное сохранение
   // Все изменения копятся, потом вызываем submitAll()

   setSort(ColDeadline, Qt::AscendingOrder);
   /*
    setSort(номер столбца, возрастание)
    Альтернативы сортировки:
        setSort(ColPriority, Qt::DescendingOrder);  // Приоритет по убыванию (5★ → 1★)
        setSort(ColCreated, Qt::DescendingOrder);   // Новые задачи сверху
        setSort(ColTitle, Qt::AscendingOrder);      // По алфавиту названия
    */

   // Устанавливаем заголовки
   setHeaderData(ColId, Qt::Horizontal, "ID");
   setHeaderData(ColTitle, Qt::Horizontal, "Название");
   setHeaderData(ColDescription, Qt::Horizontal, "Описание");
   setHeaderData(ColDeadline, Qt::Horizontal, "Дедлайн");
   setHeaderData(ColPriority, Qt::Horizontal, "Приоритет");
   setHeaderData(ColCompleted, Qt::Horizontal, "✓");
   setHeaderData(ColArchived, Qt::Horizontal, "📁");
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    // Получаем данные из БД
    QSqlRecord record = this->record(row);
    QDateTime deadline = record.value("deadline").toDateTime();
    bool completed = record.value("completed").toBool();
    int priority = record.value("priority").toInt();

    switch (role)
    {
        case Qt::DisplayRole:  // Основной текст
            switch (col)
            {
                case ColId:
                    return record.value("id");
                case ColTitle:
                    return record.value("title");
                case ColDescription:
                    return record.value("description");
                case ColDeadline:
                    return deadline.isValid() ? deadline.toString("dd.MM.yyyy HH:mm") : "Нет дедлайна";
                case ColPriority:
                    return priorityToStars(priority);
                case ColCompleted:
                    return completed ? "✓" : "❌";
                case ColArchived:
                    return completed ? "✓" : "❌";
            }
            break;

        case Qt::BackgroundRole: // ФОН
        {
            TaskStatus status = getTaskStatus(deadline, completed);
            return QBrush(getStatusColor(status));
        }

        case Qt::ForegroundRole: // Цвет букв
        {
            if (completed) {
                return QBrush(QColor(100, 100, 100)); // Серый для выполненных
            }
            return QBrush(Qt::black);
        }

        case Qt::FontRole: // Шрифт букв
        {
            TaskStatus status = getTaskStatus(deadline, completed);
            QFont font;
            if (status == StatusUrgent || status == StatusExpired)
            {
                font.setBold(true);
            }
            return font;
        }

        case Qt::TextAlignmentRole: // выравнивание
        {
        if (col == ColPriority || col == ColCompleted)
                return Qt::AlignCenter;
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }

        case Qt::ToolTipRole: // подсказка - hint
        {
            TaskStatus status = getTaskStatus(deadline, completed);
            QString tooltip = record.value("title").toString() + "\n";

            if (status == StatusExpired)
            {
                tooltip += "⚠ ПРОСРОЧЕНО";
            }
            else if (status == StatusUrgent)
            {
                tooltip += "🔥 СРОЧНО (менее 1 часа)";
            }
            else if (status == StatusSoon)
            {
                tooltip += "⚠ Скоро дедлайн (менее 24 часов)";
            }
            return tooltip;
        }
    }
    return QSqlTableModel::data(index, role);
}

QVariant TaskModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // дубляж с конструктором
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case ColId: return "ID";
            case ColTitle: return "Название";
            case ColDescription: return "Описание";
            case ColDeadline: return "Дедлайн";
            case ColPriority: return "Приоритет";
            case ColCompleted: return "✓";
            case ColArchived: return "📁";
        }
    }
    return QSqlTableModel::headerData(section, orientation, role);
}


// Вспомогательные методы
TaskModel::TaskStatus TaskModel::getTaskStatus(const QDateTime &deadline, bool completed) const
{

}
