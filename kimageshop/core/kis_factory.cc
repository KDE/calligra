/*
 *  kis_factory.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>

#include "kis_factory.h"
#include "kis_pluginserver.h"
#include "kis_brushserver.h"
#include "kis_doc.h"

extern "C"
{
    void* init_libkimageshop()
    {
	return new KisFactory;
    }
};

KInstance* KisFactory::s_global = 0;
KisPluginServer* KisFactory::s_pserver = 0;
KisBrushServer* KisFactory::s_bserver = 0;

KisFactory::KisFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
  (void)global();
  s_pserver = new KisPluginServer;
  s_bserver = new KisBrushServer;
}

KisFactory::~KisFactory()
{
  delete s_pserver;
  if ( s_global )
    delete s_global;
}

QObject* KisFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
/*
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KisFactory: parent does not inherit KoDocument");
	return 0;
    }
*/
    bool bWantKoDocument = ( strcmp( classname, "KofficeDocument" ) == 0 );

    KisDoc *doc = new KisDoc( parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      doc->setReadWrite( false );

    emit objectCreated(doc);
    return doc;
}

KInstance* KisFactory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance( "kimageshop" );

      s_global->dirs()->addResourceType("kis", KStandardDirs::kde_default("data") + "kimageshop/");
      s_global->dirs()->addResourceType("kis_images", KStandardDirs::kde_default("data") + "kimageshop/images/");
      s_global->dirs()->addResourceType("kis_brushes", KStandardDirs::kde_default("data") + "kimageshop/brushes/");
      s_global->dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "koffice/toolbar/");
      s_global->dirs()->addResourceType("kis_pics", KStandardDirs::kde_default("data") + "kimageshop/pics/");
      s_global->dirs()->addResourceType("kis_plugins", KStandardDirs::kde_default("data") + "kimageshop/plugins/");
      
    }
    return s_global;
}

KisPluginServer* KisFactory::pServer()
{
    return s_pserver;
}

KisBrushServer* KisFactory::bServer()
{
    return s_bserver;
}

#include "kis_factory.moc"
