#ifndef EXCELFILTER_H
#define EXCELFILTER_H

#include <qdatastream.h>

#include <filterbase.h>
#include <xmltree.h>
#include <myfile.h>

const int MAX_RECORD_SIZE = 0x2024;

class ExcelFilter : public FilterBase 
{

    Q_OBJECT

public:
    ExcelFilter(const myFile &mainStream);
    ~ExcelFilter();

    virtual const bool filter();
    virtual const QString extension() { return ".ksp"; }
    virtual const QString part();

private:
    QDataStream *s;
    XMLTree *tree;
    bool filterOk;
};

#endif // EXCELFILTER_H
