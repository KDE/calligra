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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "mswordimport.h"

#include <qdom.h>
#include <qfontinfo.h>
#include <qfile.h>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <koFilterChain.h>

#include <document.h>

typedef KGenericFactory<MSWordImport, KoFilter> MSWordImportFactory;
K_EXPORT_COMPONENT_FACTORY( libmswordimport, MSWordImportFactory( "mswordimport" ) );

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

    kdDebug() << "######################## MSWordImport::convert ########################" << endl;

    QDomDocument mainDocument;
    QDomElement mainFramesetElement;
    prepareDocument( mainDocument, mainFramesetElement );

    Document document( QFile::encodeName( m_chain->inputFile() ).data(), mainDocument, mainFramesetElement );
    document.parse();

    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if ( !out ) {
        kdError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QCString cstr = mainDocument.toCString();
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
    out->writeBlock( cstr, cstr.length() );

    kdDebug() << "######################## MSWordImport::convert done ####################" << endl;
    return KoFilter::OK;
}

void MSWordImport::prepareDocument( QDomDocument& mainDocument, QDomElement& mainFramesetElement )
{
    mainDocument.appendChild( mainDocument.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement elementDoc;
    elementDoc=mainDocument.createElement("DOC");
    elementDoc.setAttribute("editor","KWord's MS Word Import Filter");
    elementDoc.setAttribute("mime","application/x-kword");
    // TODO: We claim to be syntax version 2, but we should verify that it is also true.
    elementDoc.setAttribute("syntaxVersion",2);
    mainDocument.appendChild(elementDoc);

    QDomElement framesetsPluralElementOut=mainDocument.createElement("FRAMESETS");
    mainDocument.documentElement().appendChild(framesetsPluralElementOut);

    mainFramesetElement=mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("autoCreateNewFrame",1);
    mainFramesetElement.setAttribute("removable",0);
    // TODO: "name" attribute (needs I18N)
    framesetsPluralElementOut.appendChild(mainFramesetElement);

    QDomElement frameElementOut=mainDocument.createElement("FRAME");
    ///// Those values are unused. The paper margins make recalcFrames() resize this frame.
    frameElementOut.setAttribute("left",28);
    frameElementOut.setAttribute("top",42);
    frameElementOut.setAttribute("bottom",566);
    frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    mainFramesetElement.appendChild(frameElementOut);
}

#include <mswordimport.moc>
