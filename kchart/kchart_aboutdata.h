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
static const char* version="0.9 (RC1)";

KAboutData * newKChartAboutData()
{
    KAboutData * aboutData= new KAboutData("kchart", I18N_NOOP("KChart"),
                                           version, description, KAboutData::License_GPL,
                                           I18N_NOOP("(c) 1998-2001, Kalle Dalheimer and Klarälvdalens Datakonsult AB"),
                                           I18N_NOOP("The drawing engine which forms the base of KChart\nis also available as a commercial product\nfrom Klaralvdalens Datakonsult AB,\nplease contact info@klaralvdalens-datakonsult.se\nfor more information!"), 
					   "http://www.koffice.org/kchart/");
    aboutData->addAuthor("Kalle Dalheimer",0, "kalle@kde.org");
    return aboutData;
}

#endif
