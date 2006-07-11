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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>
#include <kinstance.h>
#include <kstandarddirs.h>

#include "AboutData.h"
#include "Doc.h"
// #include "KSpreadAppIface.h"

#include "Factory.h"

using namespace KSpread;

KInstance* Factory::s_global = 0;
// DCOPObject* Factory::s_dcopObject = 0;
KAboutData* Factory::s_aboutData = 0;

Factory::Factory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
  //kDebug(36001) << "Factory::Factory()" << endl;
  // Create our instance, so that it becomes KGlobal::instance if the
  // main app is KSpread.
  (void)global();
//   (void)dcopObject();
}

Factory::~Factory()
{
  //kDebug(36001) << "Factory::~Factory()" << endl;
  delete s_aboutData;
  s_aboutData=0;
  delete s_global;
  s_global = 0;
//   delete s_dcopObject;
//   s_dcopObject = 0;
}

KParts::Part* Factory::createPartObject( QWidget *parentWidget, QObject* parent, const char* classname, const QStringList & )
{
  bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

  Doc *doc = new Doc( parentWidget, parent, !bWantKoDocument );

  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  return doc;
}

KAboutData* Factory::aboutData()
{
  if( !s_aboutData )
      s_aboutData = newAboutData();
  return s_aboutData;
}

KInstance* Factory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance(aboutData());

      s_global->dirs()->addResourceType( "kspread_template",
                                          KStandardDirs::kde_default("data") + "kspread/templates/");

      s_global->dirs()->addResourceType( "toolbar",
				         KStandardDirs::kde_default("data") + "koffice/toolbar/");
      s_global->dirs()->addResourceType( "functions", KStandardDirs::kde_default("data") + "kspread/functions/");
      s_global->dirs()->addResourceType( "sheet-styles", KStandardDirs::kde_default("data") + "kspread/sheetstyles/");
      // Tell the iconloader about share/apps/koffice/icons
      s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

// DCOPObject* Factory::dcopObject()
// {
//     if ( !s_dcopObject )
//         s_dcopObject = new AppIface;
//     return s_dcopObject;
// }

#include "Factory.moc"
