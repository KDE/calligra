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

#include "WUFactory.h"
#include "WUAboutData.h"
#include "WUDocument.h"

#include <kstandarddirs.h>
#include <kinstance.h>
#include <kiconloader.h>

K_EXPORT_COMPONENT_FACTORY( libwriteup_part, WUFactory )


KInstance*   WUFactory::s_instance  = 0;
KAboutData*  WUFactory::s_aboutData = 0;

WUFactory::WUFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
  // Create our instance, so that it becomes KGlobal::instance if the
  // main app is WriteUp.
  (void) instance();
}

WUFactory::~WUFactory()
{
    delete s_aboutData;
    s_aboutData=0;
    delete s_instance;
    s_instance=0L;
}

KParts::Part* WUFactory::createPartObject( QWidget *parentWidget, const char *widname, QObject* parent, const char* name, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    WUDocument *doc = new WUDocument( parentWidget, widname, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      doc->setReadWrite( false );

    return doc;
}

KAboutData* WUFactory::aboutData()
{
    if(!s_aboutData) {
        s_aboutData = newWriteupAboutData();
    }
    return s_aboutData;
}

KInstance* WUFactory::instance()
{
    if ( !s_instance )
    {
      s_instance = new KInstance( aboutData() );

      s_instance->dirs()->addResourceType( "kword_template",
              KStandardDirs::kde_default("data") + "kword/templates/");
      s_instance->dirs()->addResourceType( "expression", KStandardDirs::kde_default("data") + "kword/expression/");
      s_instance->dirs()->addResourceType( "horizontalLine", KStandardDirs::kde_default("data") + "kword/horizontalline/");

      s_instance->dirs()->addResourceType( "toolbar",
              KStandardDirs::kde_default("data") + "koffice/toolbar/");
      s_instance->dirs()->addResourceType( "toolbar",
              KStandardDirs::kde_default("data") + "kformula/pics/");
      s_instance->iconLoader()->addAppDir("koffice");
    }
    return s_instance;
}

#include "WUFactory.moc"
