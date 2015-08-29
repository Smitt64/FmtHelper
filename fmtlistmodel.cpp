#include "fmtlistmodel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QIcon>

FmtListModel::FmtListModel(QSqlDatabase *_db, QObject *parent) :
    QSqlQueryModel(parent)
{
    db = _db;
    tableIcon = QIcon(":/table");
    tableIconFound = QIcon(":/tablefound");

    tmpTableIcon = QIcon(":/tablet");
    tmpTableIconFound = QIcon(":/tmptablefound");
}

void FmtListModel::updateFmtModel()
{
    setQuery("select T_NAME, T_COMMENT, T_ID from FMT_NAMES", *db);
    while (canFetchMore())
        fetchMore();
}

void FmtListModel::findFirst(const QString &value)
{
    matchData.clear();
    curFind = -1;
    matchData  = match(index(0, 0), Qt::DisplayRole, value, -1, Qt::MatchContains);

    if (matchData.size() > 0)
    {
        curFind = 0;
        emit findItemSelection(matchData[0]);
    }
}

void FmtListModel::findNext()
{
    if (curFind < 0)
        return;

    if (curFind < matchData.size() - 1)
    {
        curFind ++;
        emit findItemSelection(matchData[curFind]);
    }
    else
    {
        curFind = 0;
    }
}

QModelIndex FmtListModel::getId(const QModelIndex &index) const
{
    return index.model()->index(index.row(), 2);
}

Qt::ItemFlags FmtListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QSqlQueryModel::flags(index);

    /*if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;*/
    return defaultFlags;
}

QVariant FmtListModel::data(const QModelIndex &item, int role) const
{
    if (item.column() != 0)
    {
        return QSqlQueryModel::data(item, role);
    }

    if (role == Qt::DisplayRole)
    {
        return QString("%1 (%2)")
                .arg(record(item.row()).value(0).toString())
                .arg(record(item.row()).value(1).toString());
    }

    if (role == Qt::StatusTipRole || role == Qt::WhatsThisRole)
    {
        return record(item.row()).value(1).toString();
    }

    if (role == Qt::DecorationRole)
    {
        QString table = record(item.row()).value(0).toString();

        if (table.contains("_dbt", Qt::CaseInsensitive))
        {
            if (matchData.contains(item))
            {
                return (QVariant)tableIconFound;
            }
            else
            {
                return (QVariant)tableIcon;
            }
        }
        else
        {
            if (matchData.contains(item))
            {
                return (QVariant)tmpTableIconFound;
            }
            else
            {
                return (QVariant)tmpTableIcon;
            }
        }
    }

    return QSqlQueryModel::data(item, role);
}
