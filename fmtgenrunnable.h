#ifndef FMTGENRUNNABLE_H
#define FMTGENRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QModelIndex>
#include <QSqlDatabase>

class FmtGenRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum GenStep
    {
        GEN_CPP = 1,
        GEN_TABLE,
        GEN_UPD_SCRIPT,
    };
    FmtGenRunnable(const QModelIndex &index, const QSqlDatabase &db);
    virtual void run();

signals:
    void finished(const qint16 &result);
    void stepFinish(const QString &text, const qint16 &step);

private:
    qint16 result;
    QSqlDatabase _db;
    QModelIndex _index;
};

#endif // FMTGENRUNNABLE_H
