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

#include "kword_factory.h"
#include "kword_doc.h"
#include "preview.h"

#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kimageio.h>
#include <kinstance.h>
#include <klocale.h>
#include <kaboutdata.h>

#include <qstringlist.h>

static const char* description=I18N_NOOP("KOffice Word Processor");
static const char* version="0.1";

extern "C"
{
    void* init_libkword()
    {
	/**
	 * Initialize KWord stuff
	 */

/*#ifndef USE_QFD
	// Image IO handler
	KFilePreviewDialog::registerPreviewModule( "wmf", wmfPreviewHandler, PreviewPixmap );
	KFilePreviewDialog::registerPreviewModule( "WMF", wmfPreviewHandler, PreviewPixmap );

	QStringList list = KImageIO::types(KImageIO::Reading);
	QStringList::ConstIterator it;
	for (it = list.begin(); it != list.end(); it++)
	    KFilePreviewDialog::registerPreviewModule( *it, pixmapPreviewHandler, PreviewPixmap );
#endif*/

	return new KWordFactory;
    }
};


KInstance* KWordFactory::s_global = 0;

KWordFactory::KWordFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
}

KWordFactory::~KWordFactory()
{
    if ( s_global )
      delete s_global;
}

QObject* KWordFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
/*
 if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KWordFactory: parent does not inherit KoDocument");
	return 0;
    }
*/
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KWordDocument *doc = new KWordDocument( parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
    {
    //      doc->initEmpty();
      doc->setReadWrite( false );
    }

    emit objectCreated(doc);
    return doc;
}

KAboutData* KWordFactory::aboutData()
{
      KAboutData *aboutData=new KAboutData( "kword", I18N_NOOP("KWord"),
                                version, description, KAboutData::License_GPL,
                                "(c) 1998-2000, Reginald Stadlbauer");
      aboutData->addAuthor("Reginald Stadlbauer",0, "reggie@kde.org");
      return aboutData;
}

KInstance* KWordFactory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance( aboutData() );

      s_global->dirs()->addResourceType( "kword_template",
				         KStandardDirs::kde_default("data") + "kword/templates/");
      s_global->dirs()->addResourceType( "toolbar",
				         KStandardDirs::kde_default("data") + "koffice/toolbar/");
      s_global->dirs()->addResourceType( "toolbar",
				         KStandardDirs::kde_default("data") + "kformula/pics/");
    }
    return s_global;
}

#include "kword_factory.moc"
