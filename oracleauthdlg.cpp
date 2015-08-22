#include "oracleauthdlg.h"
//#include "global.h"
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDataStream>

QDataStream &operator <<(QDataStream &stream, const RecentList &i)
{
    stream << i.database
           << i.host
           << i.pass
           << i.port
           << i.service
           << i.user;

    return stream;
}

QDataStream &operator >>(QDataStream &stream, RecentList &i)
{
    stream >> i.database
           >> i.host
           >> i.pass
           >> i.port
           >> i.service
           >> i.user;

    return stream;
}

bool operator ==(RecentList &list, const RecentList &other)
{
    return (list.database == other.database &&
            list.host == other.host &&
            list.pass == other.pass &&
            list.port == other.port &&
            list.service == other.service &&
            list.user == other.user);
}

const QString sOracleDatabaseString ="(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=@host@)(PORT=@port@)))(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=@service@)))";

OracleAuthDlg::OracleAuthDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

    QByteArray oradir = qgetenv("");
    sConfigOraFilePath = QDir::current().absoluteFilePath("OracleAuthDlg.cfg");

    LoadPrefs();
    qDebug() << sConfigOraFilePath;
}

OracleAuthDlg::~OracleAuthDlg()
{

}

bool OracleAuthDlg::authirizate()
{
    connectionname = QString("%1@%2#%3").arg(ui.lineEdit_username->text(), ui.lineEdit_service->text(), QDateTime::currentDateTime().toString(Qt::RFC2822Date));
    QSqlDatabase db =
#ifndef HELPER_SQLITE
            QSqlDatabase::addDatabase("QOCI", connectionname);
#else
            QSqlDatabase::addDatabase("QSQLITE", connectionname);
#endif

#ifndef HELPER_SQLITE
	QString sOracleDBString = sOracleDatabaseString;
	sOracleDBString.replace("@host@", ui.lineEdit_host->text());
	sOracleDBString.replace("@port@", QString::number(ui.spinBox->value()));
	sOracleDBString.replace("@service@", ui.lineEdit_service->text());

	db.setUserName(ui.lineEdit_username->text());
	db.setPassword(ui.lineEdit_passw->text());
	
	db.setDatabaseName(sOracleDBString);
#else
    db.setDatabaseName("./fmt_emulator.sqlite");
#endif
	
	bool login = db.open();
    if(db.lastError().isValid())
    {
		QMessageBox::information(this, "", db.lastError().text());
    }	/*else ::sGLobalSchemaDot = ui.lineEdit_base->text().append(".");*/
	return login;
}

void OracleAuthDlg::LoadPrefs()
{
	QFile file(sConfigOraFilePath);
    if(file.open(QIODevice::ReadOnly))
    {
        QDataStream stream(&file);
        QStringList lst;
        stream >> lst;
        stream >> recentList;

        if(lst.count()>=5)
        {
			QString sDatabase = lst.at(2);
			QString sHostname = lst.at(0);
			QString sPort = lst.at(1);
			QString sUsername = lst.at(3);
			QString sService = lst.at(4);
			
			ui.lineEdit_base->setText(sDatabase);
			ui.lineEdit_host->setText(sHostname);
			ui.lineEdit_username->setText(sUsername);
			ui.lineEdit_service->setText(sService);
			ui.spinBox->setValue(sPort.toInt());
            if(lst.count()==6)
            {
				ui.lineEdit_passw->setText( lst.at(5) );
				ui.checkBox_savePass->setChecked(true);
                //authirizate();
			}
		}

        foreach (RecentList recent, recentList)
        {
            int row = ui.tableWidget->rowCount();
            ui.tableWidget->setRowCount(row + 1);

            ui.tableWidget->setItem(row, 0, new QTableWidgetItem(recent.user));
            ui.tableWidget->setItem(row, 1, new QTableWidgetItem(recent.service));
        }
		file.close();
	}
}

void OracleAuthDlg::SavePrefs()
{
	QFile f(sConfigOraFilePath);
    if(f.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&f);
		QStringList lstValues;
		lstValues << ui.lineEdit_host->text() 
			<< QString::number(ui.spinBox->value())
			<< ui.lineEdit_base->text()
			<< ui.lineEdit_username->text() << ui.lineEdit_service->text();
        if(ui.checkBox_savePass->isChecked())
            lstValues << ui.lineEdit_passw->text();

        RecentList list;
        list.database = ui.lineEdit_base->text();
        list.host = ui.lineEdit_host->text();
        list.pass = ui.lineEdit_passw->text();
        list.port = ui.spinBox->value();
        list.service = ui.lineEdit_service->text();
        list.user = ui.lineEdit_username->text();

        if (!recentList.contains(list))
        {
            recentList.append(list);
        }

        stream << lstValues;
        stream << recentList;
		f.close();
    }
    else
    {
		QMessageBox::information(this, "", tr("Failed to open config file!") );
	}
}

void OracleAuthDlg::on_pushButton_accept_clicked()
{
    if(authirizate())
    {
		SavePrefs();
		accept();
	}
}

void OracleAuthDlg::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    ui.lineEdit_base->setText(recentList[index.row()].database);
    ui.lineEdit_host->setText(recentList[index.row()].host);
    ui.lineEdit_username->setText(recentList[index.row()].user);
    ui.lineEdit_service->setText(recentList[index.row()].service);
    ui.spinBox->setValue(recentList[index.row()].port);

    if(ui.checkBox_savePass->isChecked())
    {
        ui.lineEdit_passw->setText(recentList[index.row()].pass);
    }

    on_pushButton_accept_clicked();
}
