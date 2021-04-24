/*
This file is part of the KDE project
SPDX-FileCopyrightText: 2002 Fred Malabre <fmalabre@yahoo.com>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KSpreadBaseWorker.h"

#include <kdebug.h>


KSpreadBaseWorker::KSpreadBaseWorker()
{
}


KSpreadBaseWorker::~KSpreadBaseWorker()
{
}


KoFilter::ConversionStatus KSpreadBaseWorker::startDocument(KSpreadFilterProperty property)
{
    KSpreadFilterProperty::Iterator it;
    for (it = property.begin(); it != property.end(); ++it) {
        kDebug(30508) << "startDocument:" << it.key() << "->" << it.data();
    }
    return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startInfoLog(KSpreadFilterProperty property)
{
    KSpreadFilterProperty::Iterator it;
    for (it = property.begin(); it != property.end(); ++it) {
        kDebug(30508) << "startInfoLog:" << it.key() << "->" << it.data();
    }
    return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startInfoAuthor(KSpreadFilterProperty property)
{
    KSpreadFilterProperty::Iterator it;
    for (it = property.begin(); it != property.end(); ++it) {
        kDebug(30508) << "startInfoAuthor:" << it.key() << "->" << it.data();
    }
    return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startInfoAbout(KSpreadFilterProperty property)
{
    KSpreadFilterProperty::Iterator it;
    for (it = property.begin(); it != property.end(); ++it) {
        kDebug(30508) << "startInfoAbout:" << it.key() << "->" << it.data();
    }
    return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startSpreadBook(KSpreadFilterProperty property)
{
    KSpreadFilterProperty::Iterator it;
    for (it = property.begin(); it != property.end(); ++it) {
        kDebug(30508) << "startSpreadBook:" << it.key() << "->" << it.data();
    }
    return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startSpreadSheet(KSpreadFilterProperty property)
{
    KSpreadFilterProperty::Iterator it;
    for (it = property.begin(); it != property.end(); ++it) {
        kDebug(30508) << "startSpreadSheet:" << it.key() << "->" << it.data();
    }
    return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startSpreadCell(KSpreadFilterProperty property)
{
    KSpreadFilterProperty::Iterator it;
    for (it = property.begin(); it != property.end(); ++it) {
        kDebug(30508) << "startSpreadCell:" << it.key() << "->" << it.data();
    }
    return KoFilter::OK;
}
