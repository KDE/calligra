/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2003 David Faure <faure@kde.org>

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
#include <qregexp.h>

#include "oowriterimport.h"
#include <ooutils.h>

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

    if ( from != "application/vnd.sun.xml.writer" || to != "application/x-kword" )
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

    // Create main frameset
    QDomElement mainFramesetElement = mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("visible",1);
    framesetsElem.appendChild(mainFramesetElement);

    createInitialFrame( mainFramesetElement, 42, 566, false );

    createDocumentContent( mainDocument, mainFramesetElement );

    KoStoreDevice* out = m_chain->storageFile( "maindoc.xml", KoStore::Write );
    if ( !out ) {
        kdError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    else
    {
        QCString cstr = mainDocument.toCString();
        kdDebug()<<" maindoc: " << cstr << endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->writeBlock( cstr, cstr.length() );
    }

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

void OoWriterImport::createDocumentContent( QDomDocument &doc, QDomElement& mainFramesetElement )
{
    QDomElement content = m_content.documentElement();

    // content.xml contains some automatic-styles that we need to store
    QDomNode automaticStyles = content.namedItem( "office:automatic-styles" );
    if ( !automaticStyles.isNull() )
        insertStyles( automaticStyles.toElement() );

    QDomNode body = content.namedItem( "office:body" );
    if ( body.isNull() )
        return;

    // TODO handle sequence-decls

    for ( QDomNode text = body.firstChild(); !text.isNull(); text = text.nextSibling() )
    {
        m_styleStack.setObjectMark();
        QDomElement t = text.toElement();
        QString name = t.tagName();

        QDomElement e;
        if ( name == "text:p" ) // text paragraph
            e = parseParagraph( doc, t );
        else if ( name == "text:unordered-list" || name == "text:ordered-list" ) // listitem
            e = parseList( doc, t );
        else
        {
            kdDebug() << "Unsupported texttype '" << name << "'" << endl;
            continue;
        }

        mainFramesetElement.appendChild( e );
        m_styleStack.clearObjectMark(); // remove the styles added by the child-objects
    }
}

void OoWriterImport::prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem )
{
    mainDocument = KoDocument::createDomDocument( "kword", "DOC", "1.2" );
    QDomElement docElement = mainDocument.documentElement();
    docElement.setAttribute( "editor", "KWord's OOWriter Import Filter" );
    docElement.setAttribute( "mime", "application/x-kword" );
    docElement.setAttribute( "syntaxVersion", "2" );

    QDomElement *style = m_styles[m_masterPage.attribute( "style:page-master-name" )];
    Q_ASSERT(style);
    QDomElement properties = style->namedItem( "style:properties" ).toElement();

    QDomElement elementPaper = mainDocument.createElement("PAPER");

    bool landscape = properties.attribute("style:print-orientation") != "portrait";
    elementPaper.setAttribute("orientation", landscape ? PG_LANDSCAPE : PG_PORTRAIT );

    double width = OoUtils::toPoint(properties.attribute("fo:page-width"));
    double height = OoUtils::toPoint(properties.attribute("fo:page-height"));
    elementPaper.setAttribute("width", width);
    elementPaper.setAttribute("height", height);

    // guessFormat takes millimeters
    // ## TODO use style:num-format instead of guessing
    width = POINT_TO_MM( width );
    height = POINT_TO_MM( height );
    KoFormat paperFormat = KoPageFormat::guessFormat( landscape ? height : width, landscape ? width : height );
    elementPaper.setAttribute("format",paperFormat);

    elementPaper.setAttribute("columns",1);
    elementPaper.setAttribute("columnspacing",2);
    elementPaper.setAttribute("hType",0);
    elementPaper.setAttribute("fType",0);
    elementPaper.setAttribute("spHeadBody",9);
    elementPaper.setAttribute("spFootBody",9);
    elementPaper.setAttribute("zoom",100);
    docElement.appendChild(elementPaper);

    // ## TODO use fo:margin-{left/right/top/bottom}

    framesetsElem=mainDocument.createElement("FRAMESETS");
    docElement.appendChild(framesetsElem);
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

// Very related to OoImpressImport::openFile()
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
        kdWarning() << "This file doesn't seem to be a valid OpenWrite file" << endl;
        delete store;
        return KoFilter::WrongFormat;
    }

    QDomDocument styles;
    m_content.setContent( store->device() );
    store->close();

    //kdDebug()<<" m_content.toCString() :"<<m_content.toCString()<<endl;
    kdDebug() << "file content.xml loaded " << endl;

    if ( store->open( "styles.xml" ) )
    {
        styles.setContent( store->device() );
        store->close();

        //kdDebug()<<" styles.toCString() :"<<styles.toCString()<<endl;
        kdDebug() << "file containing styles loaded" << endl;
    }
    else
        kdWarning() << "Style definitions do not exist!" << endl;

    if ( store->open( "meta.xml" ) )
    {
        m_meta.setContent( store->device() );
        store->close();

        kdDebug() << "File containing meta definitions loaded" << endl;
    }
    else
        kdWarning() << "Meta definitions do not exist!" << endl;

    if ( store->open( "settings.xml" ) )
    {
        m_settings.setContent( store->device() );
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

// Very related to OoImpressImport::createDocumentInfo
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

    //kdDebug()<<" meta-info :"<<m_meta.toCString()<<endl;
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
      kdDebug() << "OpenWriter version: " << d << endl;
      if ( d > 1.0 )
      {
        QString message( i18n("This document was created with the OpenOffice.org version '%1'. This filter was written for version for 1.0. Reading this file could cause strange behavior, crashes or incorrect display of the data. Do you want to continue converting the document?") );
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
    kdDebug() << master.attribute( "style:name" ) << endl;
#if 0 // I don't understand this code
      QString name( "pm" );
    name += master.attribute( "style:name" );
    kdDebug() << "Master style: '" << name << "' loaded " << endl;
    m_styles.insert( name, new QDomElement( master ) );

    master = master.nextSibling().toElement();
#else
    m_masterPage = master;
#endif
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

// Perfect copy of OoImpressImport::insertStyles
void OoWriterImport::insertStyles( const QDomElement& styles )
{
    for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();

        if ( !e.hasAttribute( "style:name" ) )
            continue;

        QString name = e.attribute( "style:name" );
        m_styles.insert( name, new QDomElement( e ) );
        //kdDebug() << "Style: '" << name << "' loaded " << endl;
    }
}

void OoWriterImport::fillStyleStack( const QDomElement& object )
{
    // find all styles associated with an object and push them on the stack
    // OoImpressImport has more tests here, but I don't think they're relevant to OoWriterImport
    if ( object.hasAttribute( "text:style-name" ) )
        addStyles( m_styles[object.attribute( "text:style-name" )] );
}

void OoWriterImport::addStyles( const QDomElement* style )
{
    // this function is necessary as parent styles can have parents themself
    if ( style->hasAttribute( "style:parent-style-name" ) )
        addStyles( m_styles[style->attribute( "style:parent-style-name" )] );

    m_styleStack.push( *style );
}

// Exact copy of OoImpressImport::parseList
QDomElement OoWriterImport::parseList( QDomDocument& doc, const QDomElement& list )
{
    //kdDebug() << k_funcinfo << "parsing list"<< endl;

    bool isOrdered;
    if ( list.tagName() == "text:ordered-list" )
        isOrdered = true;
    else
        isOrdered = false;

    // take care of nested lists
    QDomElement e;
    for ( QDomNode n = list.firstChild(); !n.isNull(); n = n.firstChild() )
    {
        e = n.toElement();
        QString name = e.tagName();
        if ( name == "text:unordered-list" )
        {
            isOrdered = false;
            // parse the list-properties
            fillStyleStack( e );
        }
        else if ( name == "text:ordered-list" )
        {
            isOrdered = true;
            // parse the list-properties
            fillStyleStack( e );
        }
        if ( name == "text:p" )
            break;
    }

    QDomElement p = parseParagraph( doc, e );

    QDomElement counter = doc.createElement( "COUNTER" );
    counter.setAttribute( "numberingtype", 0 );
    counter.setAttribute( "depth", 0 );

    if ( isOrdered )
        counter.setAttribute( "type", 1 );
    else
        counter.setAttribute( "type", 10 );

    // Don't 'appendChild()'! Text elements have to be the last children of the
    // paragraph element otherwise kpresenter will cut off the last character of
    // every item!
    p.insertBefore( counter, QDomNode() );

    return p;
}

QDomElement OoWriterImport::parseParagraph( QDomDocument& doc, const QDomElement& paragraph )
{
    QDomElement p = doc.createElement( "PARAGRAPH" );

    // parse the paragraph-properties
    fillStyleStack( paragraph );

    if ( m_styleStack.hasAttribute( "fo:text-align" ) )
    {
        if ( m_styleStack.attribute( "fo:text-align" ) == "center" )
            p.setAttribute( "align", "center" );
        else if ( m_styleStack.attribute( "fo:text-align" ) == "justify" )
            p.setAttribute( "align", "justify" );
        else if ( m_styleStack.attribute( "fo:text-align" ) == "start" )
            p.setAttribute( "align", "left" );
        else if ( m_styleStack.attribute( "fo:text-align" ) == "end" )
            p.setAttribute( "align", "right" );
    }
    else
        p.setAttribute( "align","auto"); // use left aligned as default

    // parse every childnode of the paragraph
    for( QDomNode n = paragraph.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QString textData;
        QDomText t = n.toText();
        if ( t.isNull() ) // no textnode, so maybe it's a text:span
        {
            QDomElement ts = n.toElement();
            if ( ts.tagName() != "text:span" ) // TODO: are there any other possible
                continue;                      // elements or even nested test:spans?

            fillStyleStack( ts.toElement() );

            // We do a look ahead to eventually find a text:span that contains
            // only a line-break. If found, we'll add it to the current string
            // and move on to the next sibling.
            QDomNode next = n.nextSibling();
            if ( !next.isNull() )
            {
                QDomNode lineBreak = next.namedItem( "text:line-break" );
                if ( !lineBreak.isNull() ) // found a line-break
                {
                    textData = ts.text() + "\n";
                    n = n.nextSibling(); // move on to the next sibling
                }
                else
                    textData = ts.text();
            }
            else
                textData = ts.text();
        }
        else
            textData = t.data();
        if( m_styleStack.hasAttribute("fo:margin-top") ||
            m_styleStack.hasAttribute("fo:margin-bottom"))
        {
            double mtop = OoUtils::toPoint( m_styleStack.attribute( "fo:margin-top"));
            double mbottom = OoUtils::toPoint( m_styleStack.attribute("fo:margin-bottom"));
            if( mtop != 0 || mbottom!=0 )
            {
                QDomElement offset = doc.createElement( "OFFSETS" );
                if( mtop!= 0)
                    offset.setAttribute("before", mtop);
                if( mbottom!=0)
                    offset.setAttribute("after",mbottom);
                p.appendChild( offset );
            }
        }
        // take care of indentation
        if ( m_styleStack.hasAttribute( "fo:margin-left" ) ||
            m_styleStack.hasAttribute( "fo:margin-right" ) ||
            m_styleStack.hasAttribute( "fo:text-indent"))
        {
            double marginLeft = OoUtils::toPoint( m_styleStack.attribute( "fo:margin-left" ) );
            double marginRight = OoUtils::toPoint( m_styleStack.attribute( "fo:margin-right" ) );
            double first = OoUtils::toPoint( m_styleStack.attribute("fo:text-indent"));
            if ( (marginLeft!= 0) || marginRight!=0 || first!=0)
            {
                QDomElement indent = doc.createElement( "INDENTS" );
                if( marginLeft != 0)
                    indent.setAttribute( "left", marginLeft );
                if( marginRight != 0 )
                    indent.setAttribute( "right", marginLeft );
                if( first != 0)
                    indent.setAttribute( "first", first);
                p.appendChild( indent );
            }
        }
        if( m_styleStack.hasAttribute("fo:line-height"))
        {
            QString value = m_styleStack.attribute( "fo:line-height" );
            QDomElement lineSpacing = doc.createElement( "LINESPACING" );
            if( value=="150%")
            {
                lineSpacing.setAttribute("type","oneandhalf");
            }
            else if( value=="200%")
            {
                lineSpacing.setAttribute("type","double");
            }
            p.appendChild(lineSpacing );
        }
        QDomElement text = doc.createElement( "TEXT" );
        text.appendChild( doc.createTextNode( textData ) );

        //kdDebug() << k_funcinfo << "Para text is: " << paragraph.text() << endl;

        // parse the text-properties
        if ( m_styleStack.hasAttribute( "fo:color" ) )
            text.setAttribute( "color", m_styleStack.attribute( "fo:color" ) );
        if ( m_styleStack.hasAttribute( "fo:font-family" ) )
        {
            // 'Thorndale' is not known outside OpenOffice so we substitute it
            // with 'Times New Roman' that looks nearly the same.
            if ( m_styleStack.attribute( "fo:font-family" ) == "Thorndale" )
                text.setAttribute( "family", "Times New Roman" );
            else
                text.setAttribute( "family", m_styleStack.attribute( "fo:font-family" ).remove( "'" ) );
        }
        if ( m_styleStack.hasAttribute( "fo:font-size" ) )
            text.setAttribute( "pointSize", OoUtils::toPoint( m_styleStack.attribute( "fo:font-size" ) ) );
        if ( m_styleStack.hasAttribute( "fo:font-weight" ) )
            if ( m_styleStack.attribute( "fo:font-weight" ) == "bold" )
                text.setAttribute( "bold", 1 );

        if ( m_styleStack.hasAttribute( "fo:font-style" ) )
            if ( m_styleStack.attribute( "fo:font-style" ) == "italic" )
            {
                QDomElement italic = doc.createElement( "ITALIC" );
                italic.setAttribute( "value", 1 );

            }
        if( m_styleStack.hasAttribute("style:text-crossing-out" ))
        {
            QString strikeOutType = m_styleStack.attribute( "style:text-crossing-out" );
            QDomElement strikeOut = doc.createElement( "STRIKEOUT" );
            if( strikeOutType =="double-line")
            {
                //text.setAttribute("strikeOut", "double");
                //text.setAttribute("strikeoutstyleline","solid");
            }
            else if( strikeOutType =="single-line")
            {
                //text.setAttribute("strikeOut", "single");
                //text.setAttribute("strikeoutstyleline","solid");
            }
            else if( strikeOutType =="thick-line")
            {
                //text.setAttribute("strikeOut", "single-bold");
                //text.setAttribute("strikeoutstyleline","solid");
            }

        }
        if( m_styleStack.hasAttribute("style:text-position"))
        {
            QString textPos =m_styleStack.attribute("style:text-position");
            //relativetextsize="0.58"
            //"super 58%"
            if( textPos.contains("super"))
            {
                textPos=textPos.remove("super");
                textPos=textPos.remove("%");
                double value = textPos.stripWhiteSpace().toDouble();
                //text.setAttribute("VERTALIGN",2);
                //text.setAttribute("relativetextsize", value/100 );
            }
            else if(textPos.contains("sub"))
            {
                textPos=textPos.remove("sub");
                textPos=textPos.remove("%");
                double value = textPos.stripWhiteSpace().toDouble();
                //text.setAttribute("VERTALIGN",1);
                //text.setAttribute("relativetextsize", value/100 );
            }
        }
        if ( m_styleStack.hasAttribute( "style:text-underline" ) )
        {
            QString underType = m_styleStack.attribute( "style:text-underline" );
            QString underLineColor = m_styleStack.attribute( "style:text-underline-color" );
            QDomElement strikeOut = doc.createElement( "UNDERLINE" );
            if ( underType == "single" )
            {
                //text.setAttribute( "underline", 1 );
                ///text.setAttribute( "underlinestyleline", "solid" );  //lukas: TODO support more underline styles
                //text.setAttribute("underlinecolor", underLineColor);
            }
            else if(underType =="double")
            {
                //text.setAttribute( "underline", "double" );
                //text.setAttribute( "underlinestyleline", "solid" );  //lukas: TODO support more underline styles
                //text.setAttribute("underlinecolor", underLineColor);
            }
            else if( underType == "bold" )
            {
                //text.setAttribute("underline","single-bold");
                //text.setAttribute("underlinestyleline","solid");
                //text.setAttribute("underlinecolor", underLineColor);
            }
            else if( underType =="wave")
            {
                //not implemented into kpresenter
                //text.setAttribute("underline","wave");
                //text.setAttribute("underlinestyleline","solid");
                //text.setAttribute("underlinecolor", underLineColor);
            }
        }

        //appendShadow( doc, p ); // this is necessary to take care of shadowed paragraphs
        m_styleStack.clearObjectMark(); // remove possible test:span styles from the stack
        p.appendChild( text );
    }

    return p;
}

#include "oowriterimport.moc"
