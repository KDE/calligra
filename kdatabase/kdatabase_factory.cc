/* This file is part of the KDE project
   Copyright (C) 2002 Chris Machemer <machey@ceinetworks.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdatabase_factory.h>
#include <kdatabase_part.h>
#include <kdatabase_aboutdata.h>
#include <kinstance.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

extern "C"
{
    void* init_libkdatabasepart()
    {
        return new KDatabaseFactory;
    }
};

KInstance* KDatabaseFactory::s_global = 0L;
KAboutData* KDatabaseFactory::s_aboutData = 0L;

KDatabaseFactory::KDatabaseFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

KDatabaseFactory::~KDatabaseFactory()
{
    delete s_aboutData;
    s_aboutData = 0L;
    delete s_global;
    s_global = 0L;
}

KParts::Part* KDatabaseFactory::createPartObject( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
    // If classname is "KoDocument", our host is a koffice application
    // otherwise, the host wants us as a simple part, so switch to readonly and single view.
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    // parentWidget and widgetName are used by KoDocument for the "readonly+singleView" case.
    KDatabasePart *part = new KDatabasePart( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    return part;
}

KAboutData* KDatabaseFactory::aboutData()
{
    if ( !s_aboutData )
        s_aboutData = newKDatabaseAboutData();
    return s_aboutData;
}

KInstance* KDatabaseFactory::global()
{
    if ( !s_global )
    {
        s_global = new KInstance( aboutData() );
        // Add any application-specific resource directories here

        // Tell the iconloader about share/apps/koffice/icons
        s_global->iconLoader()->addAppDir("koffice");
        s_global->dirs()->addResourceType( "kdatabase_template",
				         KStandardDirs::kde_default("data") + "kdatabase/templates/");
        s_global->dirs()->addResourceType( "toolbar",
				         KStandardDirs::kde_default("data") + "koffice/toolbar/");

    }
    return s_global;
}

#include <kdatabase_factory.moc>
