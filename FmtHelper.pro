#-------------------------------------------------
#
# Project created by QtCreator 2015-08-14T11:30:57
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets

    win32 {
        packagesExist(QtWinExtras)  {
            DEFINES += WINEXTRAS
        }
    }
}

TARGET = FmtHelper
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    codeeditor.cpp \
    highlighter.cpp \
    fmtwindow.cpp \
    oracleauthdlg.cpp \
    fmtlistmodel.cpp \
    fmtobject.cpp \
    fmtindex.cpp \
    fmtgenrunnable.cpp \
    customupdatescript.cpp \
    dragablefmtlistview.cpp \
    sqlcodedialog.cpp

HEADERS  += mainwindow.h \
    codeeditor.h \
    highlighter.h \
    fmtwindow.h \
    oracleauthdlg.h \
    fmtlistmodel.h \
    fmtobject.h \
    fmtindex.h \
    fmtgenrunnable.h \
    customupdatescript.h \
    dragablefmtlistview.h \
    sqlcodedialog.h

FORMS    += mainwindow.ui \
    fmtwindow.ui \
    oracleauthdlg.ui

RESOURCES += \
    fmt.qrc

# использовать локальную базу даннх sqlite вместо подключения к oracle
DEFINES +=  HELPER_SQLITE
