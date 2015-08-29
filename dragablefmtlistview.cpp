#include "dragablefmtlistview.h"
#include "fmtlistmodel.h"
#include <QDrag>
#include <QDataStream>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QMapIterator>
#include <QHeaderView>
#include <QStandardItem>
#include "fmtobject.h"

DragableFmtListView::DragableFmtListView(QWidget *parent) :
    QListView(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
}

void DragableFmtListView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex indAt = indexAt(event->pos());
    QModelIndex index = static_cast<FmtListModel*>(model())->getId(indAt);
    if (event->button() == Qt::RightButton && index.isValid())
    {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << index.data().toInt() << indAt.data().toString();

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("fmt/names_id_title", data);

        mimeData->setText(index.data().toString());
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/table"));

        Qt::DropAction dropAction = drag->exec();
        Q_UNUSED(dropAction);
    }
    else
    {
        QListView::mousePressEvent(event);
    }
}

// ===============================================================
DropableFmtTreeView::DropableFmtTreeView(const QSqlDatabase &_db, QWidget *parent) :
    QTreeView(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setModel(&_model);

    db = _db;
    header()->hide();

    parentItem = _model.invisibleRootItem();

    connect(&_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(onItemDataChanged(QModelIndex,QModelIndex,QVector<int>)));
}

void DropableFmtTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("fmt/names_id_title"))
    {
        event->acceptProposedAction();
    }
}

void DropableFmtTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("fmt/names_id_title"))
    {
        event->acceptProposedAction();
    }
}

FmtObject *DropableFmtTreeView::object(const QStandardItem *item)
{
    return objects[(QStandardItem*)item];
}

void DropableFmtTreeView::onItemDataChanged(const QModelIndex &topLeft,
                                            const QModelIndex &bottomRight,
                                            const QVector<int> &roles)
{
    if (topLeft.data(Qt::UserRole + 2).toInt() == FLD_TABLE)
    {
        QStandardItem *item = _model.itemFromIndex(topLeft);

        int rows = item->rowCount();
        for (int i = 0; i < rows; i++)
        {
            item->child(i)->setCheckState(item->checkState());
        }
    }
}

QStandardItem *DropableFmtTreeView::itemFromIndex(const QModelIndex &index)
{
    return _model.itemFromIndex(index);
}

void DropableFmtTreeView::deleteObject(const QStandardItem *item)
{
    delete objects[const_cast<QStandardItem*>(item)];
    objects.erase(objects.find(const_cast<QStandardItem*>(item)));
}

void DropableFmtTreeView::addSql(const QString &sql)
{
    QTextStream stream((QString*)&sql);
    QStandardItem *item = new QStandardItem(stream.readLine());
    item->setIcon(QIcon(":/sql"));
    item->setData(FLD_SQL, Qt::UserRole + 2);
    item->setData(sql, Qt::UserRole + 3);
    parentItem->appendRow(item);
}

void DropableFmtTreeView::addTable(const qint32 &id, const QString &title)
{
    try
    {
        FmtObject *obj = new FmtObject(id, db);

        QStandardItem *item = new QStandardItem(title);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setIcon(QIcon((obj->isTmp() ? ":/tablet" : ":/table")));
        item->setData(FLD_TABLE, Qt::UserRole + 2);
        item->setCheckable(true);
        objects.insert(item, obj);

        QMapIterator<qint32, FmtField*> iterator(obj->getFields());
        while (iterator.hasNext())
        {
            FmtField *fld = iterator.next().value();
            QStandardItem *fldItem = new QStandardItem(fld->getName());
            fldItem->setData(fld->getId());
            fldItem->setData(FLD_FIELD, Qt::UserRole + 2);
            fldItem->setCheckable(true);

            item->appendRow(fldItem);
        }

        parentItem->appendRow(item);
    }
    catch(...)
    {

    }
}

void DropableFmtTreeView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("fmt/names_id_title"))
    {
        QByteArray data = event->mimeData()->data("fmt/names_id_title");
        QDataStream stream(&data, QIODevice::ReadOnly);

        QString title;
        int id;
        stream >> id >> title;

        addTable(id, title);
    }
}
