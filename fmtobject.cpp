#include "fmtobject.h"
#include "fmtindex.h"

FmtField::FmtField(qint32 id, const QSqlDatabase &db)
{
    _db = db;
    _id = id;

    QSqlQuery qf(_db);
    qf.prepare("SELECT T_ID, T_NAME, T_TYPE, T_SIZE, T_COMMENT from FMT_FIELDS WHERE T_ID = :fid ORDER BY T_OFFSET");
    qf.bindValue(":fid", id);

    if (!qf.exec())
        throw 2;

    while (qf.next())
    {
        _name = qf.value(1).toString();
        _comment = qf.value(4).toString();
        _type = qf.value(2).toInt();
        _size = qf.value(3).toInt();
    }

    init();
}

FmtField::FmtField(const qint32 &id, const qint32 &size,
         const qint16 &type, const QString &name,
         const QString &comment, const QSqlDatabase &db)
{
    _name = _id;
    _comment = comment;
    _type = type;
    _name = name;
    _name = _name.remove("t_", Qt::CaseInsensitive);
    _size = size;
    _db = db;
    _id = id;

    init();
}

void FmtField::init()
{
    if (_type == fmt_charptr)
    {
        _cppDecl = QString("%1[%2]")
                .arg(_name)
                .arg(_size);
    }
    else
    {
        _cppDecl = _name;
    }
}

QString FmtField::getTypeName() const
{
    return getCppTypeName(_type);
}

QString FmtField::getCppTypeName(const qint16 &type)
{
    QString t;
    switch(type)
    {
    case fmt_int16:
        t = "db_int16";
        break;
    case fmt_int32:
        t = "db_int32";
        break;
    case fmt_float:
        t = "float";
        break;
    case fmt_char:
    case fmt_charptr:
        t = "char";
        break;
    case fmt_bdate:
        t = "bdate";
        break;
    case fmt_btime:
        t = "btime";
        break;
    case fmt_lmoney:
        t = "db_lmoney";
        break;
    default:
        t = "<unknown>";
    }

    return t;
}

bool FmtField::operator == (const FmtField &other)
{
    return _id == other._id;
}

bool FmtField::operator == (const qint32 &id)
{
    return _id == id;
}

// ========================================================================

FmtObject::FmtObject(const QModelIndex &fmtnames, const QSqlDatabase &db):
    fTmp(false)
{
    _db = db;

    QSqlQuery q(_db);
    q.prepare("select T_NAME, T_COMMENT, T_ID from FMT_NAMES WHERE T_ID = ?");
    QModelIndex idindex = fmtnames.model()->index(fmtnames.row(), 2);
    q.bindValue(0, idindex.data(Qt::DisplayRole).toInt());

    if (!q.exec())
    {
        throw 1;
    }

    q.next();
    name = q.value(0).toString();
    comment = q.value(1).toString();
    id = idindex.data(Qt::DisplayRole).toInt();

    if (name.contains("_tmp", Qt::CaseInsensitive))
        fTmp = true;

    sStructName = name;
    sStructName = sStructName.remove("_dbt", Qt::CaseInsensitive).remove("_tmp", Qt::CaseInsensitive);
    sStructName = sStructName.mid(1).toUpper();

    QSqlQuery qf(_db);
    qf.prepare("SELECT T_ID, T_SIZE, T_TYPE, T_NAME, T_COMMENT from FMT_FIELDS WHERE T_FMTID = :id ORDER BY T_OFFSET");
    qf.bindValue(":id", id);

    if (!qf.exec())
        throw 2;

    while (qf.next())
    {
        fields.insert(qf.value(0).toInt(), new FmtField(qf.value(0).toInt(),
                                   qf.value(1).toInt(),
                                   qf.value(2).toInt(),
                                   qf.value(3).toString(),
                                   qf.value(4).toString(), _db));
    }

    QSqlQuery qi(_db);
    qi.prepare(QString("SELECT K .T_KEYNUM, f.T_NAME, f.T_TYPE, f.T_SIZE") +
#ifndef HELPER_SQLITE
               QString(", NVL(k.T_COMMENT, f.T_COMMENT)") +
#else
               QString(", k.T_COMMENT") +
#endif
               QString(", K.T_FMTFLDID FROM ") +
               QString("FMT_KEYS K, FMT_FIELDS f WHERE K .T_FMTID = :fmt AND K .T_FMTFLDID = f.T_ID ORDER BY K .T_KEYNUM,K .T_SEGNUM"));
    qi.bindValue(":fmt", id);

    if (!qi.exec())
    {
        throw 3;
    }

    int prevkey = -1;
    while (qi.next())
    {
        if (prevkey != qi.value(0).toInt())
        {
            indeces.append(new FmtIndex(qi.value(0).toInt(), qi.value(4).toString(), this));
            prevkey = qi.value(0).toInt();
        }

        indeces.last()->pushField(fields[qi.value(5).toInt()]);
    }
}

qint16 FmtObject::calcMaxCppLenght(qint16 *maxfieldname)
{
    qint16 len = 0, fieldname = 0;

    QMapIterator<qint32, FmtField *> i(fields);
    while(i.hasNext())
    {
        i.next();
        FmtField *f = i.value();
        len = qMax(len, (qint16)f->getTypeName().length());
        fieldname = qMax(fieldname, (qint16)f->getCppDecl().length());
    }

    if (maxfieldname)
        *maxfieldname = fieldname;

    return len;
}

bool FmtObject::isTmp() const
{
    return fTmp;
}

QString FmtObject::tryToFormatField(const QString &name)
{
    return name;
}

QString FmtObject::getBFileName() const
{
    return QString("%1.%2")
            .arg(sStructName.toLower())
            .arg(isTmp() ? "tmp" : "dbt");
}

void FmtObject::generateCppCode(QTextStream *stream)
{
    makeOpener(stream);
    *stream << endl;
    makeStruct(stream);
    *stream << endl;
    makeKeysUnion(stream);
}

void FmtObject::makeOpener(QTextStream *stream)
{
    *stream << "// " << getComment() << endl;
    *stream << QString("extern BTRVFILE *File%1;").arg(getStructName()) << endl;
    *stream << QString("extern int iOpen%1 (int OpenMode);").arg(getStructName()) << endl << endl;

    *stream << "//----------------------------------------------------------------------------" << endl;
    *stream << QString("// Файл %1").arg(getBFileName()) << endl;
    *stream << "//----------------------------------------------------------------------------" << endl;
    *stream << QString("BTRVFILE *File%1 = NULL;").arg(getStructName()) << endl;
    *stream << QString("int iOpen%1 (int OpenMode)").arg(getStructName()) << endl;
    *stream << "{" << endl;
    *stream << "\tchar FileName[_MAX_PATH];" << endl;
    *stream << QString("\tgetFileName(FileName, \"%1\");").arg(getBFileName()) << endl;
    *stream << QString("\treturn bfOpen( &File%1, FileName, OpenMode, sizeof(%1), sizeof(%1_KEYS), 0, NULL, NULL, NULL, NULL );")
            .arg(getStructName()) << endl;
    *stream << "}" << endl;
}

void FmtObject::makeStruct(QTextStream *stream)
{
    *stream << "// " << getComment() << endl;
    *stream << "typedef struct" << endl;
    *stream << "{" << endl;

    qint16 fldname = 0;
    qint16 maxlen = calcMaxCppLenght(&fldname);

    QMapIterator<qint32, FmtField*> i(fields);
    while (i.hasNext())
    {
        i.next();
        FmtField *f = i.value();
        *stream << "\t" << f->getTypeName().leftJustified(maxlen) << " ";
        *stream << QString("%1;").arg(f->getCppDecl()).leftJustified(fldname + 1) << " // " << f->getComment() <<endl;
    }

    *stream << "} " << sStructName << ";" << endl;
}

void FmtObject::makeKeysUnion(QTextStream *stream)
{
    *stream << "typedef union" << endl;
    *stream << "{" << endl;

    foreach (FmtIndex *ind, indeces)
    {
        *stream << "\tstruct" << endl;
        *stream << "\t{" << endl;

        qint16 fldname = 0;
        qint16 maxlen = ind->calcMaxCppLenght(&fldname);

        for (int i = 0; i < ind->getFldCount(); i++)
        {
            const FmtField *f = ind->getField(i);
            *stream << "\t\t";
            *stream << f->getTypeName().leftJustified(maxlen) << " ";
            *stream << QString("%1;").arg(f->getCppDecl()).leftJustified(fldname + 1) << " // " << f->getComment() <<endl;
        }

        *stream << "\t} k" << ind->getKeyNum() + 1 << ";" << endl;

        if (indeces.last() != ind)
        {
            *stream << endl;
        }
    }

    *stream << "} " << sStructName << "_KEYS;" << endl << endl;

    *stream << "typedef enum" << endl;
    *stream << "{" << endl;
    for (int i = 0; i < indeces.size(); i++)
    {
        *stream << "\t" << sStructName << "_KEY" << i + 1;
        if (i != indeces.size() - 1)
        {
            *stream << ",";
        }
        *stream << endl;
    }
    *stream << "} " << sStructName << "_KEYNUM;" << endl;
}
