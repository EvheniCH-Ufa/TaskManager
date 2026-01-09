#ifndef DEADLINECHECKER_H
#define DEADLINECHECKER_H

#include<QThread>
#include<QTimer>
#include"DatabaseManager.h"



class DeadlineChecker : public QThread
{
    Q_OBJECT

public:
    explicit DeadlineChecker(DatabaseManager *db_manager, QObject *parent = nullptr);
    void stopChecking();


signals:
    void deadlineFound(const QString &task_title);

protected:
    void run() override;

private slots:
    void checkDeadlines();


private:
    DatabaseManager *dbManager_;
    bool m_running_;
//    QTimer *m_timer_;

};

#endif // DEADLINECHECKER_H
