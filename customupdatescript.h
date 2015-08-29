#ifndef CUSTOMUPDATESCRIPT_H
#define CUSTOMUPDATESCRIPT_H

#include <QWidget>
#include <QSqlDatabase>
#include <QPushButton>
#include <QHBoxLayout>
#include "codeeditor.h"
#include "highlighter.h"
#include "dragablefmtlistview.h"

class CustomUpdateScript : public QWidget
{
    Q_OBJECT
public:
    explicit CustomUpdateScript(const QSqlDatabase &db, QWidget *parent = 0);
    ~CustomUpdateScript();

    CodeEditor *getEditor() { return editor; }
    DropableFmtTreeView *getTree() { return tree; }

private slots:
    void generate();
    void deleteTable();
    void addSql();

private:
    DropableFmtTreeView *tree;
    QPushButton *clearButton, *genButton, *delButton, *addSqlButton;

    CodeEditor *editor;
    SqlHighlighter *highlight;

    QHBoxLayout *buttons1, *mainl;
    QVBoxLayout *buttons2;
};

#endif // CUSTOMUPDATESCRIPT_H
