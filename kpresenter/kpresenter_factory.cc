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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpresenter_factory.h"
#include "kpresenter_doc.h"
#include "preview.h"

#include <kimgio.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kinstance.h>

#include <qstringlist.h>

extern "C"
{
    void* init_libkpresenter()
    {
	/**
	 * Initialize KPresenter stuff
	 */
		
#ifndef USE_QFD
	// Image IO handler
	//KFilePreviewDialog::registerPreviewModule( "wmf", wmfPreviewHandler, PreviewPixmap );
	//KFilePreviewDialog::registerPreviewModule( "WMF", wmfPreviewHandler, PreviewPixmap );
	
	//QStringList list = KImageIO::types(KImageIO::Reading);
	//QStringList::ConstIterator it;
	//for (it = list.begin(); it != list.end(); it++)
	//    KFilePreviewDialog::registerPreviewModule( *it, pixmapPreviewHandler, PreviewPixmap );
#endif

	return new KPresenterFactory;
    }
};


KInstance* KPresenterFactory::s_global = 0;

KPresenterFactory::KPresenterFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
}

KPresenterFactory::~KPresenterFactory()
{
    if ( s_global ) 
      delete s_global;
}

QObject* KPresenterFactory::create( QObject* parent, const char* name,
				    const char *classname, const QStringList & )
{
/*
   if ( parent && !parent->inherits("KoDocument") ) {
	qDebug("KPresenterFactory: parent does not inherit KoDocument");
	return 0;
    }
*/
    bool bWantKoDocument = ( strcmp( classname, "KofficeDocument" ) == 0 );

    KPresenterDoc *doc = new KPresenterDoc( (KoDocument*)parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
    {
      doc->initEmpty();
      doc->setReadWrite( false );
    }

    emit objectCreated(doc);
    return doc;
}

KInstance* KPresenterFactory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance( "kpresenter" );

      s_global->dirs()->addResourceType("kpresenter_template",
				        KStandardDirs::kde_default("data") + "kpresenter/templates/");
      s_global->dirs()->addResourceType("autoforms",
				        KStandardDirs::kde_default("data") + "kpresenter/autoforms/");
      s_global->dirs()->addResourceType("toolbar",
				        KStandardDirs::kde_default("data") + "koffice/toolbar/");
      s_global->dirs()->addResourceType("toolbar",
 				        KStandardDirs::kde_default("data") + "kpresenter/toolbar/");
    }
    return s_global;
}

#include "kpresenter_factory.moc"
