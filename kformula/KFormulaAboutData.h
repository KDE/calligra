/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULAABOUTDATA_H
#define KFORMULAABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* description=I18N_NOOP("KOffice Formula Editor");
static const char* version=KOFFICE_VERSION_STRING;

KAboutData * newKFormulaAboutData()
{
    KAboutData * aboutData = new KAboutData( "kformula", 0, ki18n( "KFormula" ),
                                             version, ki18n(description), KAboutData::License_GPL,
                                             ki18n("(c) 1998-2008, The KFormula Team"), KLocalizedString(),
					     "http://www.koffice.org/kformula/" );
    aboutData->addAuthor( ki18n("Andrea Rizzi"), ki18n( "original author" ), "rizzi@kde.org" );
    aboutData->addAuthor( ki18n("Ulrich Kuettler"), ki18n( "core developer" ), "ulrich.kuettler@gmx.de" );
    aboutData->addAuthor( ki18n("Heinrich Kuettler"), ki18n( "core developer" ), "heinrich.kuettler@gmx.de" );
    aboutData->addCredit( ki18n("Claus O. Wilke"), ki18n( "for your advice to look at TeX first" ) );
    aboutData->addAuthor( ki18n("Martin Pfeiffer"), ki18n( "core developer" ), "hubipete@gmx.net" );
    aboutData->addAuthor( ki18n("Alfredo Beaumont"), ki18n( "current maintainer" ), "alfredo.beaumont@gmail.com" );
    return aboutData;
}

#endif
