/* This file is part of the KDE project
   Copyright (C) 1999 Matthias Kalle Dalheimer <kalle@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KCHART_ABOUTDATA
#define KCHART_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>
#include <config.h>

namespace KChart
{

static const char* description=I18N_NOOP("KOffice Chart Generator");
static const char* version=VERSION;


KAboutData * newKChartAboutData()
{
    KAboutData * aboutData= new KAboutData("kchart", I18N_NOOP("KChart"),
                                           version, description, KAboutData::License_GPL,
                                           I18N_NOOP("(c) 1998-2006, Kalle Dalheimer and Klarälvdalens Datakonsult AB"),
                                           I18N_NOOP("The drawing engine which forms the base of KChart\nis also available as a commercial product\nfrom Klarälvdalens Datakonsult AB.\nContact info@klaralvdalens-datakonsult.se\nfor more information."),
					   "http://www.koffice.org/kchart/");
    aboutData->addAuthor("Kalle Dalheimer", 0, "kalle@kde.org");
    aboutData->addAuthor("Laurent Montel", 0, "lmontel@mandrakesoft.com");
    aboutData->addAuthor("Karl-Heinz Zimmer", 0, "khz@kde.org");
    aboutData->addAuthor("Inge Wallin", 0, "inge@lysator.liu.se");
    aboutData->addCredit("Danny Allen", I18N_NOOP("Created chart preview images, toolbar icons"), "danny@dannyallen.co.uk");
    return aboutData;
}

}  //namespace KChart

#endif
