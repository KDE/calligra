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
    m_styles.setAutoDelete( true );
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
    kdDebug()<<" styles.toCString() :"<<styles.toCString()<<endl;
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

  if ( !createStyleMap( styles ) )
      return KoFilter::UserCancelled;

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
        QDomElement about = doc.createElement( "about" );
        QDomElement title = doc.createElement( "title" );
        about.appendChild( title );
        title.appendChild( doc.createTextNode( e.text() ) );
        elementDocInfo.appendChild( about );
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

bool OoWriterImport::createStyleMap( const QDomDocument & styles )
{
  QDomElement content  = styles.documentElement();
  QDomNode docStyles   = content.namedItem( "office:document-styles" );

  if ( content.hasAttribute( "office:version" ) )
  {
    bool ok = true;
    double d = content.attribute( "office:version" ).toDouble( &ok );

    if ( ok )
    {
      kdDebug() << "OpenCalc version: " << d << endl;
      if ( d > 1.0 )
      {
        QString message( i18n("This document was created with the OpenOffice version '%1'. This filter was written for version for 1.0. Reading this file could cause strange behavior, crashes or incorrect display of the data. Do you want to continue converting the document?") );
        message.arg( content.attribute( "office:version" ) );
        if ( KMessageBox::warningYesNo( 0, message, i18n( "Unsupported document version" ) ) == KMessageBox::No )
          return false;
      }
    }
  }

  QDomNode fontStyles = content.namedItem( "office:font-decls" );

  if ( !fontStyles.isNull() )
  {
    kdDebug() << "Starting reading in font-decl..." << endl;

    insertStyles( fontStyles.toElement() );
  }
  else
    kdDebug() << "No items found" << endl;

  kdDebug() << "Starting reading in auto:styles" << endl;

  QDomNode autoStyles = content.namedItem( "office:automatic-styles" );
  if ( !autoStyles.isNull() )
  {
      insertStyles( autoStyles.toElement() );
  }
  else
    kdDebug() << "No items found" << endl;


  kdDebug() << "Reading in master styles" << endl;

  QDomNode masterStyles = content.namedItem( "office:master-styles" );

  if ( masterStyles.isNull() )
  {
    kdDebug() << "Nothing found " << endl;
  }

  QDomElement master = masterStyles.namedItem( "style:master-page").toElement();
  if ( !master.isNull() )
  {
    QString name( "pm" );
    name += master.attribute( "style:name" );
    kdDebug() << "Master style: '" << name << "' loaded " << endl;
    m_styles.insert( name, new QDomElement( master ) );

    master = master.nextSibling().toElement();
  }


  kdDebug() << "Starting reading in office:styles" << endl;

  QDomNode fixedStyles = content.namedItem( "office:styles" );

  kdDebug() << "Reading in default styles" << endl;

  QDomNode def = fixedStyles.namedItem( "style:default-style" );
  while ( !def.isNull() )
  {
    QDomElement e = def.toElement();
    kdDebug() << "Style found " << e.nodeName() << ", tag: " << e.tagName() << endl;

    if ( e.nodeName() != "style:default-style" )
    {
      def = def.nextSibling();
      continue;
    }

    if ( !e.isNull() )
    {
      kdDebug() << "Default style " << e.attribute( "style:family" ) << "default" << " loaded " << endl;

      //m_defaultStyles.insert( e.attribute( "style:family" ) + "default", layout );
    }
    def = def.nextSibling();
  }

  QDomElement defs = fixedStyles.namedItem( "style:style" ).toElement();
  while ( !defs.isNull() )
  {
    if ( defs.nodeName() != "style:style" )
      break; // done

    if ( !defs.hasAttribute( "style:name" ) )
    {
      // ups...
      defs = defs.nextSibling().toElement();
      continue;
    }

    //m_defaultStyles.insert( defs.attribute( "style:name" ), layout );

    defs = defs.nextSibling().toElement();
  }

  if ( !fixedStyles.isNull() )
    insertStyles( fixedStyles.toElement() );

  kdDebug() << "Starting reading in automatic styles" << endl;

  content = m_content.documentElement();
  autoStyles = content.namedItem( "office:automatic-styles" );

  if ( !autoStyles.isNull() )
  {
      insertStyles( autoStyles.toElement() );
  }

  fontStyles = content.namedItem( "office:font-decls" );

  if ( !fontStyles.isNull() )
  {
    kdDebug() << "Starting reading in special font decl" << endl;
    insertStyles( fontStyles.toElement() );
  }

  kdDebug() << "Styles read in." << endl;

  return true;
}

void OoWriterImport::insertStyles( const QDomElement& element )
{
  if ( element.isNull() )
    return;

  QDomNode n = element.firstChild();

  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() || !e.hasAttribute( "style:name" ) )
    {
      n = n.nextSibling();
      continue;
    }

    QString name = e.attribute( "style:name" );
    kdDebug() << "Style: '" << name << "' loaded " << endl;
    m_styles.insert( name, new QDomElement( e ) );

    n = n.nextSibling();
  }
}


#include <oowriterimport.moc>

