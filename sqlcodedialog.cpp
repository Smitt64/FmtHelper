#include "sqlcodedialog.h"

SqlCodeDialog::SqlCodeDialog(QWidget *parent) :
    QDialog(parent)
{
    buttons = new QDialogButtonBox(this);
    buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    editor = new CodeEditor(this);
    sql = new SqlHighlighter(editor->document());

    l = new QVBoxLayout;
    l->addWidget(editor);
    l->addWidget(buttons);
    setLayout(l);

    editor->setFocus();

    connect(buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), SLOT(reject()));
}

SqlCodeDialog::~SqlCodeDialog()
{
    delete buttons;
    delete sql;
    delete editor;
    delete l;
}
