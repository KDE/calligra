/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include "kspread_doc.h"
#include "kspread_aboutdata.h"
#include "KSpreadAppIface.h"

#include <kstandarddirs.h>
#include <kdebug.h>

K_EXPORT_COMPONENT_FACTORY( libkspread, KSpreadFactory )

KInstance* KSpreadFactory::s_global = 0;
DCOPObject* KSpreadFactory::s_dcopObject = 0;
KAboutData* KSpreadFactory::s_aboutData = 0;

KSpreadFactory::KSpreadFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
  //kdDebug(36001) << "KSpreadFactory::KSpreadFactory()" << endl;
  // Create our instance, so that it becomes KGlobal::instance if the
  // main app is KSpread.
  (void)global();
  (void)dcopObject();
}

KSpreadFactory::~KSpreadFactory()
{
  //kdDebug(36001) << "KSpreadFactory::~KSpreadFactory()" << endl;
  delete s_aboutData;
  s_aboutData=0;
  delete s_global;
  s_global = 0L;
  delete s_dcopObject;
  s_dcopObject = 0L;
}

KParts::Part* KSpreadFactory::createPartObject( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
  bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

  KSpreadDoc *doc = new KSpreadDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  return doc;
}

KAboutData* KSpreadFactory::aboutData()
{
  if( !s_aboutData )
      s_aboutData = newKSpreadAboutData();
  return s_aboutData;
}

KInstance* KSpreadFactory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance(aboutData());

      s_global->dirs()->addResourceType( "kspread_template",
                                          KStandardDirs::kde_default("data") + "kspread/templates/");

      s_global->dirs()->addResourceType( "toolbar",
				         KStandardDirs::kde_default("data") + "koffice/toolbar/");
      s_global->dirs()->addResourceType( "extensions", KStandardDirs::kde_default("data") + "kspread/extensions/");
      s_global->dirs()->addResourceType( "table-styles", KStandardDirs::kde_default("data") + "kspread/tablestyles/");
      // Tell the iconloader about share/apps/koffice/icons
      s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

DCOPObject* KSpreadFactory::dcopObject()
{
    if ( !s_dcopObject )
        s_dcopObject = new KSpreadAppIface;
    return s_dcopObject;
}

#include "kspread_factory.moc"
