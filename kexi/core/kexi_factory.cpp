/***************************************************************************
                          kexi_factory.cpp  -  description
                             -------------------
    begin                : Sun Nov  17 23:30:00 CET 2002
    copyright            : (C) 2002 Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kexi_factory.h>
#include <kexi_factory.moc>
#include <kexiproject.h>
#include <kexi_aboutdata.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

KInstance* KexiFactory::s_global = 0L;
KAboutData* KexiFactory::s_aboutData = 0L;

KexiFactory::KexiFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

KexiFactory::~KexiFactory()
{
    delete s_aboutData;
    s_aboutData = 0L;
    delete s_global;
    s_global = 0L;
}

KParts::Part* KexiFactory::createPartObject( QWidget *parentWidget, const char *widgetName,
	QObject* parent, const char* name, const char* classname, const QStringList & )

{
    // If classname is "KoDocument", our host is a koffice application
    // otherwise, the host wants us as a simple part, so switch to readonly and single view.
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    // parentWidget and widgetName are used by KoDocument for the "readonly+singleView" case.
    KexiProject *part = new KexiProject( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    return part;
}

KAboutData* KexiFactory::aboutData()
{
    if ( !s_aboutData )
        s_aboutData = newKexiAboutData();
    return s_aboutData;
}

KInstance* KexiFactory::global()
{
    if ( !s_global )
    {
        s_global = new KInstance( aboutData() );
        // Add any application-specific resource directories here

        // Tell the iconloader about share/apps/koffice/icons
        s_global->iconLoader()->addAppDir("koffice");
        s_global->iconLoader()->addAppDir("kexi");
    }
    return s_global;
}

