/* This file is part of the KDE project
   Copyright (C) 2001 Ian Reinhart Geiser <geiseri@yahoo.com>
   This is based off of the KOffice Example found in the KOffice
   CVS.  Torben Weis <weis@kde.org> is the original author.

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

#include <kosoap_factory.h>
#include <kosoap_part.h>
#include <kosoap_aboutdata.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

extern "C"
{
    void* init_libkosoappart()
    {
        return new KOSoapFactory;
    }
};

KInstance* KOSoapFactory::s_global = 0L;
KAboutData* KOSoapFactory::s_aboutData = 0L;

KOSoapFactory::KOSoapFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

KOSoapFactory::~KOSoapFactory()
{
    delete s_aboutData;
    s_aboutData = 0L;
    delete s_global;
    s_global = 0L;
}

KParts::Part* KOSoapFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
    // If classname is "KoDocument", our host is a koffice application
    // otherwise, the host wants us as a simple part, so switch to readonly and single view.
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    // parentWidget and widgetName are used by KoDocument for the "readonly+singleView" case.
    KOSoapPart *part = new KOSoapPart( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    // Tell the factory base class that we created the object (mandatory)
    emit objectCreated(part);

    return part;
}

KAboutData* KOSoapFactory::aboutData()
{
    if ( !s_aboutData )
        // Change this, of course
        s_aboutData = newKOSoapAboutData();
    return s_aboutData;
}

KInstance* KOSoapFactory::global()
{
    if ( !s_global )
    {
        s_global = new KInstance( aboutData() );
        // Add any application-specific resource directories here

        // Tell the iconloader about share/apps/koffice/icons
        s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

#include <kosoap_factory.moc>
