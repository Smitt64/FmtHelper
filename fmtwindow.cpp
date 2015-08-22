#include "fmtwindow.h"
#include "ui_fmtwindow.h"
#include "oracleauthdlg.h"
#include "fmtobject.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QInputDialog>
#include <QTextStream>

enum FMT_NAMES
{
    fmt_name = Qt::UserRole,
    fmt_comment,
    fmt_id
};

FmtWindow::FmtWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(NULL)
{
}

FmtWindow::~FmtWindow()
{
    if (ui)
    {
        delete ui;
    }
}

void FmtWindow::init()
{
    ui = new Ui::FmtWindow;
    ui->setupUi(this);

    cppCodeEditor = new CodeEditor(this);
    cppHighlighter = new Highlighter(cppCodeEditor->document());

    fmtList = new QListView(this);
    fmtModel = new FmtListModel(&db, this);
    fmtList->setModel(fmtModel);

    dock = new QDockWidget(tr("Таблицы"), this);
    dock->setWidget(fmtList);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    ui->tabWidget->addTab(cppCodeEditor, QIcon(":/cplusplus"), tr("C++ Код"));
    setCentralWidget(ui->tabWidget);
    fmtModel->updateFmtModel();

    connect(fmtModel, SIGNAL(findItemSelection(QModelIndex)), SLOT(onFoundCurItem(QModelIndex)));
    connect(fmtList, SIGNAL(doubleClicked(QModelIndex)), SLOT(onFmtListDoubleClicked(QModelIndex)));
}

bool FmtWindow::connectToDatabase()
{
    OracleAuthDlg dlg(this);

    int ret = dlg.exec();
    if (ret == QDialog::Accepted && !dlg.authirizate())
    {
        db = QSqlDatabase::database(dlg.getConnectionName());
        if (!db.open())
        {
            QMessageBox::critical(this, "Error", db.lastError().text());
        }
    }

    if (ret == QDialog::Accepted)
    {
        db = QSqlDatabase::database(dlg.getConnectionName());
        setWindowTitle(dlg.getConnectionName());
    }

    return db.isOpen();
}

QString FmtWindow::connectionName() const
{
    return db.connectionName();
}

void FmtWindow::findFirst()
{
    bool ok = false;
    QString f = QInputDialog::getText(this, "Найти", "Название таблицы", QLineEdit::Normal, "", &ok);

    if (ok)
    {
        fmtModel->findFirst(f);
    }
}

void FmtWindow::findNext()
{
    fmtModel->findNext();
}

void FmtWindow::onFoundCurItem(const QModelIndex &index)
{
    fmtList->setCurrentIndex(index);
}

void FmtWindow::fillFmtNames()
{

}

void FmtWindow::onFmtListDoubleClicked(const QModelIndex &index)
{
    QString cpp;
    QTextStream stream(&cpp);

    try
    {
        FmtObject obj(index, db);
        obj.generateCppCode(&stream);
        cppCodeEditor->setText(cpp);
    }
    catch(...)
    {

    }
}
