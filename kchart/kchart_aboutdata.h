/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef KCHART_ABOUTDATA
#define KCHART_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("KOffice Chart Generator");
static const char* version="0.1";

KAboutData * newKChartAboutData()
{
    KAboutData * aboutData= new KAboutData("kchart", I18N_NOOP("KChart"),
                                           version, description, KAboutData::License_GPL,
                                           "(c) 1998-2001, Kalle Dalheimer");
    aboutData->addAuthor("Kalle Dalheimer",0, "kalle@kde.org");
    return aboutData;
}

#endif
