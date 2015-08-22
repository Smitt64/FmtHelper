#ifndef FMTINDEX_H
#define FMTINDEX_H

#include <QtCore>

class FmtObject;
class FmtField;

class FmtIndex
{
public:
    FmtIndex(const qint32 &keynum, const QString &comment, FmtObject *object);

    qint32 getKeyNum() const { return _keynum; }
    QString getComment() const { return _comment; }

    qint32 getFldCount() const { return fields.size(); }
    FmtField *getField(const qint32 &index) const { return fields[index]; }

    qint16 calcMaxCppLenght(qint16 *maxfieldname);

    void pushField(FmtField *field);
private:
    qint32 _keynum;
    QString _comment;
    FmtObject *obj;
    QList<FmtField*> fields;
};

#endif // FMTINDEX_H
