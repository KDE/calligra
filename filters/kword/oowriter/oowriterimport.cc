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
#include <koRect.h>

#if ! KDE_IS_VERSION(3,1,90)
# include <kdebugclasses.h>
#endif

typedef KGenericFactory<OoWriterImport, KoFilter> OoWriterImportFactory;
K_EXPORT_COMPONENT_FACTORY( liboowriterimport, OoWriterImportFactory( "oowriterimport" ) )


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
            fillStyleStack( t, "text:style-name" );
            parseBodyOrSimilar( doc, t, currentFramesetElement);
        }
        else if ( name == "table:table" )
        {
            kdDebug(30518) << "Table found!" << endl;
            parseTable( doc, t, currentFramesetElement );
        }
        else if ( name == "draw:image" )
        {
            appendPicture( doc, t );
        }
        else if ( name == "draw:text-box" )
        {
            appendTextBox( doc, t );
        }
        // TODO text:sequence-decls
        else
        {
            kdDebug(30518) << "Unsupported body element '" << name << "'" << endl;
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

void OoWriterImport::fillStyleStack( const QDomElement& object, const QString& attrName )
{
    // find all styles associated with an object and push them on the stack
    // OoImpressImport has more tests here, but I don't think they're relevant to OoWriterImport
    if ( object.hasAttribute( attrName ) )
        addStyles( m_styles[object.attribute( attrName )] );
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

        //kdDebug(30518) << k_funcinfo << "Got tag: " << e.tagName() << endl;

        // parse the list-properties
        fillStyleStack( e, "text:style-name" );
        QDomElement p = parseParagraph( doc, e );

        QDomElement counter = doc.createElement( "COUNTER" );
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

        QDomElement layout (p.namedItem("LAYOUT").toElement());
        if (layout.isNull())
            kdWarning(30518) << "Cannot find <LAYOUT> in KWord paragraph." << endl;
        else
            layout.appendChild(counter);

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
            fillStyleStack( ts, "text:style-name" );
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
            QString frameName = appendPicture(doc, ts);
            anchorFrameset( doc, outputFormats, pos, frameName );
        }
        else if ( tagName == "draw:text-box" )
        {
            textData = '#'; // anchor placeholder
            QString frameName = appendTextBox(doc, ts);
            anchorFrameset( doc, outputFormats, pos, frameName );
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
                appendKWordVariable(doc, outputFormats, ts, pos, "STRING", 9, linkElement);
            }
            m_styleStack.restore();
        }
        else if (tagName == "text:date" // fields
                 || tagName == "text:time"
                 || tagName == "text:page-number"
                 || tagName == "text:file-name"
                 || tagName == "text:author-name"
                 || tagName == "text:author-initials")
        {
            textData = "#";     // field placeholder
            appendField(doc, outputFormats, ts, pos);
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
    fillStyleStack( paragraph, "text:style-name" );

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
    // On 2nd thought, this doesn't seem to happen. If a style property needs undoing
    // OO always writes it down in the XML, so we write out the property just fine.
    // Both apps implement a "write property only if necessary" mechanism, I can't'
    // find a case that breaks yet.

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
        QString text_position = m_styleStack.attribute("style:text-position");
        QString value;
        QString relativetextsize;
        OoUtils::importTextPosition( text_position, value, relativetextsize );
        vertAlign.setAttribute( "value", value );
        if ( !relativetextsize.isEmpty() )
            vertAlign.setAttribute( "relativetextsize", relativetextsize );
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
        if ( !underLineColor.isEmpty() && underLineColor != "font-color" )
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
      style:use-window-font-color, 3.10.4 - this is what KWord uses by default (fg color from the color style)
         OO also switches to another color when necessary to avoid dark-on-dark and light-on-light cases.
         (that is TODO in KWord)
      style:text-outline, 3.10.5 - not implemented in kotext
      style:font-family-generic, 3.10.10 - roman, swiss, modern -> map to a font?
      style:font-style-name, 3.10.11 - can be ignored, says DV, the other ways to specify a font are more precise
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

    /* This was only an intermediate OASIS decision. The final decision is:
     *  fo:text-align can be "left", "right", "center", "justify", and
     *  "start" will mean direction-dependent. However if we use this right now,
     *  OOo won't understand it. So that's for later, we keep our own attribute
     *  for now, so that export-import works.
     */
    if ( m_styleStack.attribute( "style:text-auto-align" ) == "true" )
        flowElement.setAttribute( "align", "auto" );
    else
    {
        if ( m_styleStack.hasAttribute( "fo:text-align" ) ) // 3.11.4
            flowElement.setAttribute( "align", Conversion::importAlignment( m_styleStack.attribute( "fo:text-align" ) ) );
        else
            flowElement.setAttribute( "align", "auto" );
    }
    layoutElement.appendChild( flowElement );

    if ( m_styleStack.hasAttribute( "fo:writing-mode" ) ) // http://web4.w3.org/TR/xsl/slice7.html#writing-mode
    {
        // LTR is lr-tb. RTL is rl-tb
        QString writingMode = m_styleStack.attribute( "fo:writing-mode" );
        flowElement.setAttribute( "dir", writingMode=="rl-tb" || writingMode=="rl" ? "R" : "L" );
    }

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
    fo:background-color (3.11.25, bg color for a paragraph, unlike style:text-background-color)
    style:background-image (3.11.26)
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
    punctuation wrap, 3.10.36
    vertical alignment - a bit like offsetfrombaseline (but not for subscript/superscript, in general)
  Michael said those are in fact parag properties:
    style:text-autospace, 3.10.32 - not implemented in kotext
    style:line-break, 3.10.37 - apparently that's for some Asian languages
*/

}

void OoWriterImport::importFrame( QDomElement& frameElementOut, const QDomElement& object, bool isText )
{
    double width = 100;
    if ( object.hasAttribute( "svg:width" ) ) { // fixed width
        // TODO handle percentage (of enclosing table/frame/page)
        width = KoUnit::parseValue( object.attribute( "svg:width" ) );
    } else if ( object.hasAttribute( "fo:min-width" ) ) {
        // min-width is not supported in KWord. Let's use it as a fixed width.
        width = KoUnit::parseValue( object.attribute( "fo:min-width" ) );
    } else {
        kdWarning(30518) << "Error in text-box: neither width nor min-width specified!" << endl;
    }
    double height = 100;
    bool hasMinHeight = false;
    if ( object.hasAttribute( "svg:height" ) ) { // fixed height
        // TODO handle percentage (of enclosing table/frame/page)
        height = KoUnit::parseValue( object.attribute( "svg:height" ) );
    } else if ( object.hasAttribute( "fo:min-height" ) ) {
        height = KoUnit::parseValue( object.attribute( "fo:min-height" ) );
        hasMinHeight = true;
    } else {
        kdWarning(30518) << "Error in text-box: neither height nor min-height specified!" << endl;
    }

    int overflowBehavior;
    if ( isText ) {
        if ( m_styleStack.hasAttribute( "style:overflow-behavior" ) ) { // OASIS extension
            overflowBehavior = Conversion::importOverflowBehavior( m_styleStack.attribute( "style:overflow-behavior" ) );
        } else {
            // AutoCreateNewFrame not supported in OO-1.1. The presence of min-height tells if it's an auto-resized frame.
            overflowBehavior = hasMinHeight ? 0 /*AutoExtendFrame*/ : 2 /*Ignore, i.e. fixed size*/;
        }
    }

    // padding. fo:padding for 4 values or padding-left/right/top/bottom (3.11.29 p228)
    double paddingLeft = KoUnit::parseValue( m_styleStack.attribute( "fo:padding", "left" ) );
    double paddingRight = KoUnit::parseValue( m_styleStack.attribute( "fo:padding", "right" ) );
    double paddingTop = KoUnit::parseValue( m_styleStack.attribute( "fo:padding", "top" ) );
    double paddingBottom = KoUnit::parseValue( m_styleStack.attribute( "fo:padding", "bottom" ) );

    // background color (3.11.25)
    bool transparent = false;
    QColor bgColor;
    if ( m_styleStack.hasAttribute( "fo:background-color" ) ) {
        QString color = m_styleStack.attribute( "fo:background-color" );
        if ( color == "transparent" )
            transparent = true;
        else
            bgColor.setNamedColor( color );
    }

    // Available in the style: draw:stroke, svg:stroke-color, draw:fill, draw:fill-color,
    // draw:textarea-vertical-align, draw:textarea-horizontal-align
    // More generally: Anchor (not in kword), Border (3.11.27), Shadow (3.11.30), Columns

    // Not supported in KWord: fo:max-height  fo:max-width

    //  #### horizontal-pos horizontal-rel vertical-pos vertical-rel anchor-type
    //  All the above changes the placement!
    //  See 3.8 (p199) for details.

    // TODO draw:auto-grow-height  draw:auto-grow-width - hmm? I thought min-height meant auto-grow-height...


    KoRect frameRect( KoUnit::parseValue( object.attribute( "svg:x" ) ),
                      KoUnit::parseValue( object.attribute( "svg:y" ) ),
                      width, height );

    frameElementOut.setAttribute("left", frameRect.left() );
    frameElementOut.setAttribute("right", frameRect.right() );
    frameElementOut.setAttribute("top", frameRect.top() );
    frameElementOut.setAttribute("bottom", frameRect.bottom() );
    if ( hasMinHeight )
        frameElementOut.setAttribute("min-height", height );
    frameElementOut.setAttribute( "z-index", object.attribute( "draw:z-index" ) );
    QPair<int, QString> attribs = Conversion::importWrapping( m_styleStack.attribute( "style:wrap" ) );
    frameElementOut.setAttribute("runaround", attribs.first );
    if ( !attribs.second.isEmpty() )
        frameElementOut.setAttribute("runaroundSide", attribs.second );
    // ## runaroundGap is a problem. KWord has one value, OO has 4 (margins on all sides, see p98).
    // => TODO, implement 4 values in KWord.

    // Not implemented in KWord: contour wrapping
    if ( isText )
        frameElementOut.setAttribute("autoCreateNewFrame", overflowBehavior);
    // TODO newFrameBehavior (i.e. behavior on new page). Not in OO/OASIS, let's have our own attrib.
    // TODO copy: ditto
    // TODO sheetSide (not implemented in KWord, but in its DTD)

    if ( paddingLeft != 0 )
        frameElementOut.setAttribute( "bleftpt", paddingLeft );
    if ( paddingRight != 0 )
        frameElementOut.setAttribute( "brightpt", paddingRight );
    if ( paddingTop != 0 )
        frameElementOut.setAttribute( "btoppt", paddingTop );
    if ( paddingBottom != 0 )
        frameElementOut.setAttribute( "bbottompt", paddingBottom );

    if ( transparent )
        frameElementOut.setAttribute( "bkStyle", 0 );
    else if ( bgColor.isValid() ) {
        frameElementOut.setAttribute( "bkStyle", 1 );
        // TODO the OO format doesn't support fill patterns (bkStyle)
        // To implement this in an OO-like way, we'd need to
        // 1) convert the Qt fill patterns to draw:hatch elements (in office:styles)
        // 2) refer to those using draw:fill="hatch" draw:fill-hatch-name="..."
        // (OASIS extension requested, 17/01/2004)
        frameElementOut.setAttribute( "bkRed", bgColor.red() );
        frameElementOut.setAttribute( "bkBlue", bgColor.blue() );
        frameElementOut.setAttribute( "bkGreen", bgColor.green() );
    }
}

QString OoWriterImport::appendTextBox(QDomDocument& doc, const QDomElement& object)
{
    const QString frameName ( object.attribute("draw:name") ); // ### TODO: what if empty, i.e. non-unique
    kdDebug() << "appendTextBox " << frameName << endl;
    m_styleStack.save();
    fillStyleStack( object, "draw:style-name" ); // get the style for the graphics element

    // Create KWord frameset
    QDomElement framesetElement(doc.createElement("FRAMESET"));
    framesetElement.setAttribute("frameType",1);
    framesetElement.setAttribute("frameInfo",0);
    framesetElement.setAttribute("visible",1);
    framesetElement.setAttribute("name",frameName);
    QDomElement framesetsPluralElement (doc.documentElement().namedItem("FRAMESETS").toElement());
    framesetsPluralElement.appendChild(framesetElement);

    QDomElement frameElementOut(doc.createElement("FRAME"));
    framesetElement.appendChild(frameElementOut);
    importFrame( frameElementOut, object, true /*text*/ );
    // TODO editable

    // We're done with the graphics style
    m_styleStack.restore();

    // Obey draw:text-style-name
    if ( m_styleStack.hasAttribute( "draw:text-style-name" ) )
        addStyles( m_styles[m_styleStack.attribute( "draw:text-style-name" )] );

    // Parse contents
    parseBodyOrSimilar( doc, object, framesetElement );

    return frameName;
}

QString OoWriterImport::appendPicture(QDomDocument& doc, const QDomElement& object)
{
    const QString frameName ( object.attribute("draw:name") ); // ### TODO: what if empty, i.e. non-unique
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
            return frameName; // Should not happen

        const KArchiveEntry* entry = m_zip->directory()->entry( filename );
        if (!entry)
        {
            kdWarning(30518) << "Picture " << filename << " not found!" << endl;
            return frameName;
        }
        if (entry->isDirectory())
        {
            kdWarning(30518) << "Picture " << filename << " is a directory!" << endl;
            return frameName;
        }
        const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
        QIODevice* io=f->device();
        kdDebug(30518) << "Picture " << filename << " has size " << f->size() << endl;

        if (!io)
        {
            kdWarning(30518) << "No QIODevice for picture  " << frameName << " " << href << endl;
            return frameName;
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
            return frameName;
        }
        if (!picture.save(out))
        kdWarning(30518) << "Cannot save picture: " << frameName << " " << href << endl;
        out->close();
    }
    else
    {
         kdWarning(30518) << "Cannot store picture: " << frameName << " " << href << endl;
         return frameName;
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
    framesetElement.appendChild(frameElementOut);

    m_styleStack.save();
    fillStyleStack( object, "draw:style-name" ); // get the style for the graphics element
    importFrame( frameElementOut, object, false /*not text*/ );
    m_styleStack.restore();

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
    return frameName;
}

void OoWriterImport::anchorFrameset( QDomDocument& doc, QDomElement& formats, uint pos, const QString& frameSetName )
{
    QDomElement formatElementOut=doc.createElement("FORMAT");
    formatElementOut.setAttribute("id",6); // Floating frame
    formatElementOut.setAttribute("pos",pos); // Start position
    formatElementOut.setAttribute("len",1); // Start position
    formats.appendChild(formatElementOut); //Append to <FORMATS>

    QDomElement anchor=doc.createElement("ANCHOR");
    // No name attribute!
    anchor.setAttribute("type","frameset");
    anchor.setAttribute("instance",frameSetName);
    formatElementOut.appendChild(anchor);
}

void OoWriterImport::appendField(QDomDocument& doc, QDomElement& outputFormats, const QDomElement& object, uint pos)
// Note: QDomElement& outputFormats replaces the parameter QDomElement& e in OoImpressImport::appendField
//  (otherwise it should be the same parameters.)
{
    const QString tag (object.tagName());

    if (tag == "text:date")
    {
        QDateTime dt(QDate::fromString(object.attribute("text:date-value"), Qt::ISODate));

        bool fixed = (object.hasAttribute("text:fixed") && object.attribute("text:fixed")=="true");

        if (!dt.isValid())
        {
            dt = QDateTime::currentDateTime(); // OOo docs say so :)
            fixed = false;
        }
        const QDate date(dt.date());
        const QTime time(dt.time());
        QDomElement dateElement ( doc.createElement("DATE") );
        dateElement.setAttribute("fix", fixed ? 1 : 0);
        dateElement.setAttribute("day", date.day());
        dateElement.setAttribute("month", date.month());
        dateElement.setAttribute("year", date.year());
        dateElement.setAttribute("hour", time.hour());
        dateElement.setAttribute("minute", time.minute());
        dateElement.setAttribute("second", time.second());
        if (object.hasAttribute("text:date-adjust"))
            dateElement.setAttribute("correct", object.attribute("text:date-adjust"));
        appendKWordVariable(doc, outputFormats, object, pos, "DATE0locale", 0, dateElement);

    }
    else if (tag == "text:time")
    {
        // Use QDateTime to work around a possible problem of QTime::FromString in Qt 3.2.2
        QDateTime dt(QDateTime::fromString(object.attribute("text:time-value"), Qt::ISODate));

        bool fixed = (object.hasAttribute("text:fixed") && object.attribute("text:fixed")=="true");

        if (!dt.isValid()) {
            dt = QDateTime::currentDateTime(); // OOo docs say so :)
            fixed = false;
        }

        const QTime time(dt.time());
        QDomElement timeElement (doc.createElement("TIME") );
        timeElement.setAttribute("fix", fixed ? 1 : 0);
        timeElement.setAttribute("hour", time.hour());
        timeElement.setAttribute("minute", time.minute());
        timeElement.setAttribute("second", time.second());
        /*if (object.hasAttribute("text:time-adjust"))
          timeElem.setAttribute("correct", object.attribute("text:time-adjust"));*/ // ### TODO
        appendKWordVariable(doc, outputFormats, object, pos, "TIMElocale", 2, timeElement);

    }
    else if (tag == "text:page-number")
    {
        int subtype = 0;        // VST_PGNUM_CURRENT

        if (object.hasAttribute("text:select-page"))
        {
            const QString select = object.attribute("text:select-page");

            if (select == "previous")
                subtype = 3;    // VST_PGNUM_PREVIOUS
            else if (select == "next")
                subtype = 4;    // VST_PGNUM_NEXT
            else
                subtype = 0;    // VST_PGNUM_CURRENT
        }

        QDomElement pgnumElement ( doc.createElement("PGNUM") );
        pgnumElement.setAttribute("subtype", subtype);
        pgnumElement.setAttribute("value", object.text());
        appendKWordVariable(doc, outputFormats, object, pos, "NUMBER", 4, pgnumElement);
    }
    else if (tag == "text:file-name")
    {
        int subtype = 5;

        if (object.hasAttribute("text:display"))
        {
            const QString display = object.attribute("text:display");

            if (display == "path")
                subtype = 1;    // VST_DIRECTORYNAME
            else if (display == "name")
                subtype = 6;    // VST_FILENAMEWITHOUTEXTENSION
            else if (display == "name-and-extension")
                subtype = 0;    // VST_FILENAME
            else
                subtype = 5;    // VST_PATHFILENAME
        }

        QDomElement fieldElement ( doc.createElement("FIELD") );
        fieldElement.setAttribute("subtype", subtype);
        fieldElement.setAttribute("value", object.text());
        appendKWordVariable(doc, outputFormats, object, pos, "STRING", 8, fieldElement);
    }
    else if (tag == "text:author-name"
             || tag == "text:author-initials")
    {
        int subtype = 2;        // VST_AUTHORNAME

        if (tag == "text:author-initials")
            subtype = 16;       // VST_INITIAL

        QDomElement authorElem = doc.createElement("FIELD");
        authorElem.setAttribute("subtype", subtype);
        authorElem.setAttribute("value", object.text());
        appendKWordVariable(doc, outputFormats, object, pos, "STRING", 8, authorElem);
    }
}

void OoWriterImport::appendKWordVariable(QDomDocument& doc, QDomElement& formats, const QDomElement& object, uint pos,
    const QString& key, int type, QDomElement& child)
{
    QDomElement variableElement ( doc.createElement("VARIABLE") );

    QDomElement typeElement ( doc.createElement("TYPE") );
    typeElement.setAttribute("key",key);
    typeElement.setAttribute("type",type);
    typeElement.setAttribute("text",object.text());
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
    QString tableName ( parent.attribute("table:name") ); // TODO: what if empty (non-unique?)
    kdDebug(30518) << "Found table " << tableName << endl;

    // In OOWriter a table is never inside a paragraph, in KWord it is always in a paragraph
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

    // ### FIXME: we have no <LAYOUT> element!

    QDomElement elementAnchor(doc.createElement("ANCHOR"));
    elementAnchor.setAttribute("type","frameset");
    elementAnchor.setAttribute("instance",tableName);
    elementFormat.appendChild(elementAnchor);


    const QDomNodeList columnStyles ( parent.elementsByTagName( "table:table-column" ));

    // Left position of the cell/column (similar to RTF's \cellx). The last one defined is the right position of the last cell/column
    QMemArray<double> columnLefts(4);
    uint maxColumns=columnLefts.size();

    uint col=0;
    columnLefts[0]=0.0; // Initialize left of first cell
    for (uint i=0; i<columnStyles.length(); i++)
    {
        const QDomElement elem ( columnStyles.item(i).toElement() );
        uint repeat = elem.attribute("table:number-columns-repeated", "1").toUInt(); // Default 1 time
        if (!repeat)
            repeat=1; // At least one column defined!
        const QString styleName ( elem.attribute("table:style-name") );
        kdDebug(30518) << "Column " << col << " style " << styleName << endl;
        const QDomElement* style=m_styles.find(styleName);
        double width=0.0;
        if (style)
        {
            const QDomElement elemProps( style->namedItem("style:properties").toElement() );
            if (elemProps.isNull())
            {
                kdWarning(30518) << "Could not find table column style properties!" << endl;
            }
            const QString strWidth ( elemProps.attribute("style:column-width") );
            kdDebug(30518) << "- raw style width " << strWidth << endl;
            width = KoUnit::parseValue( strWidth );
        }
        else
            kdWarning(30518) << "Could not find table column style!" << endl;

        if (width < 1.0) // Something is wrong with the width
        {
            kdWarning(30518) << "Table column width ridiculous, assuming 1 inch!" << endl;
            width=72.0;
        }
        else
            kdDebug(30518) << "- style width " << width << endl;

        for (uint j=0; j<repeat; j++)
        {
            ++col;
            if (col>=maxColumns)
            {
                // We need more columns
                maxColumns+=4;
                columnLefts.resize(maxColumns, QGArray::SpeedOptim);
            }
            columnLefts.at(col) = width + columnLefts.at(col-1);
            kdDebug(30518) << "Cell column " << col-1 << " left " << columnLefts.at(col-1) << " right " << columnLefts.at(col) << endl;
        }
    }

    uint row=0;
    uint column=0;
    parseInsideOfTable(doc, parent, currentFramesetElement, tableName, columnLefts, row, column);
}

void OoWriterImport::parseInsideOfTable( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement,
    const QString& tableName, const QMemArray<double> & columnLefts, uint& row, uint& column )
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
                .arg(row).arg(column).arg(tableName)); // The table name could have a % sequence, so use the table name as last!
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
            frameElementOut.setAttribute("left",columnLefts.at(column));
            frameElementOut.setAttribute("right",columnLefts.at(column+1));
            frameElementOut.setAttribute("top", 0);
            frameElementOut.setAttribute("bottom", 0);
            frameElementOut.setAttribute("runaround",1);
            frameElementOut.setAttribute("autoCreateNewFrame",0); // Very important for cell growing!
            // ### TODO: a few attributes are missing
            framesetElement.appendChild(frameElementOut);

            parseBodyOrSimilar( doc, t, framesetElement); // We change the frameset!
            column++;
        }
        else if ( name == "table:table-row" )
        {
            column=0;
            parseInsideOfTable( doc, t, currentFramesetElement, tableName, columnLefts, row, column);
            row++;
        }
        else if ( name == "table:table-header-rows" ) // Provisory (###TODO)
        {
            parseInsideOfTable( doc, t, currentFramesetElement, tableName, columnLefts, row, column);
        }
        else if (name == "table:table-column")
        {
            // Allready treated in OoWriterImport::parseTable, we do not need to do anything here!
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
