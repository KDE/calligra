/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_factory.h"
#include "kivio_doc.h"
#include "kivio_aboutdata.h"

#include <klocale.h>
#include <kinstance.h>
#include <kstandarddirs.h>
#include <kiconloader.h>


KInstance* KivioFactory::s_global = 0;
KAboutData* KivioFactory::s_aboutData = 0;

KivioFactory::KivioFactory( QObject* parent, const char* name )
: KoFactory( parent, name )
{
  global();
}

KivioFactory::~KivioFactory()
{
  delete s_aboutData;
  s_aboutData=0;
  delete s_global;
  s_global = 0L;
}

KParts::Part* KivioFactory::createPartObject( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
  bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

  KivioDoc *doc = new KivioDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  return doc;
}

KAboutData* KivioFactory::aboutData()
{
  if( !s_aboutData )
    s_aboutData = newKivioAboutData();
  return s_aboutData;
}

KInstance* KivioFactory::global()
{
  if ( !s_global )
  {
    s_global = new KInstance(aboutData());
    s_global->dirs()->addResourceType( "kivio_template", KStandardDirs::kde_default("data") + "kivio/templates/" );
    s_global->dirs()->addResourceType( "toolbar", KStandardDirs::kde_default("data") + "koffice/toolbar/");
    s_global->iconLoader()->addAppDir("koffice");
  }

  return s_global;
}

#include "kivio_factory.moc"
