#include "fmtwindow.h"
#include "ui_fmtwindow.h"
#include "oracleauthdlg.h"
#include "fmtobject.h"
#include "fmtgenrunnable.h"
#include <QMessageBox>
#include <QThreadPool>
#include <QSqlError>
#include <QSqlQuery>
#include <QInputDialog>
#include <QTextStream>
#include <QFileDialog>

enum FMT_NAMES
{
    fmt_name = Qt::UserRole,
    fmt_comment,
    fmt_id
};

FmtWindow::FmtWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(NULL),
    inGenerate(false)
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
    tablesSql = new CodeEditor(this);
    updScriptCodeEditor = new CodeEditor(this);

    cppHighlighter = new Highlighter(cppCodeEditor->document());
    tablesSqlHlght = new SqlHighlighter(tablesSql->document());
    updScriptHlght = new SqlHighlighter(updScriptCodeEditor->document());

    fmtList = new QListView(this);
    fmtModel = new FmtListModel(&db, this);
    fmtList->setModel(fmtModel);

    dock = new QDockWidget(tr("Таблицы"), this);
    dock->setWidget(fmtList);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(cppCodeEditor, QIcon(":/cplusplus"), tr("C++ Код"));
    tabWidget->addTab(tablesSql, QIcon(":/sql"), tr("tables.sql"));
    tabWidget->addTab(updScriptCodeEditor, QIcon(":/sql"), tr("update script"));
    setCentralWidget(tabWidget);
    fmtModel->updateFmtModel();

    progress = new QProgressBar(this);
    progress->setRange(0, 0);
    progress->setValue(0);
    progress->setTextVisible(false);
    progress->setVisible(false);
    tabWidget->setCornerWidget(progress);

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

void FmtWindow::onStepGenFinish(const QString &text, const qint16 &step)
{
    switch(step)
    {
    case FmtGenRunnable::GEN_CPP:
        cppCodeEditor->setText(text);
        break;
    case FmtGenRunnable::GEN_TABLE:
        tablesSql->setText(text);
        break;
    case FmtGenRunnable::GEN_UPD_SCRIPT:
        updScriptCodeEditor->setText(text);
        break;
    };
}

void FmtWindow::onGenerationFinish(const qint16 &result)
{
    progress->setVisible(false);
    inGenerate = false;
}

void FmtWindow::onFmtListDoubleClicked(const QModelIndex &index)
{
    FmtGenRunnable *gen = new FmtGenRunnable(index, db);
    connect(gen, SIGNAL(stepFinish(QString,qint16)), SLOT(onStepGenFinish(QString,qint16)));
    connect(gen, SIGNAL(finished(qint16)), SLOT(onGenerationFinish(qint16)));

    if (!inGenerate && QThreadPool::globalInstance()->tryStart(gen))
    {
        progress->setVisible(true);
        inGenerate = true;
    }
}

void FmtWindow::save()
{
    QString file, filter;
    CodeEditor *editor = NULL;

    bool res = true;
    switch(tabWidget->currentIndex())
    {
    case 0: //c++
        filter = "C files (*.c)";
        editor = cppCodeEditor;
        break;
    case 1: // tables.sql
        filter = "Sql files (*.sql)";
        editor = tablesSql;
        break;
    default:
        res = false;
    }

    file = QFileDialog::getSaveFileName(this, QString(), QString(), filter);

    if (!file.isEmpty())
    {
        QFile f(file);
        if (f.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&f);
            stream.setCodec("IBM 866");
            stream << editor->toPlainText();
            f.close();
        }
    }
}
