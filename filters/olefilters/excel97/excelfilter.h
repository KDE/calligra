/*
 *  koffice/filters/excel97/excelfilter.h
 *
 *  Copyright (C) 1999 Percy Leonhardt
 *
 */

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
    virtual const QDomDocument * const part();

private:
    QDataStream *s;
    XMLTree *tree;
};

#endif // EXCELFILTER_H
