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
#include <kimgio.h>
#include <kinstance.h>

#include <qstringlist.h>

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
    s_global = new KInstance( "kword" );

    s_global->dirs()->addResourceType( "kword_template",
				       KStandardDirs::kde_default("data") + "kword/templates/");
    s_global->dirs()->addResourceType( "toolbar",
				       KStandardDirs::kde_default("data") + "koffice/toolbar/");
    s_global->dirs()->addResourceType( "toolbar",
				       KStandardDirs::kde_default("data") + "kformula/pics/");
}

KWordFactory::~KWordFactory()
{
    delete s_global;
}

QObject* KWordFactory::create( QObject* parent, const char* name, const char* /*classname*/, const QStringList & )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KWordFactory: parent does not inherit KoDocument");
	return 0;
    }
    KWordDocument *doc = new KWordDocument( (KoDocument*)parent, name );
    emit objectCreated(doc);
    return doc;
}

KInstance* KWordFactory::global()
{
    return s_global;
}

#include "kword_factory.moc"
