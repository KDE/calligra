/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "mswordimport.h"

#include <qdom.h>
#include <qfontinfo.h>
#include <qfile.h>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>

#include <document.h>

typedef KGenericFactory<MSWordImport, KoFilter> MSWordImportFactory;
K_EXPORT_COMPONENT_FACTORY( libmswordimport, MSWordImportFactory( "kofficefilters" ) )

MSWordImport::MSWordImport( KoFilter *, const char *, const QStringList& ) : KoFilter()
{
}

MSWordImport::~MSWordImport()
{
}

KoFilter::ConversionStatus MSWordImport::convert( const QCString& from, const QCString& to )
{
    // check for proper conversion
    if ( to != "application/x-kword" || from != "application/msword" )
        return KoFilter::NotImplemented;

    kdDebug(30513) << "######################## MSWordImport::convert ########################" << endl;

    QDomDocument mainDocument;
    QDomElement framesetsElem;
    prepareDocument( mainDocument, framesetsElem );

    QDomDocument documentInfo;
    documentInfo.appendChild (documentInfo.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    Document document( QFile::encodeName( m_chain->inputFile() ).data(), mainDocument, documentInfo, framesetsElem, m_chain );

    if ( !document.hasParser() )
        return KoFilter::WrongFormat;
    if ( !document.parse() )
        return KoFilter::ParsingError;
    document.processSubDocQueue();
    document.finishDocument();
    if ( !document.bodyFound() )
        return KoFilter::WrongFormat;

    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if ( !out ) {
        kdError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QCString cstr = mainDocument.toCString();
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
    out->writeBlock( cstr, cstr.length() );
    out->close();

    out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if ( !out ) {
	return KoFilter::StorageCreationError;
    }

    cstr = documentInfo.toCString();
    out->writeBlock( cstr, cstr.length() );
    out->close();

    kdDebug(30513) << "######################## MSWordImport::convert done ####################" << endl;
    return KoFilter::OK;
}

void MSWordImport::prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem )
{
    mainDocument.appendChild( mainDocument.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement elementDoc;
    elementDoc=mainDocument.createElement("DOC");
    elementDoc.setAttribute("editor","KWord's MS Word Import Filter");
    elementDoc.setAttribute("mime","application/x-kword");
    elementDoc.setAttribute("syntaxVersion",2);
    mainDocument.appendChild(elementDoc);

    framesetsElem=mainDocument.createElement("FRAMESETS");
    mainDocument.documentElement().appendChild(framesetsElem);
}

#include <mswordimport.moc>
