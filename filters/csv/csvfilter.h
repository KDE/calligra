/*
 *  koffice/filters/excel97/csvfilter.h
 *
 *  Copyright (C) 1999 David Faure <faure@kde.org>
 *
 */

#ifndef CSVFILTER_H
#define CSVFILTER_H

#include <qtextstream.h>

#include <filterbase.h>
#include <xmltree.h>
#include <myfile.h>

class CSVFilter : public FilterBase 
{
    Q_OBJECT

public:
    CSVFilter(const myFile &mainStream);
    ~CSVFilter();

    /** Do the filtering */
    virtual const bool filter();
    /** Return the XML output */
    virtual const QString part();

private:
    QTextStream *s;
    XMLTree *tree;
};

#endif // CSVFILTER_H
