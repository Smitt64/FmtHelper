#ifndef DRAGABLEFMTLISTVIEW_H
#define DRAGABLEFMTLISTVIEW_H

#include <QObject>
#include <QMouseEvent>
#include <QListView>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSqlDatabase>

class DragableFmtListView : public QListView
{
    Q_OBJECT
public:
    DragableFmtListView(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
};

class FmtObject;
class DropableFmtTreeView : public QTreeView
{
    Q_OBJECT
public:
    enum
    {
        FLD_TABLE,
        FLD_FIELD,
        FLD_SQL
    };
    DropableFmtTreeView(const QSqlDatabase &_db, QWidget *parent = 0);
    virtual ~DropableFmtTreeView() {}

    void addTable(const qint32 &id, const QString &title);
    void addSql(const QString &sql);
    FmtObject *object(const QStandardItem *item);
    void deleteObject(const QStandardItem *item);
    QStandardItem *itemFromIndex(const QModelIndex &index);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void onItemDataChanged(const QModelIndex &topLeft,
                     const QModelIndex &bottomRight,
                     const QVector<int> &roles);

private:
    QStandardItem *parentItem;
    QStandardItemModel _model;

    QMap<QStandardItem*, FmtObject*> objects;

    QSqlDatabase db;
};

#endif // DRAGABLEFMTLISTVIEW_H
