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

#include "ooimpressimport.h"

#include <kdebug.h>
#include <koUnit.h>
#include <koDocumentInfo.h>
#include <koDocument.h>

#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koGlobal.h>

typedef KGenericFactory<OoImpressImport, KoFilter> OoImpressImportFactory;
K_EXPORT_COMPONENT_FACTORY( libooimpressimport, OoImpressImportFactory( "ooimpressimport" ) );


OoImpressImport::OoImpressImport( KoFilter *, const char *, const QStringList & )
  : KoFilter()
{
}

OoImpressImport::~OoImpressImport()
{
}

KoFilter::ConversionStatus OoImpressImport::convert( QCString const & from, QCString const & to )
{
    kdDebug() << "Entering Ooimpress Import filter: " << from << " - " << to << endl;

    if ( from != "application/x-ooimpress" || to != "application/x-kpresenter" )
    {
        kdWarning() << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    KoFilter::ConversionStatus preStatus = openFile();

    if ( preStatus != KoFilter::OK )
        return preStatus;

    QDomDocument docinfo;
    createDocumentInfo( docinfo );

    // store document info
    KoStoreDevice* out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if( out )
    {
        QCString info = docinfo.toCString();
        //kdDebug() << " info :" << info << endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->writeBlock( info , info.length() );
    }

    QDomDocument doccontent;
    createDocumentContent( doccontent );

    // store document content
    out = m_chain->storageFile( "maindoc.xml", KoStore::Write );
    if( out )
    {
        QCString content = doccontent.toCString();
        //kdDebug() << " content :" << content << endl;
        out->writeBlock( content , content.length() );
    }

    kdDebug() << "######################## OoImpressImport::convert done ####################" << endl;
    return KoFilter::OK;
}

KoFilter::ConversionStatus OoImpressImport::openFile()
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
    kdWarning() << "This file doesn't seem to be a valid OoImpress file" << endl;
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
  //kdDebug() << "m_content.toCString() :" << m_content.toCString() << endl;
  kdDebug() << "File content.xml loaded " << endl;

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
    //kdDebug() << "styles.toCString() :" << styles.toCString() << endl;
    kdDebug() << "File containing styles loaded" << endl;
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

void OoImpressImport::createDocumentInfo( QDomDocument &docinfo )
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

    //kdDebug() << " meta-info :" << m_meta.toCString() << endl;
}

void OoImpressImport::createDocumentContent( QDomDocument &doccontent )
{
    QDomDocument doc = KoDocument::createDomDocument( "kpresenter", "DOC", "1.2" );
    QDomElement docElement = doc.documentElement();
    docElement.setAttribute( "editor", "KPresenter" );
    docElement.setAttribute( "mime", "application/x-kpresenter" );
    docElement.setAttribute( "syntaxVersion", "2" );

    QDomElement content = m_content.documentElement();
    QDomNode body = content.namedItem( "office:body" );

    if ( body.isNull() )
        return;

    // at the moment use some default settings for paper
    QDomElement paperElement = doc.createElement( "PAPER" );
    paperElement.setAttribute( "ptWidth", CM_TO_POINT(28)/*680*/ );
    paperElement.setAttribute( "ptHeight", CM_TO_POINT(21)/*510*/ );
    paperElement.setAttribute( "unit", 0 );
    paperElement.setAttribute( "format", 5 );
    paperElement.setAttribute( "tabStopValue", 42.5198 );
    paperElement.setAttribute( "orientation", 0 );
    double pageHeight = 21;

    QDomElement paperBorderElement = doc.createElement( "PAPERBORDERS" );
    paperBorderElement.setAttribute( "ptRight", 0 );
    paperBorderElement.setAttribute( "ptBottom", 0 );
    paperBorderElement.setAttribute( "ptLeft", 0 );
    paperBorderElement.setAttribute( "ptTop", 0 );
    paperElement.appendChild( paperBorderElement );

    QDomElement objectElement = doc.createElement( "OBJECTS" );
    QDomElement pageTitleElement = doc.createElement( "PAGETITLES" );

    // parse all pages
    for ( QDomNode drawPage = body.firstChild(); !drawPage.isNull(); drawPage = drawPage.nextSibling() )
    {
        QDomElement dp = drawPage.toElement();

        // set the pagetitle
        QDomElement titleElement = doc.createElement( "Title" );
        titleElement.setAttribute( "title", dp.attribute( "draw:name" ) );
        pageTitleElement.appendChild( titleElement );

        double offset = ( dp.attribute( "draw:id" ).toInt() - 1 ) * pageHeight;

        // parse all objects
        for ( QDomNode object = drawPage.firstChild(); !object.isNull(); object = object.nextSibling() )
        {
            QDomElement o = object.toElement();
            QString name = o.tagName();

            QDomElement e;
            if ( name == "draw:text-box" ) // textbox
            {
                e = parseObject( doc, o, offset );
                e.setAttribute( "type", 4 );
                e.appendChild( parseTextBox( doc, o ) );
            }
            else if ( name == "draw:rect" ) // rectangle
            {
                e = parseObject( doc, o, offset );
                e.setAttribute( "type", 2 );
            }
            else if ( name == "draw:circle" || name == "draw:ellipse" ) // circle or ellipse
            {
                e = parseObject( doc, o, offset );
                e.setAttribute( "type", 3 );
            }
            else if ( name == "draw:line" ) // line
            {
                e = parseLineObject( doc, o, offset );
            }
            else
            {
                kdDebug() << "Unsupported object '" << name << "'" << endl;
                continue;
            }

            objectElement.appendChild( e );
        }
    }

    docElement.appendChild( paperElement );
    docElement.appendChild( pageTitleElement );
    docElement.appendChild( objectElement );
    doccontent.appendChild( doc );
}

QDomElement OoImpressImport::parseObject( QDomDocument& doc, const QDomElement& object, int offset )
{
    // get origin, size, pen and brush of the object
    // this code is identical for all objects
    QDomElement objectElement = doc.createElement( "OBJECT" );

    QDomElement orig = doc.createElement( "ORIG" );
    orig.setAttribute( "x", CM_TO_POINT(object.attribute("svg:x").toDouble()) );
    orig.setAttribute( "y", CM_TO_POINT(object.attribute("svg:y").toDouble() + offset) );
    objectElement.appendChild( orig );

    QDomElement size = doc.createElement( "SIZE" );
    size.setAttribute( "width", CM_TO_POINT(object.attribute("svg:width").toDouble()) );
    size.setAttribute( "height", CM_TO_POINT(object.attribute("svg:height").toDouble()) );
    objectElement.appendChild( size );

    QDomElement pen = doc.createElement( "PEN" );
    objectElement.appendChild( pen );

    QDomElement brush = doc.createElement( "BRUSH" );
    objectElement.appendChild( brush );

    return objectElement;
}

QDomElement OoImpressImport::parseLineObject( QDomDocument& doc, const QDomElement& object, int offset )
{
    double x1 = object.attribute("svg:x1").toDouble();
    double y1 = object.attribute("svg:y1").toDouble();
    double x2 = object.attribute("svg:x2").toDouble();
    double y2 = object.attribute("svg:y2").toDouble();

    double x = QMIN(x1, x2);
    double y = QMIN(y1, y2);

    QDomElement objectElement = doc.createElement( "OBJECT" );

    QDomElement orig = doc.createElement( "ORIG" );
    orig.setAttribute( "x", CM_TO_POINT( x ) );
    orig.setAttribute( "y", CM_TO_POINT( y + offset) );
    objectElement.appendChild( orig );

    QDomElement size = doc.createElement( "SIZE" );
    size.setAttribute( "width", CM_TO_POINT( fabs( x1 - x2 ) ) );
    size.setAttribute( "height", CM_TO_POINT( fabs( y1 - y2 ) ) );
    objectElement.appendChild( size );

    QDomElement linetype = doc.createElement( "LINETYPE" );
    if ( ( x1 < x2 && y1 < y2 ) || ( x1 > x2 && y1 > y2 ) )
        linetype.setAttribute( "value", 2 );
    else
        linetype.setAttribute( "value", 3 );

    objectElement.appendChild( linetype );

    QDomElement pen = doc.createElement( "PEN" );
    objectElement.appendChild( pen );

    QDomElement brush = doc.createElement( "BRUSH" );
    objectElement.appendChild( brush );
    objectElement.setAttribute( "type", 1 );

    return objectElement;
}

QDomElement OoImpressImport::parseTextBox( QDomDocument& doc, const QDomElement& textBox )
{
    QDomElement textObjectElement = doc.createElement( "TEXTOBJ" );

    for ( QDomNode text = textBox.firstChild(); !text.isNull(); text = text.nextSibling() )
    {
        QDomElement t = text.toElement();
        QString name = t.tagName();

        QDomElement e;
        if ( name == "text:p" ) // text paragraph
            e = parseParagraph( doc, t );
        else if ( name == "text:unordered-list" || name == "text:ordered-list" ) // listitem
        {
            e = parseList( doc, t );
        }
        else
        {
            kdDebug() << "Unsupported texttype '" << name << "'" << endl;
            continue;
        }

        textObjectElement.appendChild( e );

    }

    return textObjectElement;
}

QDomElement OoImpressImport::parseList( QDomDocument& doc, const QDomElement& list )
{
    QDomNode item = list.namedItem( "text:list-item" );
    QDomElement paragraph = item.namedItem( "text:p" ).toElement();

    QDomElement p = parseParagraph( doc, paragraph );
    QDomElement counter = doc.createElement( "COUNTER" ); // TODO when styles are done
    counter.setAttribute( "numberingtype", 0 );
    counter.setAttribute( "type", 10 );
    counter.setAttribute( "depth", 0 );
    p.appendChild( counter );

    return p;
}

QDomElement OoImpressImport::parseParagraph( QDomDocument& doc, const QDomElement& paragraph )
{
    QDomElement p = doc.createElement( "P" );

    QDomElement name = doc.createElement( "NAME" );
    name.setAttribute( "value", paragraph.attribute( "text:style-name" ) );
    p.appendChild( name );

    QDomElement text = doc.createElement( "TEXT" );
    text.appendChild( doc.createTextNode( paragraph.text() ) );
    p.appendChild( text );

    return p;
}

#include <ooimpressimport.moc>

