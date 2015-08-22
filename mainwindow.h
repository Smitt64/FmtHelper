#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onConnect();
    void findFirst();
    void findNext();

private:
    QMdiArea *mdi;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
