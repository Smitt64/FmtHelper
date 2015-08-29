#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMdiSubWindow>
#include "fmtwindow.h"
#ifdef WINEXTRAS
#include <QtWinExtras/QtWin>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mdi = new QMdiArea(this);
    mdi->setDocumentMode(true);
    mdi->setViewMode(QMdiArea::TabbedView);
    mdi->setTabsClosable(true);
    setCentralWidget(mdi);

    ui->mainToolBar->addAction(ui->actionConnect);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionSave);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionFind);
    ui->mainToolBar->addAction(ui->actionFindNext);
    ui->mainToolBar->addAction(ui->actionCppRefrsh);
    ui->mainToolBar->addAction(ui->actionTablesRefresh);
    ui->mainToolBar->addAction(ui->actionAddTable);

    ui->actionSave->setShortcut(QKeySequence(QKeySequence::Save));
    ui->actionFind->setShortcut(QKeySequence(QKeySequence::Find));
    ui->actionConnect->setShortcut(QKeySequence(QKeySequence::Open));
    ui->actionCppRefrsh->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    ui->actionTablesRefresh->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_R));
    ui->actionFindNext->setShortcut(QKeySequence(Qt::Key_F3));
    ui->actionAddTable->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));

    setWindowIcon(QIcon(":/icon"));

    connect(ui->actionConnect, SIGNAL(triggered(bool)), SLOT(onConnect()));
    connect(ui->actionFind, SIGNAL(triggered(bool)), SLOT(findFirst()));
    connect(ui->actionFindNext, SIGNAL(triggered(bool)), SLOT(findNext()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), SLOT(save()));
    connect(ui->actionAddTable, SIGNAL(triggered(bool)), SLOT(addTable()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnect()
{
    FmtWindow *w = new FmtWindow(mdi);

    if(w->connectToDatabase())
    {
        QMdiSubWindow *window = mdi->addSubWindow(w, Qt::Window | Qt::SubWindow);
        w->init();
        w->show();
        window->setWindowTitle(w->connectionName());
        window->setWindowIcon(QIcon(":/db"));
    }
    else
    {
        delete w;
    }
}

void MainWindow::findFirst()
{
    QMdiSubWindow *window = mdi->currentSubWindow();

    if (!window)
        return;

    FmtWindow *w = (FmtWindow*)window->widget();
    w->findFirst();
}

void MainWindow::findNext()
{
    QMdiSubWindow *window = mdi->currentSubWindow();

    if (!window)
        return;

    FmtWindow *w = (FmtWindow*)window->widget();
    w->findNext();
}

void MainWindow::save()
{
    QMdiSubWindow *window = mdi->currentSubWindow();

    if (!window)
        return;

    FmtWindow *w = (FmtWindow*)window->widget();
    w->save();
}

void MainWindow::addTable()
{
    QMdiSubWindow *window = mdi->currentSubWindow();

    if (!window)
        return;

    FmtWindow *w = (FmtWindow*)window->widget();
    w->addTable();
}
