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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpresenter_factory.h"
#include "kpresenter_doc.h"
#include "kpresenter_aboutdata.h"

#include <kstandarddirs.h>
#include <kinstance.h>

K_EXPORT_COMPONENT_FACTORY( libkpresenterpart, KPresenterFactory )


KInstance* KPresenterFactory::s_global = 0;
KAboutData* KPresenterFactory::s_aboutData = 0;

KPresenterFactory::KPresenterFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    (void)global();
}

KPresenterFactory::~KPresenterFactory()
{
    delete s_aboutData;
    s_aboutData=0;
    delete s_global;
    s_global=0;
}

KParts::Part* KPresenterFactory::createPartObject( QWidget *parentWidget, const char *widgetName, QObject* parent,
                                                   const char* name, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KPresenterDoc *doc = new KPresenterDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
        doc->setReadWrite( false );

    return doc;
}

KAboutData* KPresenterFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData=newKPresenterAboutData();
    return s_aboutData;
}

KInstance* KPresenterFactory::global()
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

#include "kpresenter_factory.moc"
