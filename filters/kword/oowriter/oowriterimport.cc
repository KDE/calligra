/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#include <qcolor.h>
#include <qfile.h>
#include <qfont.h>
#include <qpen.h>

#include "oowriterimport.h"

#include <kdebug.h>
#include <koDocumentInfo.h>
#include <koDocument.h>

#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <koFilterChain.h>
#include <koGlobal.h>

typedef KGenericFactory<OoWriterImport, KoFilter> OoWriterImportFactory;
K_EXPORT_COMPONENT_FACTORY( liboowriterimport, OoWriterImportFactory( "oowriterimport" ) );


OoWriterImport::OoWriterImport( KoFilter *, const char *, const QStringList & )
  : KoFilter()
{
}

OoWriterImport::~OoWriterImport()
{
}

KoFilter::ConversionStatus OoWriterImport::convert( QCString const & from, QCString const & to )
{
    kdDebug() << "Entering Oowriter Import filter: " << from << " - " << to << endl;

    if ( from != "application/x-oowriter" || to != "application/x-kword" )
    {
        kdWarning() << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    KoFilter::ConversionStatus preStatus = openFile();

    if ( preStatus != KoFilter::OK )
        return preStatus;

    QDomDocument mainDocument;
    QDomElement framesetsElem;
    prepareDocument( mainDocument, framesetsElem );
    createPageDocument( mainDocument, framesetsElem );

    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if ( !out ) {
        kdError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QCString cstr = mainDocument.toCString();
    kdDebug()<<" cstr :"<<cstr<<endl;
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
    out->writeBlock( cstr, cstr.length() );


    QDomDocument docinfo;
    createDocumentInfo( docinfo );

    // store document info
    out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if( out )
    {
        QCString info = docinfo.toCString();
        kdDebug()<<" info :"<<info<<endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->writeBlock( info , info.length() );
    }

    kdDebug() << "######################## OoWriterImport::convert done ####################" << endl;
    return KoFilter::OK;
}

void OoWriterImport::prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem )
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

void OoWriterImport::createPageDocument( QDomDocument& mainDocument, QDomElement& framesetsElem )
{
    QDomElement elementDoc = mainDocument.documentElement();
    QDomElement elementPaper = mainDocument.createElement("PAPER");

    elementPaper.setAttribute("format",PG_US_LETTER);
    elementPaper.setAttribute("width", KoPageFormat::width (PG_US_LETTER,PG_PORTRAIT));
    elementPaper.setAttribute("height",KoPageFormat::height(PG_US_LETTER,PG_PORTRAIT));
    elementPaper.setAttribute("orientation",PG_PORTRAIT);
    elementPaper.setAttribute("columns",1);
    elementPaper.setAttribute("columnspacing",2);
    elementPaper.setAttribute("hType",0);
    elementPaper.setAttribute("fType",0);
    elementPaper.setAttribute("spHeadBody",9);
    elementPaper.setAttribute("spFootBody",9);
    elementPaper.setAttribute("zoom",100);
    elementDoc.appendChild(elementPaper);

    QDomElement mainFramesetElement = mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("visible",1);
    framesetsElem.appendChild(mainFramesetElement);

    createInitialFrame( mainFramesetElement, 42, 566, false );

}

void OoWriterImport::createInitialFrame( QDomElement& parentFramesetElem, int top, int bottom, bool headerFooter )
{
    QDomElement frameElementOut = parentFramesetElem.ownerDocument().createElement("FRAME");
    // Those values are unused. The paper margins make recalcFrames() resize this frame.
    frameElementOut.setAttribute( "left", 28 );
    frameElementOut.setAttribute( "right", 798 );
    frameElementOut.setAttribute( "top", top );
    frameElementOut.setAttribute( "bottom", bottom );
    frameElementOut.setAttribute( "runaround", 1 );
    // AutoExtendFrame for header/footers, AutoCreateNewFrame for body text
    frameElementOut.setAttribute( "autoCreateNewFrame", headerFooter ? 0 : 1 );
    parentFramesetElem.appendChild( frameElementOut );
}


KoFilter::ConversionStatus OoWriterImport::openFile()
{
  KoStore * store = KoStore::createStore( m_chain->inputFile(), KoStore::Read);

  kdDebug() << "Store created" << endl;

  if ( !store )
  {
    kdWarning() << "Couldn't open the requested file." << endl;
    return KoFilter::FileNotFound;
  }

  kdDebug() << "Trying to open content.xml" << endl;
  if ( !store->open( "content.xml" ) )
  {
    kdWarning() << "This file doesn't seem to be a valid OpenCalc file" << endl;
    delete store;
    return KoFilter::WrongFormat;
  }
  kdDebug() << "Opened" << endl;

  QDomDocument styles;
  QCString totalString;
  char tempData[1024];

  Q_LONG size = store->read( &tempData[0], 1023 );
  while ( size > 0 )
  {
    QCString tempString( tempData, size + 1);
    totalString += tempString;

    size = store->read( &tempData[0], 1023 );
  }

  m_content.setContent( totalString );
  totalString = "";
  store->close();
  kdDebug()<<" m_content.toCString() :"<<m_content.toCString()<<endl;
  kdDebug() << "file content.xml loaded " << endl;

  if ( store->open( "styles.xml" ) )
  {
    size = store->read( &tempData[0], 1023 );
    while ( size > 0 )
    {
      QCString tempString( tempData, size + 1);
      totalString += tempString;

      size = store->read( &tempData[0], 1023 );
    }

    styles.setContent( totalString );
    totalString = "";
    store->close();
    kdDebug() << "file containing styles loaded" << endl;
  }
  else
    kdWarning() << "Style definitions do not exist!" << endl;

  if ( store->open( "meta.xml" ) )
  {
    size = store->read( &tempData[0], 1023 );
    while ( size > 0 )
    {
      QCString tempString( tempData, size + 1);
      totalString += tempString;

      size = store->read( &tempData[0], 1023 );
    }

    m_meta.setContent( totalString );
    totalString = "";
    store->close();
    kdDebug() << "File containing meta definitions loaded" << endl;
  }
  else
    kdWarning() << "Meta definitions do not exist!" << endl;

  if ( store->open( "settings.xml" ) )
  {
    size = store->read( &tempData[0], 1023 );
    while ( size > 0 )
    {
      QCString tempString( tempData, size + 1);
      totalString += tempString;

      size = store->read( &tempData[0], 1023 );
    }

    m_settings.setContent( totalString );
    totalString = "";
    store->close();
    kdDebug() << "File containing settings loaded" << endl;
  }
  else
    kdWarning() << "Settings do not exist!" << endl;

  delete store;

  emit sigProgress( 10 );

  return KoFilter::OK;
}

void OoWriterImport::createDocumentInfo( QDomDocument &docinfo )
{
    docinfo.appendChild( docinfo.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomDocument doc = KoDocument::createDomDocument( "document-info" /*DTD name*/, "document-info" /*tag name*/, "1.1" );

    QDomNode meta   = m_meta.namedItem( "office:document-meta" );
    QDomNode office = meta.namedItem( "office:meta" );

    if ( office.isNull() )
        return;
    QDomElement elementDocInfo  = doc.documentElement();

    QDomElement e = office.namedItem( "dc:creator" ).toElement();
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        QDomElement author = doc.createElement( "author" );
        QDomElement t = doc.createElement( "full-name" );
        author.appendChild( t );
        t.appendChild( doc.createTextNode( e.text() ) );
        elementDocInfo.appendChild( author);
    }

    e = office.namedItem( "dc:title" ).toElement();
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        QDomElement title = doc.createElement( "title" );
        title.appendChild( doc.createTextNode( e.text() ) );
        elementDocInfo.appendChild( title);

    }
#if 0
    e = office.namedItem( "dc:description" ).toElement();
    if ( !e.isNull() && !e.text().isEmpty() )
    {

    }
#endif
    docinfo.appendChild(doc);

    kdDebug()<<" meta-info :"<<m_meta.toCString()<<endl;
}

#include <oowriterimport.moc>

