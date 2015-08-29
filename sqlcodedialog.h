#ifndef SQLCODEDIALOG_H
#define SQLCODEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include "codeeditor.h"
#include "highlighter.h"
#include <QDialog>

class SqlCodeDialog : public QDialog
{
    Q_OBJECT
public:
    SqlCodeDialog(QWidget *parent = 0);
    virtual ~SqlCodeDialog();
    CodeEditor *getEditor() { return editor; }

private:
    CodeEditor *editor;
    SqlHighlighter *sql;
    QDialogButtonBox *buttons;
    QVBoxLayout *l;
};

#endif // SQLCODEDIALOG_H
