#include "TaskModel.h"

#include <QSqlRecord>
#include <QBrush>
#include <QFont>
#include <QDebug>
#include <QSettings>


const int SECONDS_IN_HOUR = 60*60;
const int SECONDS_IN_DAY  = 60*60*24;
const int SCHEME_DEFAULT = 0; // normal

TaskModel::TaskModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
    , m_colorScheme(0)
{
    // Настраиваем таблицу

    setTable("tasks"); // nasleduem ot QSqlTableModel
   /* Связывает модель с конкретной таблицей в базе данных, в данном случае работает с таблицей под названием tasks:
   - Модель теперь знает, из какой таблицы брать данные
   - Автоматически получает структуру таблицы (поля, типы)
   - Может выполнять SQL-запросы к этой таблице
   Это метод родительского класса, мы его переопределяем*/

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


TaskModel::~TaskModel()
{
    saveSettings();
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
    if (completed)
    {
        return StatusCompleted;
    }

    if (!deadline.isValid())
    {
        return StatusNormal;
    }

    QDateTime currentDateTime = QDateTime::currentDateTime();
    qint64 secondToDeadLine = currentDateTime.secsTo(deadline);

    if (secondToDeadLine < 0)
    {
        return StatusExpired; // prosrocheno
    }
    else if (secondToDeadLine < SECONDS_IN_HOUR)
    {
        return StatusUrgent;
    }
    else if (secondToDeadLine < SECONDS_IN_DAY)
    {
        return StatusSoon;
    }
    return StatusNormal;
}

QColor TaskModel::getStatusColor(TaskModel::TaskStatus status) const
{
    switch (m_colorScheme)
    {
        case StandartScheme:
        {
            switch (status)
            {
                case StatusNormal: return QColor("#ffffff");
                case StatusSoon: return QColor("#fffacd");  // светло-желтый
                case StatusUrgent: return QColor("#ffcccb"); // светло-красный
                case StatusExpired: return QColor("#ffcccc");
                case StatusCompleted: return QColor("#d4edda"); // светло-зеленый
                default: return QColor("#ffffff");
            }
        }
        case DarkScheme:
        {
            switch (status)
            {
                case StatusNormal: return QColor("#2d3748");
                case StatusSoon: return QColor("#4a5568");
                case StatusUrgent: return QColor("#c53030");
                case StatusExpired: return QColor("#742a2a");
                case StatusCompleted: return QColor("#276749");
                default: return QColor("#2d3748");
            }
        }
        default: return QColor("#ffffff");
    }
}

QString TaskModel::priorityToStars(int priority) const
{
    QString result;
    result.reserve(5);
    for (int i = 0; i < 5; ++i)
    {
        result += (i < priority) ?  "★" : "☆";
    }
    return result;
}

void TaskModel::filterByStatus(const QString &status)
{
    if (status == "Все")
    {
        setFilter("");
    }
    else if (status == "Активные")
    {
        setFilter("completed = 0 AND archived = 0"); // добавить про архив
    }
    else if (status == "Просроченные")
    {
        QString filter = QString("completed = 0 AND archived = 0 AND deadline < '%1'")
                    .arg(QDateTime::currentDateTime().toString(Qt::ISODate));
        setFilter(filter);
    }
    else if (status == "Выполненные")
    {
        setFilter("completed = 1 AND arhived = 0" );
    }
    else if (status == "Архивные")
    {
        setFilter("archived = 1");
    }
    else if (status == "Срочные")
    {
        QString filter = QString("completed = 0 AND archived = 0 AND deadline > '%1' AND deadline < '%2'")
                    .arg(QDateTime::currentDateTime().toString(Qt::ISODate)
                       , QDateTime::currentDateTime().addSecs(SECONDS_IN_HOUR).toString(Qt::ISODate));
        setFilter(filter);
    }
    select(); // apply filter
}

int TaskModel::getColorScheme() const //13/01/2026
{
    return m_colorScheme;
}

void TaskModel::setColorScheme(int scheme) //13/01/2026
{
    if (m_colorScheme == scheme)
    {
        return;
    }

    m_colorScheme = scheme;
    emit colorSchemeChanged(m_colorScheme);

    // update view   -= indexes aka iterators =-
    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
}

void TaskModel::updateTaskColors() //13/01/2026
{
    if (rowCount() > 0)
    {
        emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1)); //13/01/2026
    }
}

void TaskModel::loadSettings() //13/01/2026
{
    int colorSheme = m_settings.value("ColorScheme", SCHEME_DEFAULT).toInt();
    m_colorScheme = static_cast<ColorSchemes>(colorSheme);
}

void TaskModel::saveSettings() //13/01/2026
{
    m_settings.setValue("ColorScheme", static_cast<int>(m_colorScheme));
}
