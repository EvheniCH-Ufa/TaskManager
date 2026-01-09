#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>

namespace Ui {
class TaskDialog;
}

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(QWidget *parent = nullptr);
    ~TaskDialog();

    QString   GetTitle() const;
    QString   GetDescription() const;
    QDateTime GetDeadline() const;
    int       GetPriority() const;


private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::TaskDialog *ui;
};

#endif // TASKDIALOG_H
