/*
 *  koffice/filters/excel97/csvfilter.h
 *
 *  Copyright (C) 1999 David Faure <faure@kde.org>
 *
 */
#ifndef CSVFILTER_H
#define CSVFILTER_H

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qobject.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <koFilter.h>
#include <koTarStore.h>

#include <xmltree.h>

class CSVFilter : public KoFilter {

    Q_OBJECT

public:
    CSVFilter(KoFilter *parent, QString name);
    virtual ~CSVFilter() {}
    /** filtering :) */
    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString& from, const QCString& to);

};
#endif // CSVFILTER_H
