#include "customupdatescript.h"
#include "fmtobject.h"
#include "sqlcodedialog.h"
#include <QStandardItemModel>

CustomUpdateScript::CustomUpdateScript(const QSqlDatabase &db, QWidget *parent) :
    QWidget(parent)
{
    tree = new DropableFmtTreeView(db);
    tree->setGeometry(10, 10, 200, 320);

    clearButton = new QPushButton(tr("Очистить"));
    delButton = new QPushButton(tr("Удалить"));
    genButton = new QPushButton(tr("Создать"));
    addSqlButton = new QPushButton(tr("Добавить свой SQL"));

    buttons1 = new QHBoxLayout();
    buttons2 = new QVBoxLayout();

    buttons1->addWidget(delButton);
    buttons1->addWidget(clearButton);

    buttons2->addWidget(tree);
    buttons2->addLayout(buttons1);
    buttons2->addWidget(addSqlButton);
    buttons2->addWidget(genButton);

    editor = new CodeEditor();
    highlight = new SqlHighlighter(editor->document());
    editor->setReadOnly(true);

    mainl = new QHBoxLayout();
    mainl->addLayout(buttons2);
    mainl->addWidget(editor);
    mainl->setStretch(0, 0);
    mainl->setStretch(1, 1);

    setLayout(mainl);

    connect(genButton, SIGNAL(clicked(bool)), SLOT(generate()));
    connect(delButton, SIGNAL(clicked(bool)), SLOT(deleteTable()));
    connect(addSqlButton, SIGNAL(clicked(bool)), SLOT(addSql()));
}

CustomUpdateScript::~CustomUpdateScript()
{
}

void CustomUpdateScript::addSql()
{
    SqlCodeDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted)
    {
        tree->addSql(dlg.getEditor()->toPlainText());
    }
}

void CustomUpdateScript::deleteTable()
{
    if (!tree->currentIndex().isValid())
        return;

    QModelIndex index = tree->currentIndex();
    QStandardItemModel *model = static_cast<QStandardItemModel*>(tree->model());
    QStandardItem *item = tree->itemFromIndex(index);

    if (item->data().toInt() == DropableFmtTreeView::FLD_TABLE ||
            item->data().toInt() == DropableFmtTreeView::FLD_SQL)
    {
        tree->deleteObject(item);
        model->removeRow(index.row(), index.parent());
    }
}

void CustomUpdateScript::generate()
{
    QString str;
    QTextStream stream(&str);

    stream << "BEGIN" << endl;

    QStandardItem *root = static_cast<QStandardItemModel*>(tree->model())->invisibleRootItem();

    for (int i = 0; i < root->rowCount(); i++)
    {
        QStandardItem *table = root->child(i);
        FmtObject *obj = tree->object(table);

        if (table->data(Qt::UserRole + 2) == DropableFmtTreeView::FLD_TABLE && table->hasChildren())
        {
            QString alter = QString("ALTER TABLE \n\t%1\nADD\n(\n").arg(obj->getName().toUpper());
            QString update = QString("UPDATE %1\nSET\n").arg(obj->getName().toUpper());

            int fc = 0;
            for (int fi = 0; fi < table->rowCount(); fi ++)
            {
                QStandardItem *row = table->child(fi);

                if (row->checkState() == Qt::Checked)
                {
                    qint32 id = row->data().toInt();
                    FmtField *f = obj->getField(id);

                    if (!f)
                        continue;

                    if (fc != 0)
                    {
                        alter += ",\n";
                        update += ",\n";
                    }

                    alter += QString("\t%1 %2").arg(f->getOraName(), f->getOraDecl());
                    update += QString("\t%1 = %2").arg(f->getOraName(), f->getOraDefaultVal());
                    fc ++;
                }
            }
            alter += "\n)";

            stream << "\tBEGIN" << endl;
            stream << "\t\tEXECUTE IMMEDIATE " << endl;

            QTextStream alterStream(&alter);
            while(!alterStream.atEnd())
            {
                QString s = alterStream.readLine();
                stream << "\t\t\t'" << s << "' ";

                if (s.at(0) == ')')
                {
                    stream << ";" << endl;
                }
                else
                {
                    stream << " ||" << endl;
                }
            }

            stream << "\tEXCEPTION" << endl;
            stream << "\t\tWHEN OTHERS THEN NULL;" << endl;
            stream << "\tEND;" << endl << endl;

            QTextStream updateStream(&update);
            stream << "\tBEGIN" << endl;
            stream << "\t\tEXECUTE IMMEDIATE " << endl;

            while(!updateStream.atEnd())
            {
                QString s = updateStream.readLine();
                stream << "\t\t\t'" << s << "' ";

                if (s.at(0) == ')')
                {
                    stream << ";" << endl;
                }
                else
                {
                    stream << " ||" << endl;
                }
            }

            stream << "\tEXCEPTION" << endl;
            stream << "\t\tWHEN OTHERS THEN NULL;" << endl;
            stream << "\tEND;" << endl << endl;
        }
        else if (table->data(Qt::UserRole + 2) == DropableFmtTreeView::FLD_SQL)
        {
            QString Sql = table->data(Qt::UserRole + 3).toString();
            Sql = Sql.replace("'", "''");
            Sql.append("\n");
            Sql.append(QChar(1));

            QTextStream sqlStream(&Sql);

            stream << "\tBEGIN" << endl;
            stream << "\t\tEXECUTE IMMEDIATE " << endl;
            while (!sqlStream.atEnd())
            {
                QString str2 = sqlStream.readLine();

                if (str2.length() && str2.at(0) == QChar(1))
                {
                    stream << ";" << endl;
                }
                else
                {
                    stream << "\t\t\t'" << str2 << "' ||" << endl;
                }
            }
            stream << "\tEXCEPTION" << endl;
            stream << "\t\tWHEN OTHERS THEN NULL;" << endl;
            stream << "\tEND;" << endl << endl;
        }
    }

    stream << "END;" << endl << "/";

    editor->setText(str);
}
