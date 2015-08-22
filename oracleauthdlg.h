#ifndef ORACLEAUTHDLG_H
#define ORACLEAUTHDLG_H

#include <QDialog>
#include "ui_oracleauthdlg.h"

typedef struct
{
    QString user;
    QString pass;
    QString database;
    QString host;
    QString service;
    qint16 port;
} RecentList;

QDataStream &operator <<(QDataStream &stream, const RecentList &i);
QDataStream &operator >>(QDataStream &stream, RecentList &i);
bool operator == (RecentList &list, const RecentList &other);

class OracleAuthDlg : public QDialog
{
	Q_OBJECT

public:
	OracleAuthDlg(QWidget *parent = 0);
	~OracleAuthDlg();
	bool authirizate();
	void LoadPrefs();
	void SavePrefs();

    QString getConnectionName() { return connectionname; }
private:
	Ui::OracleAuthDlg ui;
private slots:
	void on_pushButton_accept_clicked();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

private:
    QString connectionname;
    QString sConfigOraFilePath;
    QList<RecentList> recentList;
};

#endif // ORACLEAUTHDLG_H
