/* This file is part of the KDE project

   Copyright 1999-2007  Kalle Dalheimer <kalle@kde.org>
   Copyright 2005-2007  Inge Wallin <inge@lysator.liu.se>

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
   Boston, MA 02110-1301, USA.
*/


// Local
#include "kchart_factory.h"

// KDE
#include <klocale.h>
#include <kcomponentdata.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

// KChart
#include "kchart_aboutdata.h"
#include "KChartPart.h"


namespace KChart
{

KComponentData *KChartFactory::s_global     = 0;
KIconLoader    *KChartFactory::s_iconLoader = 0;
KAboutData     *KChartFactory::s_aboutData  = 0;


KChartFactory::KChartFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}


KChartFactory::~KChartFactory()
{
    delete s_aboutData;
    s_aboutData = 0;
    delete s_global;
    s_global = 0;
    delete s_iconLoader;
    s_iconLoader = 0;
}


KParts::Part* KChartFactory::createPartObject( QWidget *parentWidget,
					       QObject* parent,
					       const char *classname,
					       const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KChartPart *part = new KChartPart( parentWidget, parent,
				       !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    return part;
}


KAboutData* KChartFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData = newKChartAboutData();
    return s_aboutData;
}


const KComponentData &KChartFactory::global()
{
    if ( !s_global ) {
         s_global = new KComponentData(aboutData());
         s_global->dirs()->addResourceType( "kchart_template",
         KStandardDirs::kde_default("data") + "kchart/templates/");
    }

    return *s_global;
}


KIconLoader* KChartFactory::iconLoader()
{
  if ( !s_iconLoader ) {
      // Tell the iconloader about share/apps/koffice/icons
      s_iconLoader = new KIconLoader(global().componentName(), global().dirs());
      s_iconLoader->addAppDir("koffice");
  }
  
  return s_iconLoader;
}


}  //namespace KChart

#include <kchart_factory.moc>
