/*
 *  koffice/filters/excel97/csvfilter.h
 *
 *  Copyright (C) 1999 David Faure <faure@kde.org>
 *
 */

#ifndef CSVFILTER_H
#define CSVFILTER_H

#include <qtextstream.h>
#include <xmltree.h>

class CSVFilter : public QObject 
{
    Q_OBJECT

public:
    /** Create the filter */
    CSVFilter(const QTextStream &_inputStream);
    ~CSVFilter();

    /** Do the filtering */
    virtual const bool filter();
    /** Return the XML output */
    virtual const QString part();

private:
    XMLTree tree;
    const QTextStream & inputStream;
    bool bReady;
    bool bSuccess;
};

#endif // CSVFILTER_H
