/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_factory.h"
#include "kivio_doc.h"

#include <kaboutdata.h>
#include <klocale.h>
#include <kinstance.h>
#include <kstddirs.h>
#include <kiconloader.h>

#include "kivio_config.h"

static const char* description=I18N_NOOP("KOffice Kivio Application");
static const char* version="0.1";

extern "C"
{
  void* init_libkiviopart()
  {
    return new KivioFactory;
  }
};

KInstance* KivioFactory::s_global = 0;
KAboutData* KivioFactory::s_aboutData = 0;

KivioFactory::KivioFactory( QObject* parent, const char* name )
: KoFactory( parent, name )
{
  global();
}

KivioFactory::~KivioFactory()
{
  if ( s_aboutData )
  {
    delete s_aboutData;
    s_aboutData=0;
  }
  if ( s_global )
  {
    delete s_global;
    s_global = 0L;
  }

  // FIXME: Is this the right place to do this?
  if( KivioConfig::config() )
  {
    KivioConfig::deleteConfig();
  }
}

KParts::Part* KivioFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
  bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

  KivioDoc *doc = new KivioDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  emit objectCreated( doc );
  return doc;
}

KAboutData* KivioFactory::aboutData()
{
  if( !s_aboutData )
  {
    s_aboutData = new KAboutData( "kivio", I18N_NOOP("Kivio"),
                                  version, description, KAboutData::License_GPL,
                                  "theKompany.com - Kivio",0,"www.thekompany.com");
    s_aboutData->addAuthor("Dave Marotti","Main author and the original author of Queesio, from which this source is based","lndshark@speakeasy.net");
    s_aboutData->addAuthor("Max Judin","GUI widgets","max@thekompany.com");
  }
  return s_aboutData;
}

KInstance* KivioFactory::global()
{
  if ( !s_global )
  {
    s_global = new KInstance(aboutData());
    s_global->dirs()->addResourceType( "toolbar", KStandardDirs::kde_default("data") + "koffice/toolbar/");
    s_global->iconLoader()->addAppDir("koffice");
  }

  // FIXME: Is this the place to do it?
  // Also load the config information here
  if( KivioConfig::config() == NULL )
  {
    KivioConfig::readUserConfig();
  }

  return s_global;
}
#include "kivio_factory.moc"
