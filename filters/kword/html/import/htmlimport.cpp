/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Frank Dekervel <Frank.Dekervel@student.kuleuven.ac.be>

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

#include <htmlimport.h>
#include <htmlimport.moc>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>

#include <KoStore.h>

#include <khtmlreader.h>
#include <kwdwriter.h>
//Added by qt3to4:
#include <Q3CString>

typedef KGenericFactory<HTMLImport> HTMLImportFactory;
K_EXPORT_COMPONENT_FACTORY( libhtmlimport, HTMLImportFactory( "kofficefilters" ) )


HTMLImport::HTMLImport(QObject* parent, const QStringList &) :
                     KoFilter(parent) {
// 123
}

KoFilter::ConversionStatus HTMLImport::convert( const QByteArray& from, const QByteArray& to )
{
	if(to!="application/x-kword" || from!="text/html")
		return KoFilter::NotImplemented;

	KoStore *k= KoStore::createStore(m_chain->outputFile(), KoStore::Write, "application/x-kword");
	KWDWriter *w= new KWDWriter(k);
	KHTMLReader h(w);
	KUrl url;
	url.setPath(m_chain->inputFile());
	bool b= h.filter(url);
	delete(w);
	delete(k);

        if ( b )
            return KoFilter::OK;
        else
            return KoFilter::StupidError;
}
