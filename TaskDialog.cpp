#include "TaskDialog.h"
#include "ui_TaskDialog.h"
#include "QMessageBox"

TaskDialog::TaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskDialog)
{
    ui->setupUi(this);

    ui->deadlineEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    ui->deadlineEdit->setCalendarPopup(true);

    ui->prioritySpin->setRange(1, 5);
    ui->prioritySpin->setValue(3);
}


TaskDialog::~TaskDialog()
{
    delete ui;
}

QString TaskDialog::GetTitle() const
{
    return ui->titleEdit->text();
}

QString TaskDialog::GetDescription() const
{
    return ui->descEdit->toPlainText().trimmed();
}

QDateTime TaskDialog::GetDeadline() const
{
    return ui->deadlineEdit->dateTime();
}

int TaskDialog::GetPriority() const
{
    return ui->prioritySpin->value();
}

void TaskDialog::on_buttonBox_accepted()
{
    if (ui->titleEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Наименование не может быть пустым!");
        ui->titleEdit->setFocus();
        return;
    }
    accept();
}

void TaskDialog::on_buttonBox_rejected()
{
    reject();
}
