/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2003 David Faure <faure@kde.org>
   Copyright 2002, 2003 Nicolas GOUTTE <goutte@kde.org>

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

#include <kdeversion.h>
#include <kdebug.h>
#include <kzip.h>

#include <koDocumentInfo.h>
#include <koDocument.h>

#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <koFilterChain.h>
#include <koGlobal.h>
#include <koPicture.h>
#include "conversion.h"

#if ! KDE_IS_VERSION(3,1,90)
# include <kdebugclasses.h>
#endif

typedef KGenericFactory<OoWriterImport, KoFilter> OoWriterImportFactory;
K_EXPORT_COMPONENT_FACTORY( liboowriterimport, OoWriterImportFactory( "oowriterimport" ) );


OoWriterImport::OoWriterImport( KoFilter *, const char *, const QStringList & )
  : KoFilter(), m_pictureNumber(0), m_zip(NULL)
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

    m_zip=new KZip(m_chain->inputFile());

    kdDebug(30518) << "Store created" << endl;

    if ( !m_zip->open(IO_ReadOnly) )
    {
        kdError(30518) << "Couldn't open the requested file "<< m_chain->inputFile() << endl;
        return KoFilter::FileNotFound;
    }

    if ( !m_zip->directory() )
    {
        kdError(30518) << "Couldn't read ZIP directory of the requested file "<< m_chain->inputFile() << endl;
        return KoFilter::FileNotFound;
    }


    KoFilter::ConversionStatus preStatus = openFile();

    if ( preStatus != KoFilter::OK )
    {
        m_zip->close();
        delete m_zip;
        return preStatus;
    }

    m_currentMasterPage = QString::null;
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

    m_zip->close();
    delete m_zip; // It has to be so late, as pictures might be read.

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
    for ( QDomNode n = fixedStyles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        if ( !e.hasAttribute( "style:name" ) )
            continue;
        // We only generate paragraph styles for now
        if ( e.attribute( "style:family" ) != "paragraph" )
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

void OoWriterImport::parseBodyOrSimilar( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement )
{
    for ( QDomNode text (parent.firstChild()); !text.isNull(); text = text.nextSibling() )
    {
        m_styleStack.save();
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
        else if ( name == "text:unordered-list" || name == "text:ordered-list" ) // list
        {
            currentFramesetElement.appendChild( parseList( doc, t ) );
            continue;
        }
        else if ( name == "text:section" ) // Provisory support (###TODO)
        {
            kdDebug(30518) << "Section found!" << endl;
            fillStyleStack( t );
            parseBodyOrSimilar( doc, t, currentFramesetElement);
        }
        else if ( name == "table:table" )
        {
            kdDebug(30518) << "Table found!" << endl;
            parseTable( doc, t, currentFramesetElement );
        }
        // TODO text:sequence-decls
        else
        {
            kdDebug(30518) << "Unsupported texttype '" << name << "'" << endl;
            m_styleStack.restore();
            continue;
        }

        currentFramesetElement.appendChild( e );
        m_styleStack.restore(); // remove the styles added by the paragraph or list
    }
}

void OoWriterImport::createDocumentContent( QDomDocument &doc, QDomElement& mainFramesetElement )
{
    QDomElement content = m_content.documentElement();

    QDomElement body ( content.namedItem( "office:body" ).toElement() );
    if ( body.isNull() )
    {
        kdError(30518) << "No office:body found!" << endl;
        return;
    }

    parseBodyOrSimilar( doc, body, mainFramesetElement);
}

void OoWriterImport::writePageLayout( QDomDocument& mainDocument, const QString& masterPageName )
{
    QDomElement docElement = mainDocument.documentElement();

    kdDebug(30518) << "writePageLayout " << masterPageName << endl;
    KoOrientation orientation;
    double width, height;
    KoFormat paperFormat;
    double marginLeft, marginTop, marginRight, marginBottom;

    QDomElement* masterPage = m_masterPages[ masterPageName ];
    Q_ASSERT( masterPage );
    QDomElement *style = masterPage ? m_styles[masterPage->attribute( "style:page-master-name" )] : 0;
    Q_ASSERT( style );
    if ( style )
    {
        QDomElement properties( style->namedItem( "style:properties" ).toElement() );
        orientation = ( (properties.attribute("style:print-orientation") != "portrait") ? PG_LANDSCAPE : PG_PORTRAIT );
        width = KoUnit::parseValue(properties.attribute("fo:page-width"));
        height = KoUnit::parseValue(properties.attribute("fo:page-height"));
        // guessFormat takes millimeters
        if ( orientation == PG_LANDSCAPE )
            paperFormat = KoPageFormat::guessFormat( POINT_TO_MM(height), MM_TO_POINT(width) );
        else
            paperFormat = KoPageFormat::guessFormat( POINT_TO_MM(width), MM_TO_POINT(height) );

        marginLeft = KoUnit::parseValue(properties.attribute("fo:margin-left"));
        marginTop = KoUnit::parseValue(properties.attribute("fo:margin-top"));
        marginRight = KoUnit::parseValue(properties.attribute("fo:margin-right"));
        marginBottom = KoUnit::parseValue(properties.attribute("fo:margin-bottom"));
    }
    else
    {
        // We have no master page! We need defaults.
        orientation=PG_PORTRAIT;
        paperFormat=PG_DIN_A4;
        width=MM_TO_POINT(KoPageFormat::width(paperFormat, orientation));
        height=MM_TO_POINT(KoPageFormat::height(paperFormat, orientation));
        // ### TODO: better defaults for margins?
        marginLeft=MM_TO_POINT(10.0);
        marginRight=MM_TO_POINT(10.0);
        marginTop=MM_TO_POINT(15.0);
        marginBottom=MM_TO_POINT(15.0);
    }

    QDomElement elementPaper = mainDocument.createElement("PAPER");
    elementPaper.setAttribute("orientation", int(orientation) );
    elementPaper.setAttribute("width", width);
    elementPaper.setAttribute("height", height);
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
    element.setAttribute("left", marginLeft);
    element.setAttribute("top", marginTop);
    element.setAttribute("right", marginRight);
    element.setAttribute("bottom", marginBottom);
    elementPaper.appendChild(element);
}

void OoWriterImport::prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem )
{
    mainDocument = KoDocument::createDomDocument( "kword", "DOC", "1.2" );
    QDomElement docElement = mainDocument.documentElement();
    docElement.setAttribute( "editor", "KWord's OOWriter Import Filter" );
    docElement.setAttribute( "mime", "application/x-kword" );
    docElement.setAttribute( "syntaxVersion", "2" );

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

KoFilter::ConversionStatus OoWriterImport::loadAndParse(const QString& filename, QDomDocument& doc)
{
    kdDebug(30518) << "Trying to open " << filename << endl;

    if (!m_zip)
    {
        kdError(30518) << "No ZIP file!" << endl;
        return KoFilter::CreationError; // Should not happen
    }

    const KArchiveEntry* entry = m_zip->directory()->entry( filename );
    if (!entry)
    {
        kdWarning(30518) << "Entry " << filename << " not found!" << endl;
        return KoFilter::FileNotFound;
    }
    if (entry->isDirectory())
    {
        kdWarning(30518) << "Entry " << filename << " is a directory!" << endl;
        return KoFilter::WrongFormat;
    }
    const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
    QIODevice* io=f->device();
    kdDebug(30518) << "Entry " << filename << " has size " << f->size() << endl;

    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !doc.setContent( io, &errorMsg, &errorLine, &errorColumn ) )
    {
        kdError(30518) << "Parsing error in " << filename << "! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
        delete io;
        return KoFilter::ParsingError;
    }
    delete io;

    kdDebug(30518) << "File " << filename << " loaded and parsed!" << endl;

    return KoFilter::OK;
}

KoFilter::ConversionStatus OoWriterImport::openFile()
{
    KoFilter::ConversionStatus status=loadAndParse("content.xml", m_content);
    if ( status != KoFilter::OK )
    {
        kdError(30518) << "Content.xml could not be parsed correctly! Aborting!" << endl;
        return status;
    }

    //kdDebug(30518)<<" m_content.toCString() :"<<m_content.toCString()<<endl;

    // We need to keep the QDomDocument for styles too, unfortunately.
    // Otherwise styleElement.parentNode() returns a null node
    // (see StyleStack::isUserStyle). Most of styles.xml is in m_styles
    // anyway, so this doesn't make a big difference.
    // We now also rely on this in createStyles.
    //QDomDocument styles;

    // We do not stop if the following calls fail.
    loadAndParse("styles.xml", m_stylesDoc);
    loadAndParse("meta.xml", m_meta);
    loadAndParse("settings.xml", m_settings);

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

  if ( !masterStyles.isNull() )
  {
      QDomElement master = masterStyles.firstChild().toElement();
      for ( ; !master.isNull() ; master = master.nextSibling().toElement() )
      {
          if ( master.tagName() ==  "style:master-page" )
          {
              QString name = master.attribute( "style:name" );
              kdDebug(30518) << "Master style: '" << name << "' loaded " << endl;
              m_masterPages.insert( name, new QDomElement( master ) );
          } else
              kdWarning(30518) << "Unknown tag " << master.tagName() << " in office:master-styles" << endl;
      }
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

QDomDocumentFragment OoWriterImport::parseList( QDomDocument& doc, const QDomElement& list )
{
    //kdDebug(30518) << k_funcinfo << "parsing list"<< endl;

    bool isOrdered;
    if ( list.tagName() == "text:ordered-list" )
        isOrdered = true;
    else
        isOrdered = false;

    QDomDocumentFragment fragment = doc.createDocumentFragment();

    // take care of nested lists
    // ### DF: I think this doesn't take care of them the right way. We need to save/parse-whole-list/restore.
    QDomElement e;
    uint listCounter = 1;
    for ( QDomNode n = list.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        e = n.firstChild().toElement();
        QString name = e.tagName();

        //kdDebug(30518) << k_funcinfo << "Got tag: " << name << endl;

        // parse the list-properties
        fillStyleStack( e );
        QDomElement p = parseParagraph( doc, e );

        QDomElement counter = doc.createElement( "COUNTER" ); // should be under <LAYOUT>
        counter.setAttribute( "numberingtype", 0 );
        counter.setAttribute( "depth", 0 );

        if ( isOrdered ) {
            counter.setAttribute("type", 1); // an arabic number
            counter.setAttribute("righttext", ".");
            counter.setAttribute("text", listCounter);
            counter.setAttribute("align", 0);
        }
        else
            counter.setAttribute( "type", 10 ); // a disc bullet

        // Don't 'appendChild()'! Text elements have to be the last children of the
        // paragraph element otherwise kpresenter will cut off the last character of
        // every item!
        p.insertBefore( counter, QDomNode() );

        fragment.appendChild(p);
        listCounter++;
    }

    return fragment;
}

void OoWriterImport::parseSpanOrSimilar( QDomDocument& doc, const QDomElement& parent,
    QDomElement& outputParagraph, QDomElement& outputFormats, QString& paragraphText, uint& pos)
{
    // parse every child node of the parent
    for( QDomNode node ( parent.firstChild() ); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement ts ( node.toElement() );
        QString textData;
        QString tagName( ts.tagName() );
        QDomText t ( node.toText() );

        bool shouldWriteFormat=false; // By default no <FORMAT> element should be written

        // Try to keep the order of the tag names by probability of happening
        if (tagName == "text:span")
        {
            m_styleStack.save();
            fillStyleStack( ts );
            parseSpanOrSimilar( doc, ts, outputParagraph, outputFormats, paragraphText, pos);
            m_styleStack.restore();
        }
        else if (tagName == "text:s")
        {
            textData = OoUtils::expandWhitespace(ts);
            shouldWriteFormat=true;
        }
        else if ( tagName == "text:tab-stop" )
        {
            // KWord currently uses \t.
            // Known bug: a line with only \t\t\t\t isn't loaded - XML (QDom) strips out whitespace.
            // One more good reason to switch to <text:tab-stop> instead...
            textData = '\t';
            shouldWriteFormat=true;
        }
        else if ( tagName == "text:line-break" )
        {
            textData = '\n';
            shouldWriteFormat=true;
        }
        else if ( tagName == "draw:image" )
        {
            textData = '#'; // anchor placeholder
            appendPicture(doc, outputFormats, ts, pos);
        }
        else if ( tagName == "text:a" )
        {
            m_styleStack.save();
            QString href( ts.attribute("xlink:href") );
            if ( href.startsWith("#") )
            {
                // We have a reference to a bookmark (### TODO)
                // As we do not support it now, treat it as a <text:span> without formatting
                parseSpanOrSimilar( doc, ts, outputParagraph, outputFormats, paragraphText, pos);
            }
            else
            {
                // The problem is that KWord's hyperlink text is not inside the normal text, but for OOWriter it is nearly a <text:span>
                // So we have to fake.
                QDomElement fakeParagraph, fakeFormats;
                uint fakePos=0;
                QString text;
                parseSpanOrSimilar( doc, ts, fakeParagraph, fakeFormats, text, fakePos);
                textData = '#'; // hyperlink placeholder
                QDomElement linkElement (doc.createElement("LINK"));
                linkElement.setAttribute("hrefName",ts.attribute("xlink:href"));
                linkElement.setAttribute("linkName",text);
                appendKWordVariable(doc, outputFormats, ts, pos, "STRING", 9, text, linkElement);
            }
            m_styleStack.restore();
        }
        // ### Fields should be moved to an appendField method like in OoImpressImport
        else if (tagName == "text:date")
        {
            textData = '#';     // field placeholder
            QDomElement dateElement ( doc.createElement("DATE") );
            // As we have no idea about the current date, use the *nix epoch 1970-01-01
            dateElement.setAttribute("year",1970);
            dateElement.setAttribute("month",1);
            dateElement.setAttribute("day",1);
            dateElement.setAttribute("fix",0);  // Has OOWriter fixed dates?
            appendKWordVariable(doc, outputFormats, ts, pos, "DATE0Locale", 0, "-", dateElement);
        }
        else if (tagName == "text:time")
        {
            textData = '#';     // field placeholder
            QDomElement timeElement (doc.createElement("TIME") );
            // We cannot calculate the time, so default to midnight
            timeElement.setAttribute("hour",0);
            timeElement.setAttribute("minute",0);
            timeElement.setAttribute("second",0);
            timeElement.setAttribute("fix",0); // Has OOWriter fixed times?
            appendKWordVariable(doc, outputFormats, ts, pos, "TIMELocale", 2, "-", timeElement);
        }
        else if ( tagName == "text:page-number" )
        {
            textData = '#';     // field placeholder
            QDomElement pgnumElement ( doc.createElement("PGNUM") );
            pgnumElement.setAttribute("subtype",0);
            pgnumElement.setAttribute("value",1);
            appendKWordVariable(doc, outputFormats, ts, pos, "NUMBER", 4, "1", pgnumElement);
        }
        else if ( tagName == "text:file-name" )
        {
            textData = '#';     // field placeholder
            QDomElement fieldElement ( doc.createElement("FIELD") );
            fieldElement.setAttribute("subtype",0);
            fieldElement.setAttribute("value","?");
            appendKWordVariable(doc, outputFormats, ts, pos, "STRING", 8, "?", fieldElement);
        }
        else if ( tagName == "text:author-name"
                 || tagName == "text:author-initials")
        {
            textData = '#';     // field placeholder
            // ### TODO
        }
        else if ( t.isNull() ) // no textnode, we must ignore
        {
            kdWarning(30518) << "Ignoring tag " << ts.tagName() << endl;
            continue;
        }
        else
        {
            textData = t.data();
            shouldWriteFormat=true;
        }

        paragraphText += textData;
        const uint length = textData.length();

        if (shouldWriteFormat)
        {
            writeFormat( doc, outputFormats, 1 /* id for normal text */, pos, length );
        }

        pos += length;
    }
}

QDomElement OoWriterImport::parseParagraph( QDomDocument& doc, const QDomElement& paragraph )
{
    QDomElement p = doc.createElement( "PARAGRAPH" );

    // parse the paragraph-properties
    fillStyleStack( paragraph );

    QDomElement formats = doc.createElement( "FORMATS" );

    QString paragraphText;
    uint pos = 0;

    m_styleStack.save();
    // parse every child node of the paragraph
    parseSpanOrSimilar( doc, paragraph, p, formats, paragraphText, pos);
    m_styleStack.restore(); // remove possible garbage (should not be needed)

    QDomElement text = doc.createElement( "TEXT" );
    text.appendChild( doc.createTextNode( paragraphText ) );
    text.setAttribute( "xml:space", "preserve" );
    p.appendChild( text );
    //kdDebug(30518) << k_funcinfo << "Para text is: " << paragraphText << endl;

    p.appendChild( formats );
    QDomElement layoutElement = doc.createElement( "LAYOUT" );
    p.appendChild( layoutElement );

    // Style name
    QString styleName = m_styleStack.userStyleName();
    if ( !styleName.isEmpty() )
    {
        QDomElement nameElement = doc.createElement("NAME");
        nameElement.setAttribute("value", styleName);
        layoutElement.appendChild(nameElement);
    }

    writeLayout( doc, layoutElement );

    QDomElement* paragraphStyle = m_styles[paragraph.attribute( "text:style-name" )];
    QString masterPageName = paragraphStyle ? paragraphStyle->attribute( "style:master-page-name" ) : QString::null;
    if ( masterPageName.isEmpty() )
        masterPageName = "Standard"; // Seems to be a builtin name for the default layout...
    if ( masterPageName != m_currentMasterPage )
    {
        // Detected a change in the master page -> this means we have to use a new page layout
        // and insert a frame break if not on the first paragraph.
        // In KWord we don't support sections so the first paragraph is the one that determines the page layout.
        if ( m_currentMasterPage.isEmpty() )
            writePageLayout( doc, masterPageName );
        else
        {
            QDomElement pageBreakElem = layoutElement.namedItem( "PAGEBREAKING" ).toElement();
            if ( !pageBreakElem.isNull() )  {
                pageBreakElem = doc.createElement( "PAGEBREAKING" );
                layoutElement.appendChild( pageBreakElem );
            }
            pageBreakElem.setAttribute( "hardFrameBreak", "true" );
            // We have no way to store the new page layout, KWord doesn't have sections.
        }
        m_currentMasterPage = masterPageName;
    }

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
        {
            // ##### TODO. This is wrong. style:font-name refers to a font-decl entry.
            // We have to look it up there, and retrieve _all_ font attributes from it, not just the name.
            fontName = m_styleStack.attribute( "style:font-name" ).remove( "'" );
        }
        // 'Thorndale' is not known outside OpenOffice so we substitute it
        // with 'Times New Roman' that looks nearly the same.
        if ( fontName == "Thorndale" )
            fontName = "Times New Roman";

        fontName.remove(QRegExp("\\sCE$")); // Arial CE -> Arial

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

    if (m_styleStack.hasAttribute("fo:text-shadow")) // 3.10.21
    {
        QDomElement shadow = doc.createElement("SHADOW");
        shadow.setAttribute("text-shadow", m_styleStack.attribute("fo:text-shadow"));
        format.appendChild(shadow);
    }

    /*
      Missing properties:
      style:use-window-font-color, 3.10.4 - this is an automatic fg color depending on the bg color
                    We need the exact algorithm to determine the fg color, I have asked for it (DF)
      style:text-outline, 3.10.5 - not implemented in kotext
      style:font-family-generic, 3.10.10 - roman, swiss, modern -> map to a font?
      style:font-style-name, 3.10.11 - ?
      style:font-pitch, 3.10.12 - fixed or variable -> map to a font?
      style:font-charset, 3.10.14 - not necessary with Qt
      style:font-size-rel, 3.10.15 - TODO in StyleStack::fontSize()
      fo:letter-spacing, 3.10.16 - not implemented in kotext
      style:text-relief, 3.10.20 - not implemented in kotext
      style:letter-kerning, 3.10.20 - not implemented in kotext
      style:text-blinking, 3.10.27 - not implemented in kotext IIRC
      style:text-combine, 3.10.29/30 - not implemented, see http://www.w3.org/TR/WD-i18n-format/
      style:text-emphasis, 3.10.31 - not implemented in kotext
      style:text-scale, 3.10.33 - not implemented in kotext
      style:text-rotation-angle, 3.10.34 - not implemented in kotext (kpr rotates whole objects)
      style:text-rotation-scale, 3.10.35 - not implemented in kotext (kpr rotates whole objects)
      style:punctuation-wrap, 3.10.36 - not implemented in kotext
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
        if ( m_styleStack.hasAttribute( "fo:keep-with-next" ) ) // 3.11.31 (the doc said style:keep-with-next but DV said it's wrong)
            // Copy the boolean value
            pageBreak.setAttribute("keepWithNext", m_styleStack.attribute( "fo:keep-with-next" ));
        layoutElement.appendChild( pageBreak );
    }

    // TODO in KWord: padding
    /* padding works together with the borders. The margins are around a
     * paragraph, and the padding is the space between the border and the
     * paragraph. In the OOo UI, you can only select padding when you have
     * selected a border.
     *
     * There's some difference in conjunction with other features, in that the
     * margin area is outside the paragraph, and the padding area is inside the
     * paragraph. So if you set a paragraph background color, the padding area
     * will be colored, but the margin area won't.
     */

/*
  Paragraph properties not implemented in KWord:
    style:text-align-last
    style:justify-single-word
    fo:background-color (bg color for a paragraph, unlike style:text-background-color)
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
  Michael said those are in fact parag properties:
    style:text-autospace, 3.10.32 - not implemented in kotext
    style:line-break, 3.10.37 - what's strict linebreaking?
*/

}

void OoWriterImport::appendPicture(QDomDocument& doc, QDomElement& formats, const QDomElement& object, uint pos)
{
    const QString frameName ( object.attribute("draw:name") ); // ### TODO: what if empty, i.e. non-unique
    const double height=KoUnit::parseValue( object.attribute("svg:height") );
    const double width=KoUnit::parseValue( object.attribute("svg:width") );
    const QString href ( object.attribute("xlink:href") );

    kdDebug(30518) << "Picture: " << frameName << " " << href << " (in OoWriterImport::appendPicture)" << endl;

    KoPicture picture;
    if ( href[0]=='#' )
    {
        QString strExtension;
        const int result=href.findRev(".");
        if (result>=0)
        {
            strExtension=href.mid(result+1); // As we are using KoPicture, the extension should be without the dot.
        }
        QString filename(href.mid(1));
        KoPictureKey key(filename, QDateTime::currentDateTime(Qt::UTC));
        picture.setKey(key);

        if (!m_zip)
            return; // Should not happen

        const KArchiveEntry* entry = m_zip->directory()->entry( filename );
        if (!entry)
        {
            kdWarning(30518) << "Picture " << filename << " not found!" << endl;
            return;
        }
        if (entry->isDirectory())
        {
            kdWarning(30518) << "Picture " << filename << " is a directory!" << endl;
            return;
        }
        const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
        QIODevice* io=f->device();
        kdDebug(30518) << "Picture " << filename << " has size " << f->size() << endl;

        if (!io)
        {
            kdWarning(30518) << "No QIODevice for picture  " << frameName << " " << href << endl;
            return;
        }
        if (!picture.load(io,strExtension))
            kdWarning(30518) << "Cannot load picture: " << frameName << " " << href << endl;
    }
    else
    {
        KURL url;
        url.setPath(href); // ### TODO: is this really right?
        picture.setKeyAndDownloadPicture(url);
    }

    kdDebug(30518) << "Picture ready! Key: " << picture.getKey().toString() << " Size:" << picture.getOriginalSize() << endl;

    QString strStoreName;
    strStoreName="pictures/picture";
    strStoreName+=QString::number(++m_pictureNumber);
    strStoreName+='.';
    strStoreName+=picture.getExtension();

    kdDebug(30518) << "Storage name: " << strStoreName << endl;

    KoStoreDevice* out = m_chain->storageFile( strStoreName , KoStore::Write );
    if (out)
    {
        if (!out->open(IO_WriteOnly))
        {
            kdWarning(30518) << "Cannot open for saving picture: " << frameName << " " << href << endl;
            return;
        }
        if (!picture.save(out))
        kdWarning(30518) << "Cannot save picture: " << frameName << " " << href << endl;
        out->close();
    }
    else
    {
         kdWarning(30518) << "Cannot store picture: " << frameName << " " << href << endl;
         return;
    }

    // Now that we have copied the image, we need to make some bookkeeping

    QDomElement docElement( doc.documentElement() );

    QDomElement framesetsPluralElement ( docElement.namedItem("FRAMESETS").toElement() );

    QDomElement framesetElement=doc.createElement("FRAMESET");
    framesetElement.setAttribute("frameType",2);
    framesetElement.setAttribute("frameInfo",0);
    framesetElement.setAttribute("visible",1);
    framesetElement.setAttribute("name",frameName);
    framesetsPluralElement.appendChild(framesetElement);

    QDomElement frameElementOut=doc.createElement("FRAME");
    frameElementOut.setAttribute("left",0);
    frameElementOut.setAttribute("top",0);
    frameElementOut.setAttribute("bottom",height);
    frameElementOut.setAttribute("right" ,width );
    frameElementOut.setAttribute("runaround",1);
    // TODO: a few attributes are missing
    framesetElement.appendChild(frameElementOut);

    QDomElement element=doc.createElement("PICTURE");
    element.setAttribute("keepAspectRatio","true");
    framesetElement.setAttribute("frameType",2); // Picture
    framesetElement.appendChild(element);

    QDomElement singleKey ( doc.createElement("KEY") );
    picture.getKey().saveAttributes(singleKey);
    element.appendChild(singleKey);

    QDomElement picturesPluralElement ( docElement.namedItem("PICTURES").toElement() );
    if (picturesPluralElement.isNull())
    {
        // We do not yet have any <PICTURES> element, so we must create it
        picturesPluralElement = doc.createElement("PICTURES");
        docElement.appendChild(picturesPluralElement);
    }

    QDomElement pluralKey ( doc.createElement("KEY") );
    picture.getKey().saveAttributes(pluralKey);
    pluralKey.setAttribute("name",strStoreName);
    picturesPluralElement.appendChild(pluralKey);

    QDomElement formatElementOut=doc.createElement("FORMAT");
    formatElementOut.setAttribute("id",6); // Normal text!
    formatElementOut.setAttribute("pos",pos); // Start position
    formatElementOut.setAttribute("len",1); // Start position
    formats.appendChild(formatElementOut); //Append to <FORMATS>

    QDomElement anchor=doc.createElement("ANCHOR");
    // No name attribute!
    anchor.setAttribute("type","frameset");
    anchor.setAttribute("instance",frameName);
    formatElementOut.appendChild(anchor);
}

void OoWriterImport::appendKWordVariable(QDomDocument& doc, QDomElement& formats, const QDomElement& /*object*/, uint pos,
    const QString& key, int type, const QString& text, QDomElement& child)
{
    QDomElement variableElement ( doc.createElement("VARIABLE") );

    QDomElement typeElement ( doc.createElement("TYPE") );
    typeElement.setAttribute("key",key);
    typeElement.setAttribute("type",type);
    typeElement.setAttribute("text",text);
    variableElement.appendChild(typeElement); //Append to <VARIABLE>

    variableElement.appendChild(child); //Append to <VARIABLE>

    QDomElement formatElement ( doc.createElement("FORMAT") );
    formatElement.setAttribute("id",4); // Variable
    formatElement.setAttribute("pos",pos); // Start position
    formatElement.setAttribute("len",1);

    formatElement.appendChild(variableElement);

    formats.appendChild(formatElement);
}

void OoWriterImport::parseTable( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement )
{
    QString tableName ( parent.attribute("table:name") ); // TODO: what is empty (non-unique?)
    kdDebug(30518) << "Found table " << tableName << endl;

    // In OOWriter a table is never inisde a paragrpah, in KWord it is always in a paragraph
    QDomElement paragraphElementOut (doc.createElement("PARAGRAPH"));
    currentFramesetElement.appendChild(paragraphElementOut);

    QDomElement textElementOut(doc.createElement("TEXT"));
    textElementOut.appendChild(doc.createTextNode("#"));
    paragraphElementOut.appendChild(textElementOut);

    QDomElement formatsPluralElementOut(doc.createElement("FORMATS"));
    paragraphElementOut.appendChild(formatsPluralElementOut);

    QDomElement elementFormat(doc.createElement("FORMAT"));
    elementFormat.setAttribute("id",6);
    elementFormat.setAttribute("pos",0);
    elementFormat.setAttribute("len",1);
    formatsPluralElementOut.appendChild(elementFormat);

    QDomElement elementAnchor(doc.createElement("ANCHOR"));
    elementAnchor.setAttribute("type","frameset");
    elementAnchor.setAttribute("instance",tableName);
    elementFormat.appendChild(elementAnchor);

    uint row=0;
    uint column=0;
    parseInsideOfTable(doc, parent, currentFramesetElement, tableName, row, column);
}

void OoWriterImport::parseInsideOfTable( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement,
    const QString& tableName, uint& row, uint& column )
{
    QDomElement framesetsPluralElement (doc.documentElement().namedItem("FRAMESETS").toElement());
    if (framesetsPluralElement.isNull())
    {
        kdError(30518) << "Cannot find KWord's <FRAMESETS>! Cannot process table!" << endl;
        return;
    }

    for ( QDomNode text (parent.firstChild()); !text.isNull(); text = text.nextSibling() )
    {
        m_styleStack.save();
        QDomElement t = text.toElement();
        QString name = t.tagName();

        if ( name == "table:table-cell" )
        {
            const QString frameName(i18n("Frameset name","Table %3, row %1, column %2")
                .arg(row).arg(column).arg(tableName)); // The table name could have a % sequence so, use the table name as last!
            kdDebug(30518) << "Trying to create " << frameName << endl;

            // We need to create a frameset for the cell
            QDomElement framesetElement(doc.createElement("FRAMESET"));
            framesetElement.setAttribute("frameType",1);
            framesetElement.setAttribute("frameInfo",0);
            framesetElement.setAttribute("visible",1);
            framesetElement.setAttribute("name",frameName);
            framesetElement.setAttribute("row",row);
            framesetElement.setAttribute("col",column);
            framesetElement.setAttribute("rows",1); // ### TODO: rowspan
            framesetElement.setAttribute("cols",1); // ### TODO: colspan
            framesetElement.setAttribute("grpMgr",tableName);
            framesetsPluralElement.appendChild(framesetElement);

            QDomElement frameElementOut(doc.createElement("FRAME"));
            //frameElementOut.setAttribute("left",28);
            //frameElementOut.setAttribute("top",42);
            //frameElementOut.setAttribute("bottom",566);
            //frameElementOut.setAttribute("right",798);
            frameElementOut.setAttribute("runaround",1);
            // ### TODO: a few attributes are missing
            framesetElement.appendChild(frameElementOut);

            parseBodyOrSimilar( doc, t, framesetElement); // We change the frameset!
            column++;
        }
        else if ( name == "table:table-row" )
        {
            column=0;
            parseInsideOfTable( doc, t, currentFramesetElement, tableName, row, column);
            row++;
        }
        else if ( name == "table:table-header-rows" ) // Provisory (###TODO)
        {
            parseInsideOfTable( doc, t, currentFramesetElement, tableName, row, column);
        }
        else
        {
            kdWarning(30518) << "Skiping element " << name << " (in OoWriterImport::parseInsideOfTable)" << endl;
        }

        m_styleStack.restore();
    }
}

#include "oowriterimport.moc"

// TODO style:num-format, default number format for page styles,
// used for page numbers (2.3.1)
