#include "fmtindex.h"
#include "fmtobject.h"

FmtIndex::FmtIndex(const qint32 &keynum, const QString &comment, FmtObject *object)
{
    _keynum = keynum;
    _comment = comment;
    obj = object;
}

void FmtIndex::pushField(FmtField *field)
{
    fields.append(field);
}

qint16 FmtIndex::calcMaxCppLenght(qint16 *maxfieldname)
{
    qint16 len = 0, fieldname = 0;

    foreach (FmtField *f, fields)
    {
        len = qMax(len, (qint16)f->getTypeName().length());
        fieldname = qMax(fieldname, (qint16)f->getCppDecl().length());
    }

    if (maxfieldname)
        *maxfieldname = fieldname;

    return len;
}
