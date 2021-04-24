/*
This file is part of the KDE project
SPDX-FileCopyrightText: 2002 Fred Malabre <fmalabre@yahoo.com>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_BASE_WORKER_H
#define KSPREAD_BASE_WORKER_H

#include <KoFilter.h>
#include <QMap>


typedef QMap<QString, QString> KSpreadFilterProperty;

class KSpreadBaseWorker
{
public:
    KSpreadBaseWorker();
    virtual ~KSpreadBaseWorker();

    virtual KoFilter::ConversionStatus startDocument(KSpreadFilterProperty property);
    virtual KoFilter::ConversionStatus startInfoLog(KSpreadFilterProperty property);
    virtual KoFilter::ConversionStatus startInfoAuthor(KSpreadFilterProperty property);
    virtual KoFilter::ConversionStatus startInfoAbout(KSpreadFilterProperty property);
    virtual KoFilter::ConversionStatus startSpreadBook(KSpreadFilterProperty property);
    virtual KoFilter::ConversionStatus startSpreadSheet(KSpreadFilterProperty property);
    virtual KoFilter::ConversionStatus startSpreadCell(KSpreadFilterProperty property);
};

#endif /* KSPREAD_BASE_WORKER_H */
