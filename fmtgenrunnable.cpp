#include "fmtgenrunnable.h"
#include <QTextStream>
#include "fmtobject.h"

FmtGenRunnable::FmtGenRunnable(const QModelIndex &index, const QSqlDatabase &db)
    :QObject(NULL), QRunnable()
{
    _index = index;
    _db = db;
    result = 0;
    setAutoDelete(true);
}

void FmtGenRunnable::run()
{
    QString cpp, tablessql, updscript;
    QTextStream stream(&cpp);
    QTextStream stream2(&tablessql);
    QTextStream stream3(&updscript);

    try
    {
        FmtObject obj(_index, _db);
        obj.generateCppCode(&stream);
        emit stepFinish(cpp, FmtGenRunnable::GEN_CPP);

        obj.generateSqls(&stream2);
        emit stepFinish(tablessql, FmtGenRunnable::GEN_TABLE);

        obj.generateUpdateScript(&stream3);
        emit stepFinish(updscript, FmtGenRunnable::GEN_UPD_SCRIPT);
    }
    catch(int &e)
    {
        result = e;
    }

    emit finished(result);
}
