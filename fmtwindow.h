#ifndef FMTWINDOW_H
#define FMTWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QMainWindow>
#include <QDockWidget>
#include <QListView>
#include <QProgressBar>
#include "fmtgenrunnable.h"
#include "fmtlistmodel.h"
#include "codeeditor.h"
#include "highlighter.h"

namespace Ui {
class FmtWindow;
}

class FmtWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FmtWindow(QWidget *parent = 0);
    ~FmtWindow();

    bool connectToDatabase();
    void init();

    void findFirst();
    void findNext();

    QString connectionName() const;

public slots:
    void fillFmtNames();

private slots:
    void onFmtListDoubleClicked(const QModelIndex &index);
    void onFoundCurItem(const QModelIndex &index);

    void onStepGenFinish(const QString &text, const qint16 &step);
    void onGenerationFinish(const qint16 &result);

private:
    bool inGenerate;
    Ui::FmtWindow *ui;
    QDockWidget *dock;
    QListView *fmtList;
    CodeEditor *cppCodeEditor, *tablesSql;
    QSqlDatabase db;
    QTabWidget *tabWidget;
    QProgressBar *progress;
    FmtListModel *fmtModel;
    Highlighter *cppHighlighter;
    SqlHighlighter *tablesSqlHlght;
};

#endif // FMTWINDOW_H
