#ifndef FMTLISTMODEL_H
#define FMTLISTMODEL_H

#include <QObject>
#include <QSqlQueryModel>
#include <QIcon>

class FmtListModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    FmtListModel(QSqlDatabase *_db, QObject *parent = 0);

public slots:
    void updateFmtModel();

signals:
    void findItemSelection(const QModelIndex &item);

public slots:
    void findFirst(const QString &value);
    void findNext();

protected:
    virtual QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;

private:
    QSqlDatabase *db;
    QIcon tableIcon, tmpTableIcon, tableIconFound, tmpTableIconFound;

    QModelIndexList matchData;
    int curFind;
};

#endif // FMTLISTMODEL_H
