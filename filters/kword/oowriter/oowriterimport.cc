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
#include "conversion.h"

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
    kdDebug(30518) << "Entering Oowriter Import filter: " << from << " - " << to << endl;

    if ( from != "application/vnd.sun.xml.writer" || to != "application/x-kword" )
    {
        kdWarning(30518) << "Invalid mimetypes " << from << " " << to << endl;
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
    createStyles( mainDocument );
    createDocumentContent( mainDocument, mainFramesetElement );

    KoStoreDevice* out = m_chain->storageFile( "maindoc.xml", KoStore::Write );
    if ( !out ) {
        kdError(30518) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    else
    {
        QCString cstr = mainDocument.toCString();
        kdDebug(30518)<<" maindoc: " << cstr << endl;
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
        kdDebug(30518)<<" info :"<<info<<endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->writeBlock( info , info.length() );
    }


    kdDebug(30518) << "######################## OoWriterImport::convert done ####################" << endl;
    return KoFilter::OK;
}

void OoWriterImport::createStyles( QDomDocument& doc )
{
    QDomElement stylesElem = doc.createElement( "STYLES" );
    doc.documentElement().appendChild( stylesElem );

    QDomNode fixedStyles = m_stylesDoc.documentElement().namedItem( "office:styles" );
    Q_ASSERT( !fixedStyles.isNull() );
    kdDebug(30518) << "Generating " << fixedStyles.childNodes().count() << " kword styles." << endl;
    for ( QDomNode n = fixedStyles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        if ( !e.hasAttribute( "style:name" ) )
            continue;

        // We use the style stack, to flatten out parent styles
        // Once KWord supports style inheritance, replace this with a single m_styleStack.push.
        // (We still need to use StyleStack, since that's what writeLayout/writeFormat read from)
        addStyles( &e );

        QDomElement styleElem = doc.createElement("STYLE");
        stylesElem.appendChild( styleElem );

        QDomElement element = doc.createElement("NAME");
        element.setAttribute( "value", e.attribute( "style:name" ) );
        styleElem.appendChild( element );

        QString followingStyle = m_styleStack.attribute( "style:next-style-name" );
        if ( !followingStyle.isEmpty() )
        {
            QDomElement element = doc.createElement( "FOLLOWING" );
            element.setAttribute( "name", followingStyle );
            styleElem.appendChild( element );
        }

        writeFormat( doc, styleElem, 1, 0, 0 );
        writeLayout( doc, styleElem );

        m_styleStack.clear();
    }
}

void OoWriterImport::createDocumentContent( QDomDocument &doc, QDomElement& mainFramesetElement )
{
    QDomElement content = m_content.documentElement();

    QDomNode body = content.namedItem( "office:body" );
    if ( body.isNull() )
        return;

    for ( QDomNode text = body.firstChild(); !text.isNull(); text = text.nextSibling() )
    {
        m_styleStack.setMark( StyleStack::ObjectMark );
        QDomElement t = text.toElement();
        QString name = t.tagName();

        QDomElement e;
        if ( name == "text:p" ) // text paragraph
            e = parseParagraph( doc, t );
        else if ( name == "text:h" ) // heading
        {
            e = parseParagraph( doc, t );
            // TODO parse text:level, the level of the heading
        }
        else if ( name == "text:unordered-list" || name == "text:ordered-list" ) // listitem
            e = parseList( doc, t );
        // TODO text:sequence-decls
        else
        {
            kdDebug(30518) << "Unsupported texttype '" << name << "'" << endl;
            continue;
        }

        mainFramesetElement.appendChild( e );
        m_styleStack.popToMark( StyleStack::ObjectMark ); // remove the styles added by the child-objects
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

    double width = KoUnit::parseValue(properties.attribute("fo:page-width"));
    double height = KoUnit::parseValue(properties.attribute("fo:page-height"));
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

    // Page margins
    QDomElement element = mainDocument.createElement("PAPERBORDERS");
    element.setAttribute("left", KoUnit::parseValue(properties.attribute("fo:margin-left")));
    element.setAttribute("top", KoUnit::parseValue(properties.attribute("fo:margin-top")));
    element.setAttribute("right", KoUnit::parseValue(properties.attribute("fo:margin-right")));
    element.setAttribute("bottom", KoUnit::parseValue(properties.attribute("fo:margin-bottom")));
    elementPaper.appendChild(element);

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

    kdDebug(30518) << "Store created" << endl;

    if ( !store )
    {
        kdWarning(30518) << "Couldn't open the requested file." << endl;
        return KoFilter::FileNotFound;
    }

    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;

    kdDebug(30518) << "Trying to open content.xml" << endl;
    if ( !store->open( "content.xml" ) )
    {
        kdWarning(30518) << "This file doesn't seem to be a valid OpenWriter file" << endl;
        delete store;
        return KoFilter::WrongFormat;
    }
    
    
    if ( !m_content.setContent( store->device(),
        &errorMsg, &errorLine, &errorColumn ) )
    {
	kdError(30518) << "Parsing error in content.xml" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
	store->close();
	delete store;
	return KoFilter::ParsingError;
    }
    store->close();

    //kdDebug(30518)<<" m_content.toCString() :"<<m_content.toCString()<<endl;
    kdDebug(30518) << "file content.xml loaded " << endl;

    // We need to keep the QDomDocument for styles too, unfortunately.
    // Otherwise styleElement.parentNode() returns a null node
    // (see StyleStack::isUserStyle). Most of styles.xml is in m_styles
    // anyway, so this doesn't make a big difference.
    // We now also rely on this in createStyles.
    //QDomDocument styles;
    if ( store->open( "styles.xml" ) )
    {
        if ( !m_stylesDoc.setContent( store->device(),
             &errorMsg, &errorLine, &errorColumn ) )
	{
	    kdWarning(30518) << "Parsing error in style.xml" << endl
		<< " In line: " << errorLine << ", column: " << errorColumn << endl
		<< " Error message: " << errorMsg << endl;
	}
        else
            kdDebug(30518) << "file containing styles loaded" << endl;
        store->close();

        //kdDebug(30518)<<" styles.toCString() :"<<m_stylesDoc.toCString()<<endl;
    }
    else
        kdWarning(30518) << "Style definitions do not exist!" << endl;

    if ( store->open( "meta.xml" ) )
    {
        if ( !m_meta.setContent( store->device(),
             &errorMsg, &errorLine, &errorColumn ) )
	{
	    kdWarning(30518) << "Parsing error in meta.xml" << endl
		<< " In line: " << errorLine << ", column: " << errorColumn << endl
		<< " Error message: " << errorMsg << endl;
	}
        else
            kdDebug(30518) << "File containing meta definitions loaded" << endl;
        
        store->close();
    }
    else
        kdWarning(30518) << "Meta definitions do not exist!" << endl;

    if ( store->open( "settings.xml" ) )
    {
        if ( !m_settings.setContent( store->device(),
             &errorMsg, &errorLine, &errorColumn ) )
	{
	    kdWarning(30518) << "Parsing error in settings.xml" << endl
		<< " In line: " << errorLine << ", column: " << errorColumn << endl
		<< " Error message: " << errorMsg << endl;
	}
        else
            kdDebug(30518) << "File containing settings loaded" << endl;

        store->close();
    }
    else
        kdWarning(30518) << "Settings do not exist!" << endl;

    delete store;

    emit sigProgress( 10 );

    // Load styles from style.xml
    if ( !createStyleMap( m_stylesDoc ) )
        return KoFilter::UserCancelled;
    // Also load styles from content.xml
    if ( !createStyleMap( m_content ) )
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

    //kdDebug(30518)<<" meta-info :"<<m_meta.toCString()<<endl;
}

bool OoWriterImport::createStyleMap( const QDomDocument & styles )
{
  QDomElement docElement  = styles.documentElement();
  QDomNode docStyles   = docElement.namedItem( "office:document-styles" );

  if ( docElement.hasAttribute( "office:version" ) )
  {
    bool ok = true;
    double d = docElement.attribute( "office:version" ).toDouble( &ok );

    if ( ok )
    {
      kdDebug(30518) << "OpenWriter version: " << d << endl;
      if ( d > 1.0 )
      {
        QString message( i18n("This document was created with the OpenOffice.org version '%1'. This filter was written for version for 1.0. Reading this file could cause strange behavior, crashes or incorrect display of the data. Do you want to continue converting the document?") );
        message.arg( docElement.attribute( "office:version" ) );
        if ( KMessageBox::warningYesNo( 0, message, i18n( "Unsupported document version" ) ) == KMessageBox::No )
          return false;
      }
    }
  }

  QDomNode fontStyles = docElement.namedItem( "office:font-decls" );

  if ( !fontStyles.isNull() )
  {
    kdDebug(30518) << "Starting reading in font-decl..." << endl;

    insertStyles( fontStyles.toElement() );
  }
  else
    kdDebug(30518) << "No items found" << endl;

  kdDebug(30518) << "Starting reading in auto:styles" << endl;

  QDomNode autoStyles = docElement.namedItem( "office:automatic-styles" );
  if ( !autoStyles.isNull() )
  {
      insertStyles( autoStyles.toElement() );
  }
  else
    kdDebug(30518) << "No items found" << endl;


  kdDebug(30518) << "Reading in master styles" << endl;

  QDomNode masterStyles = docElement.namedItem( "office:master-styles" );

  if ( masterStyles.isNull() )
  {
    kdDebug(30518) << "Nothing found " << endl;
  }

  QDomElement master = masterStyles.namedItem( "style:master-page" ).toElement();
  if ( !master.isNull() )
  {
    kdDebug(30518) << master.attribute( "style:name" ) << endl;
    QString name( "pm" );
    name += master.attribute( "style:name" );
    kdDebug(30518) << "Master style: '" << name << "' loaded " << endl;
    m_styles.insert( name, new QDomElement( master ) );

    master = master.nextSibling().toElement();
    m_masterPage = master;
  }


  kdDebug(30518) << "Starting reading in office:styles" << endl;

  QDomNode fixedStyles = docElement.namedItem( "office:styles" );

  kdDebug(30518) << "Reading in default styles" << endl;

  QDomNode def = fixedStyles.namedItem( "style:default-style" );
  while ( !def.isNull() )
  {
    QDomElement e = def.toElement();
    kdDebug(30518) << "Style found " << e.nodeName() << ", tag: " << e.tagName() << endl;

    if ( e.nodeName() != "style:default-style" )
    {
      def = def.nextSibling();
      continue;
    }

    if ( !e.isNull() )
    {
      kdDebug(30518) << "Default style " << e.attribute( "style:family" ) << "default" << " loaded " << endl;

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

  kdDebug(30518) << "Styles read in." << endl;

  return true;
}

// Perfect copy of OoImpressImport::insertStyles
void OoWriterImport::insertStyles( const QDomElement& styles )
{
    kdDebug(30518) << "Inserting styles from " << styles.tagName() << endl;
    for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();

        if ( !e.hasAttribute( "style:name" ) )
            continue;

        QString name = e.attribute( "style:name" );
        QDomElement* ep = new QDomElement( e );
        m_styles.insert( name, ep );

        kdDebug(30518) << "Style: '" << name << "' loaded " << endl;
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
    //kdDebug(30518) << k_funcinfo << "parsing list"<< endl;

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
    m_styleStack.setMark( StyleStack::ParagraphMark );

    QDomElement formats = doc.createElement( "FORMATS" );

    QString paragraphText;
    uint pos = 0;

    // parse every childnode of the paragraph
    for( QDomNode n = paragraph.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QString textData;
        QDomElement ts = n.toElement();
        QString tagName = ts.tagName();
        QDomText t = n.toText();

        if (tagName == "text:s")
            textData = OoUtils::expandWhitespace(n.toElement());
        else if (tagName == "text:date" // fields
                 || tagName == "text:time"
                 || tagName == "text:page-number"
                 || tagName == "text:file-name"
                 || tagName == "text:author-name"
                 || tagName == "text:author-initials")
        {
            textData = '#';     // field placeholder
            appendField(doc, p, ts, pos);
        }
        else if ( tagName == "text:tab-stop" )
        {
            // KWord currently uses \t.
            // Known bug: a line with only \t\t\t\t isn't loaded - XML (QDom) strips out whitespace.
            // One more good reason to switch to <text:tab-stop> instead...
            textData = '\t';
        }
        else if ( t.isNull() ) // no textnode, so maybe it's a text:span
        {
            if ( tagName != "text:span" )
            {
                // TODO: are there any other possible
                // elements or even nested test:spans?
                // DF: yes. Footnotes, for instance, and many fields.
                kdWarning() << "Ignoring " << ts.tagName() << endl;
                continue;
            }
            fillStyleStack( ts );

            // We do a look ahead to eventually find a text:span that contains
            // only a line-break. If found, we'll add it to the current string
            // and move on to the next sibling.
            // DF: What proves that the line-break will be at the _beginning_ of the next item?
            // (What if it's a footnote?). Why not treat line-break _when_ it occurs?
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

        paragraphText += textData;
        uint length = textData.length();

        writeFormat( doc, formats, 1 /* id for normal text */, pos, length );

        //appendShadow( doc, p ); // this is necessary to take care of shadowed paragraphs
        pos += length;

        m_styleStack.popToMark( StyleStack::ParagraphMark  ); // remove possible text:span styles from the stack
    }

    QDomElement text = doc.createElement( "TEXT" );
    text.appendChild( doc.createTextNode( paragraphText ) );
    text.setAttribute( "xml:space", "preserve" );
    p.appendChild( text );
    //kdDebug(30518) << k_funcinfo << "Para text is: " << paragraphText << endl;

    p.appendChild( formats );
    QDomElement layoutElement = doc.createElement( "LAYOUT" );
    p.appendChild( layoutElement );

    // TODO style name (when specified by user)
    // Style name
    QString styleName = m_styleStack.userStyleName();
    if ( !styleName.isEmpty() )
    {
        QDomElement nameElement = doc.createElement("NAME");
        nameElement.setAttribute("value", styleName);
        layoutElement.appendChild(nameElement);
    }

    writeLayout( doc, layoutElement );

    return p;
}

void OoWriterImport::writeFormat( QDomDocument& doc, QDomElement& formats, int id, int pos, int length )
{
    // Prepare a FORMAT element for this run of text
    QDomElement format( doc.createElement( "FORMAT" ) );
    format.setAttribute( "id", id );
    format.setAttribute( "pos", pos );
    format.setAttribute( "len", length );

    // parse the text-properties
    // TODO compare with the paragraph style, and only write out if != from style.
    // (This is very important, it's not only an optimization. If no attribute is
    // specified in the .kwd file, the style's property will be used, which might
    // not always be correct).
    if ( m_styleStack.hasAttribute( "fo:color" ) ) { // 3.10.3
        QColor color( m_styleStack.attribute( "fo:color" ) ); // #rrggbb format
        QDomElement colorElem( doc.createElement( "COLOR" ) );
        colorElem.setAttribute( "red", color.red() );
        colorElem.setAttribute( "blue", color.blue() );
        colorElem.setAttribute( "green", color.green() );
        format.appendChild( colorElem );
    }
    if ( m_styleStack.hasAttribute( "fo:font-family" )  // 3.10.9
         || m_styleStack.hasAttribute("style:font-name") ) // 3.10.8
    {
        // Hmm, the remove "'" could break it's in the middle of the fontname...
        QString fontName = m_styleStack.attribute( "fo:font-family" ).remove( "'" );
        if (fontName.isEmpty())
            fontName = m_styleStack.attribute( "style:font-name" ).remove( "'" );
        // 'Thorndale' is not known outside OpenOffice so we substitute it
        // with 'Times New Roman' that looks nearly the same.
        if ( fontName == "Thorndale" )
            fontName = "Times New Roman";

        QDomElement fontElem( doc.createElement( "FONT" ) );
        fontElem.setAttribute( "name", fontName );
        format.appendChild( fontElem );
    }
    if ( m_styleStack.hasAttribute( "fo:font-size" ) ) { // 3.10.14
        double pointSize = m_styleStack.fontSize();

        QDomElement fontSize( doc.createElement( "SIZE" ) );
        fontSize.setAttribute( "value", qRound(pointSize) ); // KWord uses toInt()!
        format.appendChild( fontSize );
    }
    if ( m_styleStack.hasAttribute( "fo:font-weight" ) ) { // 3.10.24
        QDomElement weightElem( doc.createElement( "WEIGHT" ) );
        QString fontWeight = m_styleStack.attribute( "fo:font-weight" );
        int boldness = fontWeight.toInt();
        if ( fontWeight == "bold" )
            boldness = 75;
        else if ( boldness == 0 )
            boldness = 50;
        weightElem.setAttribute( "value", boldness );
        format.appendChild( weightElem );
    }

    if ( m_styleStack.hasAttribute( "fo:font-style" ) ) // 3.10.19
        if ( m_styleStack.attribute( "fo:font-style" ) == "italic" ||
             m_styleStack.attribute( "fo:font-style" ) == "oblique" ) // no difference in kotext
        {
            QDomElement italic = doc.createElement( "ITALIC" );
            italic.setAttribute( "value", 1 );
            format.appendChild( italic );
        }

    bool wordByWord = (m_styleStack.hasAttribute("fo:score-spaces")) // 3.10.25
                      && (m_styleStack.attribute("fo:score-spaces") == "false");
    if( m_styleStack.hasAttribute("style:text-crossing-out" )) // 3.10.6
    {
        QString strikeOutType = m_styleStack.attribute( "style:text-crossing-out" );
        QDomElement strikeOut = doc.createElement( "STRIKEOUT" );
        if( strikeOutType =="double-line")
        {
            strikeOut.setAttribute("value", "double");
            strikeOut.setAttribute("styleline","solid");
        }
        else if( strikeOutType =="single-line")
        {
            strikeOut.setAttribute("value", "single");
            strikeOut.setAttribute("styleline","solid");
        }
        else if( strikeOutType =="thick-line")
        {
            strikeOut.setAttribute("value", "single-bold");
            strikeOut.setAttribute("styleline","solid");
        }
        if ( wordByWord )
            strikeOut.setAttribute("wordbyword", 1);
        // not supported by KWord: "slash" and "X"
        // not supported by OO: stylelines (solid, dash, dot, dashdot, dashdotdot)
        format.appendChild( strikeOut );
    }
    if( m_styleStack.hasAttribute("style:text-position")) // 3.10.7
    {
        QDomElement vertAlign = doc.createElement( "VERTALIGN" );
        QString textPos =m_styleStack.attribute("style:text-position");
        //relativetextsize="0.58"
        //"super 58%"
        if( textPos.contains("super"))
        {
            textPos=textPos.remove("super");
            textPos=textPos.remove("%");
            double value = textPos.stripWhiteSpace().toDouble();
            vertAlign.setAttribute( "value", 2 );
            vertAlign.setAttribute( "relativetextsize", value/100 );
        }
        else if(textPos.contains("sub"))
        {
            textPos=textPos.remove("sub");
            textPos=textPos.remove("%");
            double value = textPos.stripWhiteSpace().toDouble();
            vertAlign.setAttribute( "value", 1 );
            vertAlign.setAttribute( "relativetextsize", value/100 );
        }
        format.appendChild( vertAlign );
    }
    if ( m_styleStack.hasAttribute( "style:text-underline" ) ) // 3.10.22
    {
        QString underline;
        QString styleline;
        OoUtils::importUnderline( m_styleStack.attribute( "style:text-underline" ),
                                  underline, styleline );
        QDomElement underLineElem = doc.createElement( "UNDERLINE" );
        underLineElem.setAttribute( "value", underline );
        underLineElem.setAttribute( "styleline", styleline );

        QString underLineColor = m_styleStack.attribute( "style:text-underline-color" ); // 3.10.23
        if ( underLineColor != "font-color" )
            underLineElem.setAttribute("underlinecolor", underLineColor);
        if ( wordByWord )
            underLineElem.setAttribute("wordbyword", 1);
        format.appendChild( underLineElem );
    }
    // Small caps, lowercase, uppercase
    if ( m_styleStack.hasAttribute( "fo:font-variant" ) // 3.10.1
         || m_styleStack.hasAttribute( "fo:text-transform" ) ) // 3.10.2
    {
        QDomElement fontAttrib( doc.createElement( "FONTATTRIBUTE" ) );
        bool smallCaps = m_styleStack.attribute( "fo:font-variant" ) == "small-caps";
        if ( smallCaps )
        {
            fontAttrib.setAttribute( "value", "smallcaps" );
        } else
        {
            // Both KWord and OO use "uppercase" and "lowercase".
            // TODO in KWord: "capitalize".
            fontAttrib.setAttribute( "value", m_styleStack.attribute( "fo:text-transform" ) );
        }
        format.appendChild( fontAttrib );
    }

    if (m_styleStack.hasAttribute("fo:language")) // 3.10.17
    {
        QDomElement lang = doc.createElement("LANGUAGE");
        QString tmp = m_styleStack.attribute("fo:language");
        if (tmp=="en")
            lang.setAttribute("value", "en_US");
        else
            lang.setAttribute("value", tmp);
        format.appendChild(lang);
    }

    if (m_styleStack.hasAttribute("style:text-background-color")) // 3.10.28
    {
        QDomElement bgCol = doc.createElement("TEXTBACKGROUNDCOLOR");
        QColor tmp = m_styleStack.attribute("style:text-background-color");
        if (tmp != "transparent")
        {
            bgCol.setAttribute("red", tmp.red());
            bgCol.setAttribute("green", tmp.green());
            bgCol.setAttribute("blue", tmp.blue());
            format.appendChild(bgCol);
        }
    }

    /*
      Missing properties:
      style:use-window-font-color, 3.10.4 - what is it?
      style:text-outline, 3.10.5 - not implemented in kotext
      style:font-family-generic, 3.10.10 - roman, swiss, modern -> map to a font?
      style:font-style-name, 3.10.11 - ?
      style:font-pitch, 3.10.12 - fixed or variable -> map to a font?
      style:font-charset, 3.10.14 - not necessary with Qt
      style:font-size-rel, 3.10.15 - TODO in StyleStack::fontSize()
      fo:letter-spacing, 3.10.16 - not implemented in kotext
      style:text-relief, 3.10.20 - not implemented in kotext
      style:text-shadow, 3.10.21 - TODO
      style:letter-kerning, 3.10.20 - not implemented in kotext
      style:text-blinking, 3.10.27 - not implemented in kotext IIRC
      style:text-combine, 3.10.29/30 - what is it?
      style:text-emphasis, 3.10.31 - not implemented in kotext
      style:text-autospace, 3.10.32 - not implemented in kotext
      style:text-scale, 3.10.33 - not implemented in kotext
      style:text-rotation-angle, 3.10.34 - not implemented in kotext (kpr rotates whole objects)
      style:text-rotation-scale, 3.10.35 - not implemented in kotext (kpr rotates whole objects)
      style:puncuation-wrap, 3.10.36 - not implemented in kotext
      style:line-break, 3.10.37 - what's strict linebreaking?
    */

    if ( format.hasChildNodes() )
        formats.appendChild( format );
}

void OoWriterImport::writeLayout( QDomDocument& doc, QDomElement& layoutElement )
{
    Q_ASSERT( layoutElement.ownerDocument() == doc );

    // Always write out the alignment, it's required
    QDomElement flowElement = doc.createElement("FLOW");

    if ( m_styleStack.hasAttribute( "fo:text-align" ) ) // 3.11.4
        flowElement.setAttribute( "align", Conversion::importAlignment( m_styleStack.attribute( "fo:text-align" ) ) );
    else
        flowElement.setAttribute( "align", "auto" );
    layoutElement.appendChild( flowElement );

    // Indentation (margins)
    OoUtils::importIndents( layoutElement, m_styleStack );

    // Offset before and after paragraph
    OoUtils::importTopBottomMargin( layoutElement, m_styleStack );

    // Line spacing
    OoUtils::importLineSpacing( layoutElement, m_styleStack );

    // Tabulators
    OoUtils::importTabulators( layoutElement, m_styleStack );

    // Borders
    OoUtils::importBorders( layoutElement, m_styleStack );

    // Page breaking. This isn't in OoUtils since it doesn't apply to KPresenter
    if( m_styleStack.hasAttribute("fo:break-before") ||
        m_styleStack.hasAttribute("fo:break-after") ||
        m_styleStack.hasAttribute("style:break-inside") ||
        m_styleStack.hasAttribute("style:keep-with-next") ||
        m_styleStack.hasAttribute("fo:keep-with-next") )
    {
        QDomElement pageBreak = doc.createElement( "PAGEBREAKING" );
        if ( m_styleStack.hasAttribute("fo:break-before") ) { // 3.11.24
            bool breakBefore = m_styleStack.attribute( "fo:break-before" ) != "auto";
            // TODO in KWord: implement difference between "column" and "page"
            pageBreak.setAttribute("hardFrameBreak", breakBefore ? "true" : "false");
        }
        else if ( m_styleStack.hasAttribute("fo:break-after") ) { // 3.11.24
            bool breakAfter = m_styleStack.attribute( "fo:break-after" ) != "auto";
            // TODO in KWord: implement difference between "column" and "page"
            pageBreak.setAttribute("hardFrameBreakAfter", breakAfter ? "true" : "false");
        }

        if ( m_styleStack.hasAttribute( "style:break-inside" ) ) { // 3.11.7
            bool breakInside = m_styleStack.attribute( "style:break-inside" ) == "true";
            pageBreak.setAttribute("linesTogether", breakInside ? "false" : "true"); // opposite meaning
        }
        if ( m_styleStack.hasAttribute( "style:keep-with-next" ) ) // 3.11.31
            // Copy the boolean value
            pageBreak.setAttribute("keepWithNext", m_styleStack.attribute( "style:keep-with-next" ));
        else if ( m_styleStack.hasAttribute( "fo:keep-with-next" ) ) // 3.11.31 (they say StarOffice, but OOo 1.0.1 uses that too)
            // Copy the boolean value
            pageBreak.setAttribute("keepWithNext", m_styleStack.attribute( "fo:keep-with-next" ));
        layoutElement.appendChild( pageBreak );
    }

    // TODO fo:background-color - not here; text property in kword/kpresenter.
    // TODO padding??? (space around the paragraph) => how is that different from margins
    // TODO shadow

/*
  Paragraph properties not implemented in KWord:
    style:text-align-last
    style:justify-single-word
    fo:widows
    fo:orphans
    fo:hyphenate
    fo:hyphenation-keep
    fo:hyphenation-remain-char-count
    fo:hyphenation-push-char-count
    fo:hyphenation-ladder-count
    style:drop-cap
    style:register-true
    style:auto-text-indent
    "page sequence entry point"
    style:background-image
    line numbering
    text autospace, punctuation wrap
    vertical alignment - a bit like offsetfrombaseline...
*/

}

void OoWriterImport::appendField(QDomDocument& doc, QDomElement& e, const QDomElement& object, uint pos)
{
    // TODO
}

#include "oowriterimport.moc"
