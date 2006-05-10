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
#include <QFile>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>

#include <document.h>

typedef KGenericFactory<MSWordImport> MSWordImportFactory;
K_EXPORT_COMPONENT_FACTORY( libmswordimport, MSWordImportFactory( "kofficefilters" ) )

MSWordImport::MSWordImport( QObject *parent, const QStringList& ) : KoFilter(parent)
{
}

MSWordImport::~MSWordImport()
{
}

KoFilter::ConversionStatus MSWordImport::convert( const QByteArray& from, const QByteArray& to )
{
    // check for proper conversion
    if ( to != "application/x-kword" || from != "application/msword" )
        return KoFilter::NotImplemented;

    kDebug(30513) << "######################## MSWordImport::convert ########################" << endl;

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
        kError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QByteArray cstr = mainDocument.toByteArray();
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
    out->write( cstr, cstr.length() );
    out->close();

    out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if ( !out ) {
	return KoFilter::StorageCreationError;
    }

    cstr = documentInfo.toByteArray();
    out->write( cstr, cstr.length() );
    out->close();

    kDebug(30513) << "######################## MSWordImport::convert done ####################" << endl;
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
