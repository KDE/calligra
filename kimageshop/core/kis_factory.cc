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
#include <klocale.h>
#include <kstddirs.h>
#include <kaboutdata.h>
#include <kiconloader.h>

#include "kis_factory.h"
#include "kis_pluginserver.h"
#include "kis_resourceserver.h"
#include "kis_doc.h"
#include "kis_log.h"

extern "C"
{
    void* init_libkimageshop()
    {
	return new KisFactory;
    }
};

KAboutData* KisFactory::s_aboutData = 0;
KInstance* KisFactory::s_global = 0;
KisPluginServer* KisFactory::s_pserver = 0;
KisResourceServer* KisFactory::s_rserver = 0;

KisFactory::KisFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    s_aboutData = new KAboutData( "kimageshop",
	I18N_NOOP("KImageShop"),
	"0.1.0",
	I18N_NOOP("KOffice image manipulation application."),
	KAboutData::License_GPL,
	"(c) 1999-2000 The KImageShop team.",
	"",
	"http://koffice.kde.org",
	"submit@bugs.kde.org");

    (void)global();
    s_pserver = new KisPluginServer;
    s_rserver = new KisResourceServer;

    //KisLog::setLogFile(locateLocal("kis", "kimageshop.log", s_global)); //jwc
    // jwc - setLogFile() expect char * for not QString

    QString FileName = locateLocal("kis", "kimageshop.log", s_global);  
    char *latinFileName = FileName.latin1();
    KisLog::setLogFile(latinFileName);
  
    log() << "Starting KImageShop" << endl;
}

KisFactory::~KisFactory()
{
    delete s_pserver;
    delete s_rserver;
    delete s_aboutData;
    delete s_global;
}

/*
    Create the document
*/

KParts::Part* KisFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KisDoc *doc = new KisDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
        doc->setReadWrite( false );

    emit objectCreated(doc);
    return doc;
}

KInstance* KisFactory::global()
{
    if ( !s_global )
    {
	s_global = new KInstance(s_aboutData);

	s_global->dirs()->addResourceType("kis",
	    KStandardDirs::kde_default("data") + "kimageshop/");

	s_global->dirs()->addResourceType("kis_images",
	    KStandardDirs::kde_default("data") + "kimageshop/images/");

	s_global->dirs()->addResourceType("kis_brushes",
	    KStandardDirs::kde_default("data") + "kimageshop/brushes/");

	s_global->dirs()->addResourceType("kis_pattern",
            KStandardDirs::kde_default("data") + "kimageshop/patterns/");

	s_global->dirs()->addResourceType("kis_gradients",
	    KStandardDirs::kde_default("data") + "kimageshop/gradients/");

	s_global->dirs()->addResourceType("kis_pics",
	    KStandardDirs::kde_default("data") + "kimageshop/pics/");

	s_global->dirs()->addResourceType("kis_plugins",
	    KStandardDirs::kde_default("data") + "kimageshop/plugins/");

	s_global->dirs()->addResourceType("toolbars",
	    KStandardDirs::kde_default("data") + "koffice/toolbar/");
                                          
	// Tell the iconloader about share/apps/koffice/icons
	s_global->iconLoader()->addAppDir("koffice");
    }

    return s_global;
}

KAboutData* KisFactory::aboutData()
{
    return s_aboutData;
}

KisPluginServer* KisFactory::pServer()
{
    return s_pserver;
}

KisResourceServer* KisFactory::rServer()
{
    return s_rserver;
}

#include "kis_factory.moc"
