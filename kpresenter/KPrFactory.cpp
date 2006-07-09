// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "KPrFactory.h"
#include "KPrDocument.h"
#include "KPrAboutData.h"

#include <kstandarddirs.h>
#include <kinstance.h>


KInstance* KPrFactory::s_global = 0;
KAboutData* KPrFactory::s_aboutData = 0;

KPrFactory::KPrFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    (void)global();
}

KPrFactory::~KPrFactory()
{
    delete s_aboutData;
    s_aboutData=0;
    delete s_global;
    s_global=0;
}

KParts::Part* KPrFactory::createPartObject( QWidget *parentWidget, const char *widgetName, QObject* parent,
                                                   const char* name, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KPrDocument *doc = new KPrDocument( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
        doc->setReadWrite( false );

    return doc;
}

KAboutData* KPrFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData=newKPresenterAboutData();
    return s_aboutData;
}

KInstance* KPrFactory::global()
{
    if ( !s_global )
    {
        s_global = new KInstance(aboutData());

        s_global->dirs()->addResourceType("kpresenter_template",
                                          KStandardDirs::kde_default("data") + "kpresenter/templates/");
        s_global->dirs()->addResourceType("autoforms",
                                          KStandardDirs::kde_default("data") + "kpresenter/autoforms/");
        s_global->dirs()->addResourceType("slideshow",
                                          KStandardDirs::kde_default("data") + "kpresenter/slideshow/");
        // Tell the iconloader about share/apps/koffice/icons
        s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

#include "KPrFactory.moc"
