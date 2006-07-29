/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include <example_factory.h>
#include <example_part.h>
#include <example_aboutdata.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

K_EXPORT_COMPONENT_FACTORY( libexamplepart, ExampleFactory )

KInstance* ExampleFactory::s_global = 0L;
KAboutData* ExampleFactory::s_aboutData = 0L;

ExampleFactory::ExampleFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

ExampleFactory::~ExampleFactory()
{
    delete s_aboutData;
    s_aboutData = 0L;
    delete s_global;
    s_global = 0L;
}

KParts::Part* ExampleFactory::createPartObject( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
    // If classname is "KoDocument", our host is a koffice application
    // otherwise, the host wants us as a simple part, so switch to readonly and single view.
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    // parentWidget and widgetName are used by KoDocument for the "readonly+singleView" case.
    ExamplePart *part = new ExamplePart( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    return part;
}

KAboutData* ExampleFactory::aboutData()
{
    if ( !s_aboutData )
        s_aboutData = newExampleAboutData();
    return s_aboutData;
}

KInstance* ExampleFactory::global()
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

#include <example_factory.moc>
