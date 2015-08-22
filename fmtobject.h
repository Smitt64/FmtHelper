#ifndef FMTOBJECT_H
#define FMTOBJECT_H

#include <QtCore>
#include <QtSql>
#include <QModelIndex>
#include <QTextStream>

class FmtField
{
    friend class FmtObject;
public:
    enum
    {
        fmt_int16 = 0,
        fmt_int32 = 1,
        fmt_float = 2,
        fmt_charptr = 7,
        fmt_bdate = 9,
        fmt_btime = 10,
        fmt_char = 12,
        fmt_lmoney = 25,
    };

    FmtField(qint32 id, const QSqlDatabase &db);
    QString getComment() const { return _comment; }
    QString getName() const { return _name; }
    qint16 getType() const { return _type; }
    qint32 getSize() const { return _size; }
    QString getTypeName() const;

    static QString getCppTypeName(const qint16 &type);
private:
    FmtField(const qint32 &id, const qint32 &size, const qint16 &type, const QString &name, const QString &comment, const QSqlDatabase &db);
    QSqlDatabase _db;
    QString _name, _comment;
    qint16 _type;
    qint32 _size, _id;
};

class FmtObject
{
public:
    FmtObject(const QModelIndex &fmtnames, const QSqlDatabase &db);
    QString getName() const { return name; }
    QString getComment() const { return comment; }
    QString getStructName() const { return sStructName; }
    QString getBFileName() const;

    bool isTmp() const;

    qint32 getId() const { return id; }

    void generateCppCode(QTextStream *stream);

    qint16 calcMaxCppLenght(qint16 *maxfieldname);

private:
    void makeOpener(QTextStream *stream);
    void makeStruct(QTextStream *stream);
    QString tryToFormatField(const QString &name);
    QString name, comment, sStructName;
    qint32 id;

    QList<FmtField*> fields;

    bool fTmp;
    QSqlDatabase _db;
};

#endif // FMTOBJECT_H