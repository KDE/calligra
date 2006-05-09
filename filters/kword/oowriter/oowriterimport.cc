/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2003 David Faure <faure@kde.org>
   Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qcolor.h>
#include <qfile.h>
#include <qfont.h>
#include <qpen.h>
#include <qregexp.h>
#include <qimage.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3MemArray>

#include "oowriterimport.h"
#include <ooutils.h>

#include <kdeversion.h>
#include <kdebug.h>
#include <kzip.h>

#include <KoDocumentInfo.h>
#include <KoDocument.h>

#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <KoFilterChain.h>
#include <KoUnit.h>
#include <KoPageLayout.h>
#include <KoPicture.h>
#include "conversion.h"
#include <KoRect.h>
#include <KoDom.h>


typedef KGenericFactory<OoWriterImport> OoWriterImportFactory;
K_EXPORT_COMPONENT_FACTORY( liboowriterimport, OoWriterImportFactory(  "kofficefilters" ) )

OoWriterImport::OoWriterImport( QObject* parent, const QStringList & )
  : KoFilter(parent),
    m_styleStack( ooNS::style, ooNS::fo ),
    m_insideOrderedList( false ), m_nextItemIsListItem( false ),
    m_hasTOC( false ), m_hasHeader( false ), m_hasFooter( false ), m_restartNumbering( -1 ),
    m_pictureNumber(0), m_zip(NULL)
{
    m_styles.setAutoDelete( true );
    m_masterPages.setAutoDelete( true );
    m_listStyles.setAutoDelete( true );
}

OoWriterImport::~OoWriterImport()
{
}

KoFilter::ConversionStatus OoWriterImport::convert( QByteArray const & from, QByteArray const & to )
{
    kDebug(30518) << "Entering Oowriter Import filter: " << from << " - " << to << endl;

    if ( ( from != "application/vnd.sun.xml.writer"
         && from != "application/vnd.sun.xml.writer.template"
         && from != "application/vnd.sun.xml.writer.master" )
         || to != "application/x-kword" )
    {
        kWarning(30518) << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    m_zip=new KZip(m_chain->inputFile());

    kDebug(30518) << "Store created" << endl;

    if ( !m_zip->open(QIODevice::ReadOnly) )
    {
        kError(30518) << "Couldn't open the requested file "<< m_chain->inputFile() << endl;
        return KoFilter::FileNotFound;
    }

    if ( !m_zip->directory() )
    {
        kError(30518) << "Couldn't read ZIP directory of the requested file "<< m_chain->inputFile() << endl;
        return KoFilter::FileNotFound;
    }


    KoFilter::ConversionStatus preStatus = openFile();

    QImage thumbnail;
    if ( preStatus == KoFilter::OK )
    {
        // We do not care about the failure
        OoUtils::loadThumbnail( thumbnail, m_zip );
    }

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

    // Load styles from style.xml
    if ( !createStyleMap( m_stylesDoc, mainDocument ) )
        return KoFilter::UserCancelled;
    // Also load styles from content.xml
    if ( !createStyleMap( m_content, mainDocument ) )
        return KoFilter::UserCancelled;

    // Create main frameset
    QDomElement mainFramesetElement = mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("visible",1);
    mainFramesetElement.setAttribute("name", i18n( "Main Text Frameset" ) );
    framesetsElem.appendChild(mainFramesetElement);

    createInitialFrame( mainFramesetElement, 29, 798, 42, 566, false, Reconnect );
    createStyles( mainDocument );
    createDocumentContent( mainDocument, mainFramesetElement );
    finishDocumentContent( mainDocument );

    m_zip->close();
    delete m_zip; // It has to be so late, as pictures might be read.

    KoStoreDevice* out = m_chain->storageFile( "maindoc.xml", KoStore::Write );
    if ( !out ) {
        kError(30518) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    else
    {
        QByteArray cstr = mainDocument.toByteArray();
        kDebug(30518)<<" maindoc: " << cstr << endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->write( cstr, cstr.length() );
    }

    QDomDocument docinfo;
    createDocumentInfo( docinfo );

    // store document info
    out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if( out )
    {
        QByteArray info = docinfo.toByteArray();
        kDebug(30518)<<" info :"<<info<<endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->write( info , info.length() );
    }

    // store preview

    if ( ! thumbnail.isNull() )
    {
        // ### TODO: thumbnail.setAlphaBuffer( false ); // legacy KOffice previews have no alpha channel
        // Legacy KOffice previews are 256x256x8 instead of 128x128x32
        QImage preview( thumbnail.smoothScale( 256, 256 ).convertDepth(8, Qt::AvoidDither | Qt::DiffuseDither) );
        // Not to be able to generate a preview is not an error
        if ( !preview.isNull() )
        {
            out = m_chain->storageFile( "preview.png", KoStore::Write );
            if( out )
            {
                preview.save( out, "PNG" );
            }
        }
    }

    kDebug(30518) << "######################## OoWriterImport::convert done ####################" << endl;
    return KoFilter::OK;
}

void OoWriterImport::createStyles( QDomDocument& doc )
{
    QDomElement stylesElem = doc.createElement( "STYLES" );
    doc.documentElement().appendChild( stylesElem );

    QDomNode fixedStyles = KoDom::namedItemNS( m_stylesDoc.documentElement(), ooNS::office, "styles" );
    Q_ASSERT( !fixedStyles.isNull() );
    QDomElement e;
    forEachElement( e, fixedStyles )
    {
        if ( !e.hasAttributeNS( ooNS::style, "name" ) )
            continue;
        // We only generate paragraph styles for now
        if ( e.attributeNS( ooNS::style, "family", QString::null ) != "paragraph" )
             continue;

        // We use the style stack, to flatten out parent styles
        // Once KWord supports style inheritance, replace this with a single m_styleStack.push.
        // (We still need to use StyleStack, since that's what writeLayout/writeFormat read from)
        addStyles( &e );

        QDomElement styleElem = doc.createElement("STYLE");
        stylesElem.appendChild( styleElem );

        QString styleName = kWordStyleName( e.attributeNS( ooNS::style, "name", QString::null ) );
        QDomElement element = doc.createElement("NAME");
        element.setAttribute( "value", styleName );
        styleElem.appendChild( element );
        //kDebug(30518) << k_funcinfo << "generating style " << styleName << endl;

        QString followingStyle = m_styleStack.attributeNS( ooNS::style, "next-style-name" );
        if ( !followingStyle.isEmpty() )
        {
            QDomElement element = doc.createElement( "FOLLOWING" );
            element.setAttribute( "name", kWordStyleName( followingStyle ) );
            styleElem.appendChild( element );
        }

        // ### In KWord the style says "I'm part of the outline" (TOC)
        // ### In OOo the paragraph says that (text:h)
        // Hence this hack...
        // OASIS solution for this: style:default-outline-level attribute
        bool outline = styleName.startsWith( "Heading" );
        if ( outline )
            styleElem.setAttribute( "outline", "true" );

        writeFormat( doc, styleElem, 1, 0, 0 );
        writeLayout( doc, styleElem );

        // writeLayout doesn't load the counter. It's modelled differently for parags and for styles.
        // ### missing info in the format! (fixed in OASIS)
        const int level = styleName.right(1).toInt(); // ## HACK
        bool listOK = false;
        if ( level > 0 ) {
            if ( outline )
                listOK = pushListLevelStyle( "<outline-style>", m_outlineStyle, level );
            else {
                const QString listStyleName = e.attributeNS( ooNS::style, "list-style-name", QString::null );
                listOK = !listStyleName.isEmpty();
                if ( listOK )
                    listOK = pushListLevelStyle( listStyleName, level );
            }
        }
        if ( listOK ) {
            const QDomElement listStyle = m_listStyleStack.currentListStyle();
            // The tag is either text:list-level-style-number or text:list-level-style-bullet
            bool ordered = listStyle.localName() == "list-level-style-number";
            writeCounter( doc, styleElem, outline, level, ordered );
            m_listStyleStack.pop();
        }

        m_styleStack.clear();
    }
}

void OoWriterImport::parseBodyOrSimilar( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement )
{
    QDomElement oldCurrentFrameset = m_currentFrameset;
    m_currentFrameset = currentFramesetElement;
    Q_ASSERT( !m_currentFrameset.isNull() );
    QDomElement t;
    forEachElement( t, parent )
    {
        m_styleStack.save();
        const QString localName = t.localName();
        const QString ns = t.namespaceURI();
        const bool isTextNS = ns == ooNS::text;

        QDomElement e;
        if ( isTextNS && localName == "p" ) {  // text paragraph
            fillStyleStack( t, ooNS::text, "style-name" );
            e = parseParagraph( doc, t );
        }
        else if ( isTextNS && localName == "h" ) // heading
        {
            fillStyleStack( t, ooNS::text, "style-name" );
            int level = t.attributeNS( ooNS::text, "level", QString::null ).toInt();
            bool listOK = false;
            // When a heading is inside a list, it seems that the list prevails.
            // Example:
            //    <text:ordered-list text:style-name="Numbering 1">
            //      <text:list-item text:start-value="5">
            //        <text:h text:style-name="P2" text:level="4">The header</text:h>
            // where P2 has list-style-name="something else"
            // Result: the numbering of the header follows "Numbering 1".
            // So we use the style for the outline level only if we're not inside a list:
            if ( !m_nextItemIsListItem )
                listOK = pushListLevelStyle( "<outline-style>", m_outlineStyle, level );
            m_nextItemIsListItem = true;
            if ( t.hasAttributeNS( ooNS::text, "start-value" ) )
                 // OASIS extension http://lists.oasis-open.org/archives/office/200310/msg00033.html
                 m_restartNumbering = t.attributeNS( ooNS::text, "start-value", QString::null ).toInt();
            e = parseParagraph( doc, t );
            if ( listOK )
                m_listStyleStack.pop();
        }
        else if ( isTextNS &&
                  ( localName == "unordered-list" || localName == "ordered-list" ) )
        {
            parseList( doc, t, currentFramesetElement );
            m_styleStack.restore();
            continue;
        }
        else if ( isTextNS && localName == "section" ) // Temporary support (###TODO)
        {
            kDebug(30518) << "Section found!" << endl;
            fillStyleStack( t, ooNS::text, "style-name" );
            parseBodyOrSimilar( doc, t, currentFramesetElement);
        }
        else if ( localName == "table" && ns == ooNS::table )
        {
            kDebug(30518) << "Table found!" << endl;
            parseTable( doc, t, currentFramesetElement );
        }
        else if ( localName == "image" && ns == ooNS::draw )
        {
            appendPicture( doc, t );
        }
        else if ( localName == "text-box" && ns == ooNS::draw )
        {
            appendTextBox( doc, t );
        }
        else if ( isTextNS && localName == "variable-decls" )
        {
            // We don't parse variable-decls since we ignore var types right now
            // (and just storing a list of available var names wouldn't be much use)
        }
        else if ( localName == "table-of-content" && ns == ooNS::text )
        {
            appendTOC( doc, t );
        }
        // TODO text:sequence-decls
        else
        {
            kWarning(30518) << "Unsupported body element '" << localName << "'" << endl;
        }

        if ( !e.isNull() )
            currentFramesetElement.appendChild( e );
        m_styleStack.restore(); // remove the styles added by the paragraph or list
    }
    m_currentFrameset = oldCurrentFrameset; // in case of recursive invokations
}

void OoWriterImport::createDocumentContent( QDomDocument &doc, QDomElement& mainFramesetElement )
{
    QDomElement content = m_content.documentElement();

    QDomElement body ( KoDom::namedItemNS( content, ooNS::office, "body" ) );
    if ( body.isNull() )
    {
        kError(30518) << "No office:body found!" << endl;
        return;
    }

    parseBodyOrSimilar( doc, body, mainFramesetElement );
}

void OoWriterImport::writePageLayout( QDomDocument& mainDocument, const QString& masterPageName )
{
    QDomElement docElement = mainDocument.documentElement();

    kDebug(30518) << "writePageLayout " << masterPageName << endl;
    QDomElement elementPaper = mainDocument.createElement("PAPER");
    KoOrientation orientation;
    double width, height;
    KoFormat paperFormat;
    double marginLeft, marginTop, marginRight, marginBottom;
    bool hasEvenOddHeader = false;
    bool hasEvenOddFooter = false;

    QDomElement* masterPage = m_masterPages[ masterPageName ];
    Q_ASSERT( masterPage );
    kDebug(30518) << "page-master-name: " << masterPage->attributeNS( ooNS::style, "page-master-name", QString::null ) << endl;
    QDomElement *style = masterPage ? m_styles[masterPage->attributeNS( ooNS::style, "page-master-name", QString::null )] : 0;
    Q_ASSERT( style );
    if ( style )
    {
        QDomElement properties( KoDom::namedItemNS( *style, ooNS::style, "properties" ) );
        Q_ASSERT( !properties.isNull() );
        orientation = ( (properties.attributeNS( ooNS::style, "print-orientation", QString::null) != "portrait") ? PG_LANDSCAPE : PG_PORTRAIT );
        width = KoUnit::parseValue(properties.attributeNS( ooNS::fo, "page-width", QString::null));
        height = KoUnit::parseValue(properties.attributeNS( ooNS::fo, "page-height", QString::null));
        kDebug(30518) << "width=" << width << " height=" << height << endl;
        // guessFormat takes millimeters
        if ( orientation == PG_LANDSCAPE )
            paperFormat = KoPageFormat::guessFormat( POINT_TO_MM(height), POINT_TO_MM(width) );
        else
            paperFormat = KoPageFormat::guessFormat( POINT_TO_MM(width), POINT_TO_MM(height) );

        marginLeft = KoUnit::parseValue(properties.attributeNS( ooNS::fo, "margin-left", QString::null));
        marginTop = KoUnit::parseValue(properties.attributeNS( ooNS::fo, "margin-top", QString::null));
        marginRight = KoUnit::parseValue(properties.attributeNS( ooNS::fo, "margin-right", QString::null));
        marginBottom = KoUnit::parseValue(properties.attributeNS( ooNS::fo, "margin-bottom", QString::null));

        QDomElement footnoteSep = KoDom::namedItemNS( properties, ooNS::style, "footnote-sep" );
        if ( !footnoteSep.isNull() ) {
            // style:width="0.018cm" style:distance-before-sep="0.101cm"
            // style:distance-after-sep="0.101cm" style:adjustment="left"
            // style:rel-width="25%" style:color="#000000"
            QString width = footnoteSep.attributeNS( ooNS::style, "width", QString::null );
            elementPaper.setAttribute( "slFootNoteWidth", KoUnit::parseValue( width ) );
            QString pageWidth = footnoteSep.attributeNS( ooNS::style, "rel-width", QString::null );
            if ( pageWidth.endsWith( "%" ) ) {
                pageWidth.truncate( pageWidth.length() - 1 ); // remove '%'
                elementPaper.setAttribute( "slFootNoteLenth", pageWidth );
            }
            elementPaper.setAttribute( "slFootNotePosition", footnoteSep.attributeNS( ooNS::style, "adjustment", QString::null ) );
            // Not in KWord: color, distance before and after separator
            // Not in OOo: line type of separator (solid, dot, dash etc.)
        }


        // Header/Footer
        QDomElement headerStyle = KoDom::namedItemNS( *style, ooNS::style, "header-style" );
        QDomElement footerStyle = KoDom::namedItemNS( *style, ooNS::style, "footer-style" );
        QDomElement headerLeftElem = KoDom::namedItemNS( *masterPage, ooNS::style, "header-left" );
        if ( !headerLeftElem.isNull() ) {
            kDebug(30518) << "Found header-left" << endl;
            hasEvenOddHeader = true;
            importHeaderFooter( mainDocument, headerLeftElem, hasEvenOddHeader, headerStyle );
        }
        QDomElement headerElem = KoDom::namedItemNS( *masterPage, ooNS::style, "header" );
        if ( !headerElem.isNull() ) {
            kDebug(30518) << "Found header" << endl;
            importHeaderFooter( mainDocument, headerElem, hasEvenOddHeader, headerStyle );
        }
        QDomElement footerLeftElem = KoDom::namedItemNS( *masterPage, ooNS::style, "footer-left" );
        if ( !footerLeftElem.isNull() ) {
            kDebug(30518) << "Found footer-left" << endl;
            importHeaderFooter( mainDocument, footerLeftElem, hasEvenOddFooter, footerStyle );
        }
        QDomElement footerElem = KoDom::namedItemNS( *masterPage, ooNS::style, "footer" );
        if ( !footerElem.isNull() ) {
            kDebug(30518) << "Found footer" << endl;
            importHeaderFooter( mainDocument, footerElem, hasEvenOddFooter, footerStyle );
        }
    }
    else
    {
        // We have no master page! We need defaults.
        kWarning(30518) << "NO MASTER PAGE" << endl;
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

    elementPaper.setAttribute("orientation", int(orientation) );
    elementPaper.setAttribute("width", width);
    elementPaper.setAttribute("height", height);
    elementPaper.setAttribute("format", paperFormat);
    elementPaper.setAttribute("columns",1); // TODO
    elementPaper.setAttribute("columnspacing",2); // TODO
    elementPaper.setAttribute("hType", hasEvenOddHeader ? 3 : 0); // ### no support for first-page
    elementPaper.setAttribute("fType", hasEvenOddFooter ? 3 : 0); // ### no support for first-page
    elementPaper.setAttribute("spHeadBody",9); // where is this in OOo?
    elementPaper.setAttribute("spFootBody",9); // ?
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

    // Now create VARIABLESETTINGS, mostly from meta.xml
    QDomElement varSettings = mainDocument.createElement( "VARIABLESETTINGS" );
    docElement.appendChild( varSettings );
    QDomNode meta   = KoDom::namedItemNS( m_meta, ooNS::office, "document-meta" );
    QDomNode office = KoDom::namedItemNS( meta, ooNS::office, "meta" );
    if ( !office.isNull() ) {
        QDomElement date = KoDom::namedItemNS( office, ooNS::dc, "date" );
        if ( !date.isNull() && !date.text().isEmpty() ) {
            // Both use ISO-8601, no conversion needed.
            varSettings.setAttribute( "modificationDate", date.text() );
        }
        date = KoDom::namedItemNS( office, ooNS::meta, "creation-date" );
        if ( !date.isNull() && !date.text().isEmpty() ) {
            varSettings.setAttribute( "creationDate", date.text() );
        }
        date = KoDom::namedItemNS( office, ooNS::meta, "print-date" );
        if ( !date.isNull() && !date.text().isEmpty() ) {
            varSettings.setAttribute( "lastPrintingDate", date.text() );
        }
    }
}

// Copied from the msword importer
QDomElement OoWriterImport::createInitialFrame( QDomElement& parentFramesetElem, double left, double right, double top, double bottom, bool autoExtend, NewFrameBehavior nfb )
{
    QDomElement frameElementOut = parentFramesetElem.ownerDocument().createElement("FRAME");
    frameElementOut.setAttribute( "left", left );
    frameElementOut.setAttribute( "right", right );
    frameElementOut.setAttribute( "top", top );
    frameElementOut.setAttribute( "bottom", bottom );
    frameElementOut.setAttribute( "runaround", 1 );
    // AutoExtendFrame for header/footer/footnote/endnote, AutoCreateNewFrame for body text
    frameElementOut.setAttribute( "autoCreateNewFrame", autoExtend ? 0 : 1 );
    frameElementOut.setAttribute( "newFrameBehavior", nfb );
    parentFramesetElem.appendChild( frameElementOut );
    return frameElementOut;
}

KoFilter::ConversionStatus OoWriterImport::loadAndParse(const QString& filename, QDomDocument& doc)
{
    return OoUtils::loadAndParse( filename, doc, m_zip);
}

KoFilter::ConversionStatus OoWriterImport::openFile()
{
    KoFilter::ConversionStatus status=loadAndParse("content.xml", m_content);
    if ( status != KoFilter::OK )
    {
        kError(30518) << "Content.xml could not be parsed correctly! Aborting!" << endl;
        return status;
    }

    //kDebug(30518)<<" m_content.toCString() :"<<m_content.toCString()<<endl;

    // We need to keep the QDomDocument for styles too, unfortunately.
    // Otherwise styleElement.parentNode() returns a null node
    // (see StyleStack::isUserStyle). Most of styles.xml is in m_styles
    // anyway, so this doesn't make a big difference.
    // We now also rely on this in createStyles.
    //QDomDocument styles;

    // We do not stop if the following calls fail.
    loadAndParse("styles.xml", m_stylesDoc);
    loadAndParse("meta.xml", m_meta);
    // not used yet: loadAndParse("settings.xml", m_settings);

    emit sigProgress( 10 );

    return KoFilter::OK;
}

// Very related to OoImpressImport::createDocumentInfo
void OoWriterImport::createDocumentInfo( QDomDocument &docinfo )
{
    docinfo = KoDocument::createDomDocument( "document-info" /*DTD name*/, "document-info" /*tag name*/, "1.1" );

    OoUtils::createDocumentInfo(m_meta, docinfo);

    //kDebug(30518)<<" meta-info :"<<m_meta.toCString()<<endl;
}

// This mainly fills member variables with the styles.
// The 'doc' argument is only for footnotes-configuration.
bool OoWriterImport::createStyleMap( const QDomDocument & styles, QDomDocument& doc )
{
  QDomElement docElement  = styles.documentElement();
  QDomNode docStyles   = KoDom::namedItemNS( docElement, ooNS::office, "document-styles" );

  if ( docElement.hasAttributeNS( ooNS::office, "version" ) )
  {
    bool ok = true;
    double d = docElement.attributeNS( ooNS::office, "version", QString::null ).toDouble( &ok );

    if ( ok )
    {
      kDebug(30518) << "OpenWriter version: " << d << endl;
      if ( d > 1.0 )
      {
        QString message( i18n("This document was created with OpenOffice.org version '%1'. This filter was written for version 1.0. Reading this file could cause strange behavior, crashes or incorrect display of the data. Do you want to continue converting the document?") );
        message = message.arg( docElement.attributeNS( ooNS::office, "version", QString::null ) );
        if ( KMessageBox::warningYesNo( 0, message, i18n( "Unsupported document version" ) ) == KMessageBox::No )
          return false;
      }
    }
  }

  QDomNode fontStyles = KoDom::namedItemNS( docElement, ooNS::office, "font-decls" );

  if ( !fontStyles.isNull() )
  {
    kDebug(30518) << "Starting reading in font-decl..." << endl;

    insertStyles( fontStyles.toElement(), doc );
  }
  else
    kDebug(30518) << "No items found" << endl;

  kDebug(30518) << "Starting reading in office:automatic-styles" << endl;

  QDomNode autoStyles = KoDom::namedItemNS( docElement, ooNS::office, "automatic-styles" );
  if ( !autoStyles.isNull() )
  {
      insertStyles( autoStyles.toElement(), doc );
  }
  else
    kDebug(30518) << "No items found" << endl;


  kDebug(30518) << "Reading in master styles" << endl;

  QDomNode masterStyles = KoDom::namedItemNS( docElement, ooNS::office, "master-styles" );

  if ( !masterStyles.isNull() )
  {

      QDomElement master;
      forEachElement( master, masterStyles )
      {
          if ( master.localName() ==  "master-page" && master.namespaceURI() == ooNS::style )
          {
              QString name = master.attributeNS( ooNS::style, "name", QString::null );
              kDebug(30518) << "Master style: '" << name << "' loaded " << endl;
              m_masterPages.insert( name, new QDomElement( master ) );
          } else
              kWarning(30518) << "Unknown tag " << master.tagName() << " in office:master-styles" << endl;
      }
  }


  kDebug(30518) << "Starting reading in office:styles" << endl;

  QDomNode fixedStyles = KoDom::namedItemNS( docElement, ooNS::office, "styles" );

  if ( !fixedStyles.isNull() )
    insertStyles( fixedStyles.toElement(), doc );

  kDebug(30518) << "Styles read in." << endl;

  return true;
}

// started as a copy of OoImpressImport::insertStyles
void OoWriterImport::insertStyles( const QDomElement& styles, QDomDocument& doc )
{
    //kDebug(30518) << "Inserting styles from " << styles.tagName() << endl;
    QDomElement e;
    forEachElement( e, styles )
    {
        const QString localName = e.localName();
        const QString ns = e.namespaceURI();

        const QString name = e.attributeNS( ooNS::style, "name", QString::null );
        if ( ns == ooNS::style && (
                localName == "style"
             || localName == "page-master"
             || localName == "font-decl" ) )
        {
            QDomElement* ep = new QDomElement( e );
            m_styles.insert( name, ep );
            kDebug(30518) << "Style: '" << name << "' loaded " << endl;
        } else if ( localName == "default-style" && ns == ooNS::style ) {
            m_defaultStyle = e;
        } else if ( localName == "list-style" && ns == ooNS::text ) {
            QDomElement* ep = new QDomElement( e );
            m_listStyles.insert( name, ep );
            kDebug(30518) << "List style: '" << name << "' loaded " << endl;
        } else if ( localName == "outline-style" && ns == ooNS::text ) {
            m_outlineStyle = e;
        } else if ( localName == "footnotes-configuration" && ns == ooNS::text ) {
            importFootnotesConfiguration( doc, e, false );
        } else if ( localName == "endnotes-configuration" && ns == ooNS::text ) {
            importFootnotesConfiguration( doc, e, true );
        } else if ( localName == "linenumbering-configuration" && ns == ooNS::text ) {
            // Not implemented in KWord
        } else if ( localName == "number-style" && ns == ooNS::number ) {
            // TODO
        } else if ( ( localName == "date-style"
                      || localName == "time-style" ) && ns == ooNS::number ) {
            importDateTimeStyle( e );
        } else {
            kWarning(30518) << "Unknown element " << localName << " in styles" << endl;
        }
    }
}


// OO spec 2.5.4. p68. Conversion to Qt format: see qdate.html
// OpenCalcImport::loadFormat has similar code, but slower, intermixed with other stuff,
// lacking long-textual forms.
void OoWriterImport::importDateTimeStyle( const QDomElement& parent )
{
    QString format;
    QDomElement e;
    forEachElement( e, parent )
    {
        const QString ns = e.namespaceURI();
        if ( ns != ooNS::number )
            continue;
        const QString localName = e.localName();
        const QString numberStyle = e.attributeNS( ooNS::number, "style", QString::null );
        const bool shortForm = numberStyle == "short" || numberStyle.isEmpty();
        if ( localName == "day" ) {
            format += shortForm ? "d" : "dd";
        } else if ( localName == "day-of-week" ) {
            format += shortForm ? "ddd" : "dddd";
        } else if ( localName == "month" ) {
            // TODO the spec has a strange mention of number:format-source
            if ( e.attributeNS( ooNS::number, "textual", QString::null ) == "true" ) {
                format += shortForm ? "MMM" : "MMMM";
            } else { // month number
                format += shortForm ? "M" : "MM";
            }
        } else if ( localName == "year" ) {
            format += shortForm ? "yy" : "yyyy";
        } else if ( localName == "week-of-year" || localName == "quarter") {
            // ### not supported in Qt
        } else if ( localName == "hours" ) {
            format += shortForm ? "h" : "hh";
        } else if ( localName == "minutes" ) {
            format += shortForm ? "m" : "mm";
        } else if ( localName == "seconds" ) {
            format += shortForm ? "s" : "ss";
        } else if ( localName == "am-pm" ) {
            format += "ap";
        } else if ( localName == "text" ) { // litteral
            format += e.text();
        } // TODO number:decimal-places
    }

#if 0
    // QDate doesn't work both ways!!! It can't parse something back from
    // a string and a format (e.g. 01/02/03 and dd/MM/yy, it will assume MM/dd/yy).
    // So we also need to generate a KLocale-like format, to parse the value
    // Update: we don't need to parse the date back.

    QString kdeFormat;
    QDomElement e;
    forEachElement( e, parent )
    {
        const QString ns = e.namespaceURI();
        if ( ns != ooNS::number )
            continue;
        QString localName = e.tagName();
        const QString numberStyle = e.attributeNS( ooNS::number, "style", QString::null );
        const bool shortForm = numberStyle == "short" || numberStyle.isEmpty();
        if ( localName == "day" ) {
            kdeFormat += shortForm ? "%e" : "%d";
        } else if ( localName == "day-of-week" ) {
            kdeFormat += shortForm ? "%a" : "%A";
        } else if ( localName == "month" ) {
            // TODO the spec has a strange mention of number:format-source
            if ( e.attributeNS( ooNS::number, "textual", QString::null ) == "true" ) {
                kdeFormat += shortForm ? "%b" : "%B";
            } else { // month number
                kdeFormat += shortForm ? "%n" : "%m";
            }
        } else if ( localName == "year" ) {
            kdeFormat += shortForm ? "%y" : "%Y";
        } else if ( localName == "week-of-year" || localName == "quarter") {
            // ### not supported in KLocale
        } else if ( localName == "hours" ) {
            kdeFormat += shortForm ? "%k" : "%H"; // TODO should depend on presence of am/pm
        } else if ( localName == "minutes" ) {
            kdeFormat += shortForm ? "%M" : "%M"; // KLocale doesn't have 1-digit minutes
        } else if ( localName == "seconds" ) {
            kdeFormat += shortForm ? "%S" : "%S"; // KLocale doesn't have 1-digit seconds
        } else if ( localName == "am-pm" ) {
            kdeFormat += "%p";
        } else if ( localName == "text" ) { // litteral
            kdeFormat += e.text();
        } // TODO number:decimal-places
    }
#endif

    QString styleName = parent.attributeNS( ooNS::style, "name", QString::null );
    kDebug(30518) << "datetime style: " << styleName << " qt format=" << format << endl;
    m_dateTimeFormats.insert( styleName, format );
}

void OoWriterImport::fillStyleStack( const QDomElement& object, const char* nsURI, const QString& attrName )
{
    // find all styles associated with an object and push them on the stack
    // OoImpressImport has more tests here, but I don't think they're relevant to OoWriterImport
    if ( object.hasAttributeNS( nsURI, attrName ) ) {
        const QString styleName = object.attributeNS( nsURI, attrName, QString::null );
        const QDomElement* style = m_styles[styleName];
        if ( style )
            addStyles( style );
        else
            kWarning(30518) << "fillStyleStack: no style named " << styleName << " found." << endl;
    }
}

void OoWriterImport::addStyles( const QDomElement* style )
{
    Q_ASSERT( style );
    if ( !style ) return;
    // this recursive function is necessary as parent styles can have parents themselves
    if ( style->hasAttributeNS( ooNS::style, "parent-style-name" ) ) {
        const QString parentStyleName = style->attributeNS( ooNS::style, "parent-style-name", QString::null );
        QDomElement* parentStyle = m_styles[ parentStyleName ];
        if ( parentStyle )
            addStyles( parentStyle );
        else
            kWarning(30518) << "Parent style not found: " << parentStyleName << endl;
    }
    else if ( !m_defaultStyle.isNull() ) // on top of all, the default style
        m_styleStack.push( m_defaultStyle );

    //kDebug(30518) << "pushing style " << style->attributeNS( ooNS::style, "name", QString::null ) << endl;
    m_styleStack.push( *style );
}

void OoWriterImport::applyListStyle( QDomDocument& doc, QDomElement& layoutElement, const QDomElement& paragraph )
{
    // Spec: see 3.3.5 p137
    if ( m_listStyleStack.hasListStyle() && m_nextItemIsListItem ) {
        bool heading = paragraph.localName() == "h";
        m_nextItemIsListItem = false;
        int level = heading ? paragraph.attributeNS( ooNS::text, "level", QString::null ).toInt() : m_listStyleStack.level();
        writeCounter( doc, layoutElement, heading, level, m_insideOrderedList );
    }
}

void OoWriterImport::writeCounter( QDomDocument& doc, QDomElement& layoutElement, bool heading, int level, bool ordered )
{
    const QDomElement listStyle = m_listStyleStack.currentListStyle();
    //const QDomElement listStyleProperties = m_listStyleStack.currentListStyleProperties();
    QDomElement counter = doc.createElement( "COUNTER" );
    counter.setAttribute( "numberingtype", heading ? 1 : 0 );
    counter.setAttribute( "depth", level - 1 ); // "depth" starts at 0

    //kDebug(30518) << "Numbered parag. heading=" << heading << " level=" << level
    //               << " m_restartNumbering=" << m_restartNumbering << endl;

    if ( ordered || heading ) {
        counter.setAttribute( "type", Conversion::importCounterType( listStyle.attributeNS( ooNS::style, "num-format", QString::null ) ) );
        counter.setAttribute( "lefttext", listStyle.attributeNS( ooNS::style, "num-prefix", QString::null ) );
        counter.setAttribute( "righttext", listStyle.attributeNS( ooNS::style, "num-suffix", QString::null ) );
        QString dl = listStyle.attributeNS( ooNS::text, "display-levels", QString::null );
        if ( dl.isEmpty() )
            dl = "1";
        counter.setAttribute( "display-levels", dl );
        if ( m_restartNumbering != -1 ) {
            counter.setAttribute( "start", m_restartNumbering );
            counter.setAttribute( "restart", "true" );
        } else {
            // useful?
            counter.setAttribute( "start", listStyle.attributeNS( ooNS::text, "start-value", QString::null ) );
        }
    }
    else { // bullets, see 3.3.6 p138
        counter.setAttribute( "type", 6 );
        QString bulletChar = listStyle.attributeNS( ooNS::text, "bullet-char", QString::null );
        if ( !bulletChar.isEmpty() ) {
#if 0 // doesn't work well. Fonts lack those symbols!
            counter.setAttribute( "bullet", bulletChar[0].unicode() );
            kDebug(30518) << "bullet code " << bulletChar[0].unicode() << endl;
            QString fontName = listStyleProperties.attributeNS( ooNS::style, "font-name", QString::null );
            counter.setAttribute( "bulletfont", fontName );
#endif
            // Reverse engineering, I found those codes:
            switch( bulletChar[0].unicode() ) {
            case 0x2022: // small disc
                counter.setAttribute( "type", 10 ); // a disc bullet
                break;
            case 0x25CF: // large disc
                counter.setAttribute( "type", 10 ); // a disc bullet
                break;
            case 0xE00C: // losange - TODO in KWord. Not in OASIS either (reserved Unicode area!)
                counter.setAttribute( "type", 10 ); // a disc bullet
                break;
            case 0xE00A: // square. Not in OASIS (reserved Unicode area!)
                counter.setAttribute( "type", 9 );
                break;
            case 0x2794: // arrow
            case 0x27A2: // two-colors right-pointing triangle (TODO)
                counter.setAttribute( "bullet", 206 ); // simpler arrow symbol
                counter.setAttribute( "bulletfont", "symbol" );
                break;
            case 0x2717: // cross
                counter.setAttribute( "bullet", 212 ); // simpler cross symbol
                counter.setAttribute( "bulletfont", "symbol" );
                break;
            case 0x2714: // checkmark
                counter.setAttribute( "bullet", 246 ); // hmm that's sqrt
                counter.setAttribute( "bulletfont", "symbol" );
                break;
            default:
                counter.setAttribute( "type", 8 ); // circle
                break;
            }
        } else { // can never happen
            counter.setAttribute( "type", 10 ); // a disc bullet
        }
    }

    layoutElement.appendChild(counter);
}

static QDomElement findListLevelStyle( QDomElement& fullListStyle, int level )
{
    QDomElement listLevelItem;
    forEachElement( listLevelItem, fullListStyle )
    {
       if ( listLevelItem.attributeNS( ooNS::text, "level", QString::null ).toInt() == level )
           return listLevelItem;
    }
    return QDomElement();
}

bool OoWriterImport::pushListLevelStyle( const QString& listStyleName, int level )
{
    QDomElement* fullListStyle = m_listStyles[listStyleName];
    if ( !fullListStyle ) {
        kWarning(30518) << "List style " << listStyleName << " not found!" << endl;
        return false;
    }
    else
        return pushListLevelStyle( listStyleName, *fullListStyle, level );
}

bool OoWriterImport::pushListLevelStyle( const QString& listStyleName, // for debug only
                                         QDomElement& fullListStyle, int level )
{
    // Find applicable list-level-style for level
    int i = level;
    QDomElement listLevelStyle;
    while ( i > 0 && listLevelStyle.isNull() ) {
        listLevelStyle = findListLevelStyle( fullListStyle, i );
        --i;
    }
    if ( listLevelStyle.isNull() ) {
        kWarning(30518) << "List level style for level " << level << " in list style " << listStyleName << " not found!" << endl;
        return false;
    }
    kDebug(30518) << "Pushing list-level-style from list-style " << listStyleName << " level " << level << endl;
    m_listStyleStack.push( listLevelStyle );
    return true;
}

void OoWriterImport::parseList( QDomDocument& doc, const QDomElement& list, QDomElement& currentFramesetElement )
{
    //kDebug(30518) << k_funcinfo << "parseList"<< endl;

    m_insideOrderedList = ( list.localName() == "ordered-list" );
    QString oldListStyleName = m_currentListStyleName;
    if ( list.hasAttributeNS( ooNS::text, "style-name" ) )
        m_currentListStyleName = list.attributeNS( ooNS::text, "style-name", QString::null );
    bool listOK = !m_currentListStyleName.isEmpty();
    const int level = m_listStyleStack.level() + 1;
    //kDebug(30518) << k_funcinfo << " listOK=" << listOK << " level=" << level << endl;
    if ( listOK )
        listOK = pushListLevelStyle( m_currentListStyleName, level );

    // Iterate over list items
    QDomElement listItem;
    forEachElement( listItem, list )
    {
        // It's either list-header (normal text on top of list) or list-item
        m_nextItemIsListItem = ( listItem.localName() != "list-header" );
        m_restartNumbering = -1;
        if ( listItem.hasAttributeNS( ooNS::text, "start-value" ) )
            m_restartNumbering = listItem.attributeNS( ooNS::text, "start-value", QString::null ).toInt();
        // ### Oasis: can be p h or list only.
        parseBodyOrSimilar( doc, listItem, currentFramesetElement );
        m_restartNumbering = -1;
    }
    if ( listOK )
        m_listStyleStack.pop();
    m_currentListStyleName = oldListStyleName;
}

static int numberOfParagraphs( const QDomElement& frameset )
{
    const QDomNodeList children = frameset.childNodes();
    const QString paragStr = "PARAGRAPH";
    int paragCount = 0;
    for ( unsigned int i = 0 ; i < children.length() ; ++i ) {
        if ( children.item( i ).toElement().tagName() == paragStr )
            ++paragCount;
    }
    return paragCount;
}

void OoWriterImport::parseSpanOrSimilar( QDomDocument& doc, const QDomElement& parent,
    QDomElement& outputParagraph, QDomElement& outputFormats, QString& paragraphText, uint& pos)
{
    // Parse every child node of the parent
    // Can't use forEachElement here since we also care about text nodes
    for( QDomNode node ( parent.firstChild() ); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement ts ( node.toElement() );
        QString textData;
        const QString localName( ts.localName() );
        const QString ns = ts.namespaceURI();
        const bool isTextNS = ns == ooNS::text;
        QDomText t ( node.toText() );

        bool shouldWriteFormat=false; // By default no <FORMAT> element should be written

        // Try to keep the order of the tag names by probability of happening
        if ( isTextNS && localName == "span" ) // text:span
        {
            m_styleStack.save();
            fillStyleStack( ts, ooNS::text, "style-name" );
            parseSpanOrSimilar( doc, ts, outputParagraph, outputFormats, paragraphText, pos);
            m_styleStack.restore();
        }
        else if ( isTextNS && localName == "s" ) // text:s
        {
            textData = OoUtils::expandWhitespace(ts);
            shouldWriteFormat=true;
        }
        else if ( isTextNS && localName == "tab-stop" ) // text:tab-stop
        {
            // KWord currently uses \t.
            // Known bug: a line with only \t\t\t\t isn't loaded - XML (QDom) strips out whitespace.
            // One more good reason to switch to <text:tab-stop> instead...
            textData = '\t';
            shouldWriteFormat=true;
        }
        else if ( isTextNS && localName == "line-break" )
        {
            textData = '\n';
            shouldWriteFormat=true;
        }
        else if ( isTextNS &&
                  ( localName == "footnote" || localName == "endnote" ) )
        {
            textData = '#'; // anchor placeholder
            importFootnote( doc, ts, outputFormats, pos, localName );
        }
        else if ( localName == "image" && ns == ooNS::draw )
        {
            textData = '#'; // anchor placeholder
            QString frameName = appendPicture(doc, ts);
            anchorFrameset( doc, outputFormats, pos, frameName );
        }
        else if ( localName == "text-box" && ns == ooNS::draw )
        {
            textData = '#'; // anchor placeholder
            QString frameName = appendTextBox(doc, ts);
            anchorFrameset( doc, outputFormats, pos, frameName );
        }
        else if ( isTextNS && localName == "a" )
        {
            m_styleStack.save();
            QString href( ts.attributeNS( ooNS::xlink, "href", QString::null) );
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
                linkElement.setAttribute("hrefName",ts.attributeNS( ooNS::xlink, "href", QString::null));
                linkElement.setAttribute("linkName",text);
                appendKWordVariable(doc, outputFormats, ts, pos, "STRING", 9, linkElement);
            }
            m_styleStack.restore();
        }
        else if ( isTextNS &&
                  (localName == "date" // fields
                   || localName == "print-time"
                   || localName == "print-date"
                   || localName == "creation-time"
                   || localName == "creation-date"
                   || localName == "modification-time"
                   || localName == "modification-date"
                   || localName == "time"
                   || localName == "page-number"
                   || localName == "chapter"
                   || localName == "file-name"
                   || localName == "author-name"
                   || localName == "author-initials"
                   || localName == "subject"
                   || localName == "title"
                   || localName == "description"
                   || localName == "variable-set"
                   || localName == "page-variable-get"
                   || localName == "user-defined"
                   || localName.startsWith( "sender-")
                      ) )
            // TODO in kword: printed-by, initial-creator
        {
            textData = "#";     // field placeholder
            appendField(doc, outputFormats, ts, pos);
        }
        else if ( isTextNS && localName == "bookmark" )
        {
            // the number of <PARAGRAPH> tags in the frameset element is the parag id
            // (-1 for starting at 0, +1 since not written yet)
            Q_ASSERT( !m_currentFrameset.isNull() );
            appendBookmark( doc, numberOfParagraphs( m_currentFrameset ),
                            pos, ts.attributeNS( ooNS::text, "name", QString::null ) );
        }
        else if ( isTextNS && localName == "bookmark-start" ) {
            m_bookmarkStarts.insert( ts.attributeNS( ooNS::text, "name", QString::null ),
                                     BookmarkStart( m_currentFrameset.attribute( "name" ),
                                                    numberOfParagraphs( m_currentFrameset ),
                                                    pos ) );
        }
        else if ( isTextNS && localName == "bookmark-end" ) {
            QString bkName = ts.attributeNS( ooNS::text, "name", QString::null );
            BookmarkStartsMap::iterator it = m_bookmarkStarts.find( bkName );
            if ( it == m_bookmarkStarts.end() ) { // bookmark end without start. This seems to happen..
                // insert simple bookmark then
                appendBookmark( doc, numberOfParagraphs( m_currentFrameset ),
                                pos, ts.attributeNS( ooNS::text, "name", QString::null ) );
            } else {
                if ( (*it).frameSetName != m_currentFrameset.attribute( "name" ) ) {
                    // Oh tell me this never happens...
                    kWarning(30518) << "Cross-frameset bookmark! Not supported." << endl;
                } else {
                    appendBookmark( doc, (*it).paragId, (*it).pos,
                                    numberOfParagraphs( m_currentFrameset ), pos, it.key() );
                }
                m_bookmarkStarts.remove( it );
            }
        }
        else if ( t.isNull() ) // no textnode, we must ignore
        {
            kWarning(30518) << "Ignoring tag " << ts.tagName() << endl;
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

    QDomElement formats = doc.createElement( "FORMATS" );

    QString paragraphText;
    uint pos = 0;

    // parse every child node of the paragraph
    parseSpanOrSimilar( doc, paragraph, p, formats, paragraphText, pos);

    QDomElement text = doc.createElement( "TEXT" );
    text.appendChild( doc.createTextNode( paragraphText ) );
    text.setAttribute( "xml:space", "preserve" );
    p.appendChild( text );
    //kDebug(30518) << k_funcinfo << "Para text is: " << paragraphText << endl;

    p.appendChild( formats );
    QDomElement layoutElement = doc.createElement( "LAYOUT" );
    p.appendChild( layoutElement );

    // Style name
    QString styleName = m_styleStack.userStyleName("paragraph");
    if ( !styleName.isEmpty() )
    {
        QDomElement nameElement = doc.createElement("NAME");
        nameElement.setAttribute( "value", kWordStyleName(styleName) );
        layoutElement.appendChild(nameElement);
    }

    writeLayout( doc, layoutElement );
    writeFormat( doc, layoutElement, 1, 0, 0 ); // paragraph format, useful for empty parags

    applyListStyle( doc, layoutElement, paragraph );

    QDomElement* paragraphStyle = m_styles[paragraph.attributeNS( ooNS::text, "style-name", QString::null )];
    QString masterPageName = paragraphStyle ? paragraphStyle->attributeNS( ooNS::style, "master-page-name", QString::null ) : QString::null;
    if ( masterPageName.isEmpty() )
        masterPageName = "Standard"; // Seems to be a builtin name for the default layout...
    if ( masterPageName != m_currentMasterPage )
    {
        // Detected a change in the master page -> this means we have to use a new page layout
        // and insert a frame break if not on the first paragraph.
        // In KWord we don't support sections so the first paragraph is the one that determines the page layout.
        if ( m_currentMasterPage.isEmpty() ) {
            m_currentMasterPage = masterPageName; // before writePageLayout to avoid recursion
            writePageLayout( doc, masterPageName );
        }
        else
        {
            m_currentMasterPage = masterPageName;
            QDomElement pageBreakElem = layoutElement.namedItem( "PAGEBREAKING" ).toElement();
            if ( !pageBreakElem.isNull() )  {
                pageBreakElem = doc.createElement( "PAGEBREAKING" );
                layoutElement.appendChild( pageBreakElem );
            }
            pageBreakElem.setAttribute( "hardFrameBreak", "true" );
            // We have no way to store the new page layout, KWord doesn't have sections.
        }
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

    if ( m_styleStack.hasAttributeNS( ooNS::fo, "color" ) ) { // 3.10.3
        QColor color( m_styleStack.attributeNS( ooNS::fo, "color" ) ); // #rrggbb format
        QDomElement colorElem( doc.createElement( "COLOR" ) );
        colorElem.setAttribute( "red", color.red() );
        colorElem.setAttribute( "blue", color.blue() );
        colorElem.setAttribute( "green", color.green() );
        format.appendChild( colorElem );
    }
    if ( m_styleStack.hasAttributeNS( ooNS::fo, "font-family" )  // 3.10.9
         || m_styleStack.hasAttributeNS( ooNS::style, "font-name") ) // 3.10.8
    {
        // Hmm, the remove "'" could break it's in the middle of the fontname...
        QString fontName = m_styleStack.attributeNS( ooNS::fo, "font-family" ).remove( "'" );
        if (fontName.isEmpty())
        {
            // ##### TODO. This is wrong. style:font-name refers to a font-decl entry.
            // We have to look it up there, and retrieve _all_ font attributes from it, not just the name.
            fontName = m_styleStack.attributeNS( ooNS::style, "font-name" ).remove( "'" );
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
    if ( m_styleStack.hasAttributeNS( ooNS::fo, "font-size" ) ) { // 3.10.14
        double pointSize = m_styleStack.fontSize();

        QDomElement fontSize( doc.createElement( "SIZE" ) );
        fontSize.setAttribute( "value", qRound(pointSize) ); // KWord uses toInt()!
        format.appendChild( fontSize );
    }
    if ( m_styleStack.hasAttributeNS( ooNS::fo, "font-weight" ) ) { // 3.10.24
        QDomElement weightElem( doc.createElement( "WEIGHT" ) );
        QString fontWeight = m_styleStack.attributeNS( ooNS::fo, "font-weight" );
        int boldness = fontWeight.toInt();
        if ( fontWeight == "bold" )
            boldness = 75;
        else if ( boldness == 0 )
            boldness = 50;
        weightElem.setAttribute( "value", boldness );
        format.appendChild( weightElem );
    }

    if ( m_styleStack.hasAttributeNS( ooNS::fo, "font-style" ) ) // 3.10.19
        if ( m_styleStack.attributeNS( ooNS::fo, "font-style" ) == "italic" ||
             m_styleStack.attributeNS( ooNS::fo, "font-style" ) == "oblique" ) // no difference in kotext
        {
            QDomElement italic = doc.createElement( "ITALIC" );
            italic.setAttribute( "value", 1 );
            format.appendChild( italic );
        }

    bool wordByWord = (m_styleStack.hasAttributeNS( ooNS::fo, "score-spaces")) // 3.10.25
                      && (m_styleStack.attributeNS( ooNS::fo, "score-spaces") == "false");
    if( m_styleStack.hasAttributeNS( ooNS::style, "text-crossing-out" )) // 3.10.6
    {
        QString strikeOutType = m_styleStack.attributeNS( ooNS::style, "text-crossing-out" );
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
    if( m_styleStack.hasAttributeNS( ooNS::style, "text-position")) // 3.10.7
    {
        QDomElement vertAlign = doc.createElement( "VERTALIGN" );
        QString text_position = m_styleStack.attributeNS( ooNS::style, "text-position");
        QString value;
        QString relativetextsize;
        OoUtils::importTextPosition( text_position, value, relativetextsize );
        vertAlign.setAttribute( "value", value );
        if ( !relativetextsize.isEmpty() )
            vertAlign.setAttribute( "relativetextsize", relativetextsize );
        format.appendChild( vertAlign );
    }
    if ( m_styleStack.hasAttributeNS( ooNS::style, "text-underline" ) ) // 3.10.22
    {
        QString underline;
        QString styleline;
        OoUtils::importUnderline( m_styleStack.attributeNS( ooNS::style, "text-underline" ),
                                  underline, styleline );
        QDomElement underLineElem = doc.createElement( "UNDERLINE" );
        underLineElem.setAttribute( "value", underline );
        underLineElem.setAttribute( "styleline", styleline );

        QString underLineColor = m_styleStack.attributeNS( ooNS::style, "text-underline-color" ); // 3.10.23
        if ( !underLineColor.isEmpty() && underLineColor != "font-color" )
            underLineElem.setAttribute("underlinecolor", underLineColor);
        if ( wordByWord )
            underLineElem.setAttribute("wordbyword", 1);
        format.appendChild( underLineElem );
    }
    // Small caps, lowercase, uppercase
    if ( m_styleStack.hasAttributeNS( ooNS::fo, "font-variant" ) // 3.10.1
         || m_styleStack.hasAttributeNS( ooNS::fo, "text-transform" ) ) // 3.10.2
    {
        QDomElement fontAttrib( doc.createElement( "FONTATTRIBUTE" ) );
        bool smallCaps = m_styleStack.attributeNS( ooNS::fo, "font-variant" ) == "small-caps";
        if ( smallCaps )
        {
            fontAttrib.setAttribute( "value", "smallcaps" );
        } else
        {
            // Both KWord and OO use "uppercase" and "lowercase".
            // TODO in KWord: "capitalize".
            fontAttrib.setAttribute( "value", m_styleStack.attributeNS( ooNS::fo, "text-transform" ) );
        }
        format.appendChild( fontAttrib );
    }

    if (m_styleStack.hasAttributeNS( ooNS::fo, "language")) // 3.10.17
    {
        QDomElement lang = doc.createElement("LANGUAGE");
        QString tmp = m_styleStack.attributeNS( ooNS::fo, "language");
        if (tmp=="en")
            lang.setAttribute("value", "en_US");
        else
            lang.setAttribute("value", tmp);
        format.appendChild(lang);
    }

    if (m_styleStack.hasAttributeNS( ooNS::style, "text-background-color")) // 3.10.28
    {
        QDomElement bgCol = doc.createElement("TEXTBACKGROUNDCOLOR");
        QColor tmp = m_styleStack.attributeNS( ooNS::style, "text-background-color");
        if (tmp != "transparent")
        {
            bgCol.setAttribute("red", tmp.red());
            bgCol.setAttribute("green", tmp.green());
            bgCol.setAttribute("blue", tmp.blue());
            format.appendChild(bgCol);
        }
    }

    if (m_styleStack.hasAttributeNS( ooNS::fo, "text-shadow")) // 3.10.21
    {
        QDomElement shadow = doc.createElement("SHADOW");
        QString css = m_styleStack.attributeNS( ooNS::fo, "text-shadow");
        // Workaround for OOo-1.1 bug: they forgot to save the color.
        QStringList tokens = QStringList::split(' ', css);
        if ( !tokens.isEmpty() ) {
            QColor col( tokens.first() );
            if ( !col.isValid() && tokens.count() > 1 ) {
                col.setNamedColor( tokens.last() );
            }
            if ( !col.isValid() ) // no valid color found at either end -> append gray
                css += " gray";
        }
        shadow.setAttribute("text-shadow", css);
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

    if ( format.hasChildNodes() || !length /*hack for styles, they should always have a format*/)
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
    if ( m_styleStack.attributeNS( ooNS::style, "text-auto-align" ) == "true" )
        flowElement.setAttribute( "align", "auto" );
    else
    {
        if ( m_styleStack.hasAttributeNS( ooNS::fo, "text-align" ) ) // 3.11.4
            flowElement.setAttribute( "align", Conversion::importAlignment( m_styleStack.attributeNS( ooNS::fo, "text-align" ) ) );
        else
            flowElement.setAttribute( "align", "auto" );
    }
    layoutElement.appendChild( flowElement );

    if ( m_styleStack.hasAttributeNS( ooNS::fo, "writing-mode" ) ) // http://web4.w3.org/TR/xsl/slice7.html#writing-mode
    {
        // LTR is lr-tb. RTL is rl-tb
        QString writingMode = m_styleStack.attributeNS( ooNS::fo, "writing-mode" );
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
    if( m_styleStack.hasAttributeNS( ooNS::fo, "break-before") ||
        m_styleStack.hasAttributeNS( ooNS::fo, "break-after") ||
        m_styleStack.hasAttributeNS( ooNS::style, "break-inside") ||
        m_styleStack.hasAttributeNS( ooNS::style, "keep-with-next") ||
        m_styleStack.hasAttributeNS( ooNS::fo, "keep-with-next") )
    {
        QDomElement pageBreak = doc.createElement( "PAGEBREAKING" );
        if ( m_styleStack.hasAttributeNS( ooNS::fo, "break-before") ) { // 3.11.24
            bool breakBefore = m_styleStack.attributeNS( ooNS::fo, "break-before" ) != "auto";
            // TODO in KWord: implement difference between "column" and "page"
            pageBreak.setAttribute("hardFrameBreak", breakBefore ? "true" : "false");
        }
        else if ( m_styleStack.hasAttributeNS( ooNS::fo, "break-after") ) { // 3.11.24
            bool breakAfter = m_styleStack.attributeNS( ooNS::fo, "break-after" ) != "auto";
            // TODO in KWord: implement difference between "column" and "page"
            pageBreak.setAttribute("hardFrameBreakAfter", breakAfter ? "true" : "false");
        }

        if ( m_styleStack.hasAttributeNS( ooNS::style, "break-inside" ) ) { // 3.11.7
            bool breakInside = m_styleStack.attributeNS( ooNS::style, "break-inside" ) == "true";
            pageBreak.setAttribute("linesTogether", breakInside ? "false" : "true"); // opposite meaning
        }
        if ( m_styleStack.hasAttributeNS( ooNS::fo, "keep-with-next" ) ) { // 3.11.31 (the doc said style:keep-with-next but DV said it's wrong)
            // OASIS spec says it's "auto"/"always", not a boolean. Not sure which one OO uses.
            QString val = m_styleStack.attributeNS( ooNS::fo, "keep-with-next" );
            pageBreak.setAttribute("keepWithNext", ( val == "true" || val == "always" ) ? "true" : "false");
        }
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
    if ( object.hasAttributeNS( ooNS::svg, "width" ) ) { // fixed width
        // TODO handle percentage (of enclosing table/frame/page)
        width = KoUnit::parseValue( object.attributeNS( ooNS::svg, "width", QString::null ) );
    } else if ( object.hasAttributeNS( ooNS::fo, "min-width" ) ) {
        // min-width is not supported in KWord. Let's use it as a fixed width.
        width = KoUnit::parseValue( object.attributeNS( ooNS::fo, "min-width", QString::null ) );
    } else {
        kWarning(30518) << "Error in text-box: neither width nor min-width specified!" << endl;
    }
    double height = 100;
    bool hasMinHeight = false;
    if ( object.hasAttributeNS( ooNS::svg, "height" ) ) { // fixed height
        // TODO handle percentage (of enclosing table/frame/page)
        height = KoUnit::parseValue( object.attributeNS( ooNS::svg, "height", QString::null ) );
    } else if ( object.hasAttributeNS( ooNS::fo, "min-height" ) ) {
        height = KoUnit::parseValue( object.attributeNS( ooNS::fo, "min-height", QString::null ) );
        hasMinHeight = true;
    } else {
        kWarning(30518) << "Error in text-box: neither height nor min-height specified!" << endl;
    }

    // draw:textarea-vertical-align, draw:textarea-horizontal-align

    // Not supported in KWord: fo:max-height  fo:max-width
    //                         Anchor, Shadow (3.11.30), Columns

    //  #### horizontal-pos horizontal-rel vertical-pos vertical-rel anchor-type
    //  All the above changes the placement!
    //  See 3.8 (p199) for details.

    // TODO draw:auto-grow-height  draw:auto-grow-width - hmm? I thought min-height meant auto-grow-height...


    KoRect frameRect( KoUnit::parseValue( object.attributeNS( ooNS::svg, "x", QString::null ) ),
                      KoUnit::parseValue( object.attributeNS( ooNS::svg, "y", QString::null ) ),
                      width, height );

    frameElementOut.setAttribute("left", frameRect.left() );
    frameElementOut.setAttribute("right", frameRect.right() );
    frameElementOut.setAttribute("top", frameRect.top() );
    frameElementOut.setAttribute("bottom", frameRect.bottom() );
    if ( hasMinHeight )
        frameElementOut.setAttribute("min-height", height );
    frameElementOut.setAttribute( "z-index", object.attributeNS( ooNS::draw, "z-index", QString::null ) );
    QPair<int, QString> attribs = Conversion::importWrapping( m_styleStack.attributeNS( ooNS::style, "wrap" ) );
    frameElementOut.setAttribute("runaround", attribs.first );
    if ( !attribs.second.isEmpty() )
        frameElementOut.setAttribute("runaroundSide", attribs.second );
    // ## runaroundGap is a problem. KWord-1.3 had one value, OO has 4 (margins on all sides, see p98).
    // Fixed in KWord-post-1.3, it has 4 values now.


    if ( isText ) {
        int overflowBehavior;
        if ( m_styleStack.hasAttributeNS( ooNS::style, "overflow-behavior" ) ) { // OASIS extension
            overflowBehavior = Conversion::importOverflowBehavior( m_styleStack.attributeNS( ooNS::style, "overflow-behavior" ) );
        } else {
            // AutoCreateNewFrame not supported in OO-1.1. The presence of min-height tells if it's an auto-resized frame.
            overflowBehavior = hasMinHeight ? 0 /*AutoExtendFrame*/ : 2 /*Ignore, i.e. fixed size*/;
        }
        // Not implemented in KWord: contour wrapping
        frameElementOut.setAttribute("autoCreateNewFrame", overflowBehavior);
    }

    // TODO sheetSide (not implemented in KWord, but in its DTD)

    importCommonFrameProperties( frameElementOut );
}

void OoWriterImport::importCommonFrameProperties( QDomElement& frameElementOut )
{
    // padding. fo:padding for 4 values or padding-left/right/top/bottom (3.11.29 p228)
    double paddingLeft = KoUnit::parseValue( m_styleStack.attributeNS( ooNS::fo, "padding", "left" ) );
    double paddingRight = KoUnit::parseValue( m_styleStack.attributeNS( ooNS::fo, "padding", "right" ) );
    double paddingTop = KoUnit::parseValue( m_styleStack.attributeNS( ooNS::fo, "padding", "top" ) );
    double paddingBottom = KoUnit::parseValue( m_styleStack.attributeNS( ooNS::fo, "padding", "bottom" ) );

    if ( paddingLeft != 0 )
        frameElementOut.setAttribute( "bleftpt", paddingLeft );
    if ( paddingRight != 0 )
        frameElementOut.setAttribute( "brightpt", paddingRight );
    if ( paddingTop != 0 )
        frameElementOut.setAttribute( "btoppt", paddingTop );
    if ( paddingBottom != 0 )
        frameElementOut.setAttribute( "bbottompt", paddingBottom );

    // background color (3.11.25)
    bool transparent = false;
    QColor bgColor;
    if ( m_styleStack.hasAttributeNS( ooNS::fo, "background-color" ) ) {
        QString color = m_styleStack.attributeNS( ooNS::fo, "background-color" );
        if ( color == "transparent" )
            transparent = true;
        else
            bgColor.setNamedColor( color );
    }
    if ( transparent )
        frameElementOut.setAttribute( "bkStyle", 0 );
    else if ( bgColor.isValid() ) {
        // OOwriter doesn't support fill patterns (bkStyle).
        // But the file support is more generic, and supports: draw:stroke, svg:stroke-color, draw:fill, draw:fill-color
        frameElementOut.setAttribute( "bkStyle", 1 );
        frameElementOut.setAttribute( "bkRed", bgColor.red() );
        frameElementOut.setAttribute( "bkBlue", bgColor.blue() );
        frameElementOut.setAttribute( "bkGreen", bgColor.green() );
    }


    // borders (3.11.27)
    // can be none/hidden, solid and double. General form is the XSL/FO "width|style|color"
    {
        double width;
        int style;
        QColor color;
        if (OoUtils::parseBorder(m_styleStack.attributeNS( ooNS::fo, "border", "left"), &width, &style, &color)) {
            frameElementOut.setAttribute( "lWidth", width );
            if ( color.isValid() ) { // should be always true, but who knows
                frameElementOut.setAttribute( "lRed", color.red() );
                frameElementOut.setAttribute( "lBlue", color.blue() );
                frameElementOut.setAttribute( "lGreen", color.green() );
            }
            frameElementOut.setAttribute( "lStyle", style );
        }
        if (OoUtils::parseBorder(m_styleStack.attributeNS( ooNS::fo, "border", "right"), &width, &style, &color)) {
            frameElementOut.setAttribute( "rWidth", width );
            if ( color.isValid() ) { // should be always true, but who knows
                frameElementOut.setAttribute( "rRed", color.red() );
                frameElementOut.setAttribute( "rBlue", color.blue() );
                frameElementOut.setAttribute( "rGreen", color.green() );
            }
            frameElementOut.setAttribute( "rStyle", style );
        }
        if (OoUtils::parseBorder(m_styleStack.attributeNS( ooNS::fo, "border", "top"), &width, &style, &color)) {
            frameElementOut.setAttribute( "tWidth", width );
            if ( color.isValid() ) { // should be always true, but who knows
                frameElementOut.setAttribute( "tRed", color.red() );
                frameElementOut.setAttribute( "tBlue", color.blue() );
                frameElementOut.setAttribute( "tGreen", color.green() );
            }
            frameElementOut.setAttribute( "tStyle", style );
        }
        if (OoUtils::parseBorder(m_styleStack.attributeNS( ooNS::fo, "border", "bottom"), &width, &style, &color)) {
            frameElementOut.setAttribute( "bWidth", width );
            if ( color.isValid() ) { // should be always true, but who knows
                frameElementOut.setAttribute( "bRed", color.red() );
                frameElementOut.setAttribute( "bBlue", color.blue() );
                frameElementOut.setAttribute( "bGreen", color.green() );
            }
            frameElementOut.setAttribute( "bStyle", style );
        }
    }
    // TODO more refined border spec for double borders (3.11.28)
}

QString OoWriterImport::appendTextBox(QDomDocument& doc, const QDomElement& object)
{
    const QString frameName ( object.attributeNS( ooNS::draw, "name", QString::null) ); // ### TODO: what if empty, i.e. non-unique
    kDebug(30518) << "appendTextBox " << frameName << endl;
    m_styleStack.save();
    fillStyleStack( object, ooNS::draw, "style-name" ); // get the style for the graphics element

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
    if ( m_styleStack.hasAttributeNS( ooNS::draw, "text-style-name" ) )
        addStyles( m_styles[m_styleStack.attributeNS( ooNS::draw, "text-style-name" )] );

    // Parse contents
    parseBodyOrSimilar( doc, object, framesetElement );

    return frameName;
}

// OOo SPEC: 3.6.3 p149
void OoWriterImport::importFootnote( QDomDocument& doc, const QDomElement& object, QDomElement& formats, uint pos, const QString& localName )
{
    const QString frameName( object.attributeNS( ooNS::text, "id", QString::null) );
    QDomElement citationElem = KoDom::namedItemNS( object, ooNS::text, (localName+"-citation").latin1() ).toElement();

    bool endnote = localName == "endnote";

    QString label = citationElem.attributeNS( ooNS::text, "label", QString::null );
    bool autoNumbered = label.isEmpty();

    // The var
    QDomElement footnoteElem = doc.createElement( "FOOTNOTE" );
    if ( autoNumbered )
        footnoteElem.setAttribute( "value", 1 ); // KWord will renumber anyway
    else
        footnoteElem.setAttribute( "value", label );
    footnoteElem.setAttribute( "notetype", endnote ? "endnote" : "footnote" );
    footnoteElem.setAttribute( "numberingtype", autoNumbered ? "auto" : "manual" );
    footnoteElem.setAttribute( "frameset", frameName );

    appendKWordVariable( doc, formats, citationElem, pos, "STRI", 11 /*KWord code for footnotes*/, footnoteElem );

    // The frameset
    QDomElement framesetElement( doc.createElement("FRAMESET") );
    framesetElement.setAttribute( "frameType", 1 /* text */ );
    framesetElement.setAttribute( "frameInfo", 7 /* footnote/endnote */ );
    framesetElement.setAttribute( "name" , frameName );
    QDomElement framesetsPluralElement (doc.documentElement().namedItem("FRAMESETS").toElement());
    framesetsPluralElement.appendChild(framesetElement);
    createInitialFrame( framesetElement, 29, 798, 567, 567+41, true, NoFollowup );
    // TODO importCommonFrameProperties ?

    // The text inside the frameset
    QDomElement bodyElem = KoDom::namedItemNS( object, ooNS::text, (localName+"-body").latin1() ).toElement();
    parseBodyOrSimilar( doc, bodyElem, framesetElement );
}

QString OoWriterImport::appendPicture(QDomDocument& doc, const QDomElement& object)
{
    const QString frameName ( object.attributeNS( ooNS::draw, "name", QString::null) ); // ### TODO: what if empty, i.e. non-unique
    const QString href ( object.attributeNS( ooNS::xlink, "href", QString::null) );

    kDebug(30518) << "Picture: " << frameName << " " << href << " (in OoWriterImport::appendPicture)" << endl;

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
            kWarning(30518) << "Picture " << filename << " not found!" << endl;
            return frameName;
        }
        if (entry->isDirectory())
        {
            kWarning(30518) << "Picture " << filename << " is a directory!" << endl;
            return frameName;
        }
        const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
        QIODevice* io=f->device();
        kDebug(30518) << "Picture " << filename << " has size " << f->size() << endl;

        if (!io)
        {
            kWarning(30518) << "No QIODevice for picture  " << frameName << " " << href << endl;
            return frameName;
        }
        if (!picture.load(io,strExtension))
            kWarning(30518) << "Cannot load picture: " << frameName << " " << href << endl;
    }
    else
    {
        KUrl url;
        url.setPath(href); // ### TODO: is this really right?
        picture.setKeyAndDownloadPicture(url, 0); // ### TODO: find a better parent if possible
    }

    kDebug(30518) << "Picture ready! Key: " << picture.getKey().toString() << " Size:" << picture.getOriginalSize() << endl;

    QString strStoreName;
    strStoreName="pictures/picture";
    strStoreName+=QString::number(++m_pictureNumber);
    strStoreName+='.';
    strStoreName+=picture.getExtension();

    kDebug(30518) << "Storage name: " << strStoreName << endl;

    KoStoreDevice* out = m_chain->storageFile( strStoreName , KoStore::Write );
    if (out)
    {
        if (!out->open(QIODevice::WriteOnly))
        {
            kWarning(30518) << "Cannot open for saving picture: " << frameName << " " << href << endl;
            return frameName;
        }
        if (!picture.save(out))
        kWarning(30518) << "Cannot save picture: " << frameName << " " << href << endl;
        out->close();
    }
    else
    {
         kWarning(30518) << "Cannot store picture: " << frameName << " " << href << endl;
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
    fillStyleStack( object, ooNS::draw, "style-name" ); // get the style for the graphics element
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

void OoWriterImport::appendField(QDomDocument& doc, QDomElement& outputFormats, QDomElement& object, uint pos)
// Note: QDomElement& outputFormats replaces the parameter QDomElement& e in OoImpressImport::appendField
//  (otherwise it should be the same parameters.)
{
    const QString localName (object.localName());
    //kDebug(30518) << localName << endl;
    int subtype = -1;

    if ( localName.endsWith( "date" ) || localName.endsWith( "time" ) )
    {
        QString dataStyleName = object.attributeNS( ooNS::style, "data-style-name", QString::null );
        QString dateFormat = "locale";
        DataFormatsMap::const_iterator it = m_dateTimeFormats.find( dataStyleName );
        if ( it != m_dateTimeFormats.end() )
            dateFormat = (*it);

        if ( localName == "date" )
        {
            subtype = 1; // current (or fixed) date
            // Standard form of the date is in date-value. Example: 2004-01-21T10:57:05
            QDateTime dt(QDate::fromString(object.attributeNS( ooNS::text, "date-value", QString::null), Qt::ISODate));

            bool fixed = (object.hasAttributeNS( ooNS::text, "fixed") && object.attributeNS( ooNS::text, "fixed", QString::null)=="true");
            if (!dt.isValid())
            {
                dt = QDateTime::currentDateTime(); // OOo docs say so :)
                fixed = false;
            }
            const QDate date(dt.date());
            const QTime time(dt.time());
            if ( fixed )
                subtype = 0;

            QDomElement dateElement ( doc.createElement("DATE") );
            dateElement.setAttribute("fix", fixed ? 1 : 0);
            dateElement.setAttribute("subtype", subtype);
            dateElement.setAttribute("day", date.day());
            dateElement.setAttribute("month", date.month());
            dateElement.setAttribute("year", date.year());
            dateElement.setAttribute("hour", time.hour());
            dateElement.setAttribute("minute", time.minute());
            dateElement.setAttribute("second", time.second());
            if (object.hasAttributeNS( ooNS::text, "date-adjust"))
                dateElement.setAttribute("correct", object.attributeNS( ooNS::text, "date-adjust", QString::null));
            appendKWordVariable(doc, outputFormats, object, pos, "DATE" + dateFormat, 0, dateElement);
        }
        else if (localName == "time")
        {
            // Use QDateTime to work around a possible problem of QTime::FromString in Qt 3.2.2
            QDateTime dt(QDateTime::fromString(object.attributeNS( ooNS::text, "time-value", QString::null), Qt::ISODate));

            bool fixed = (object.hasAttributeNS( ooNS::text, "fixed") && object.attributeNS( ooNS::text, "fixed", QString::null)=="true");

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
            /*if (object.hasAttributeNS( ooNS::text, "time-adjust"))
              timeElem.setAttribute("correct", object.attributeNS( ooNS::text, "time-adjust", QString::null));*/ // ### TODO
            appendKWordVariable(doc, outputFormats, object, pos, "TIME" + dateFormat, 2, timeElement);

        }
        else if ( localName == "print-time"
                  || localName == "print-date"
                  || localName == "creation-time"
                  || localName == "creation-date"
                  || localName == "modification-time"
                  || localName == "modification-date" )
        {
            if ( localName.startsWith( "print" ) )
                subtype = 2;
            else if ( localName.startsWith( "creation" ) )
                subtype = 3;
            else if ( localName.startsWith( "modification" ) )
                subtype = 4;
            // We do NOT include the date value here. It will be retrieved from
            // meta.xml
            QDomElement dateElement ( doc.createElement("DATE") );
            dateElement.setAttribute("subtype", subtype);
            if (object.hasAttributeNS( ooNS::text, "date-adjust"))
                dateElement.setAttribute("correct", object.attributeNS( ooNS::text, "date-adjust", QString::null));
            appendKWordVariable(doc, outputFormats, object, pos, "DATE" + dateFormat, 0, dateElement);
        }
    }// end of date/time variables
    else if (localName == "page-number")
    {
        subtype = 0;        // VST_PGNUM_CURRENT

        if (object.hasAttributeNS( ooNS::text, "select-page"))
        {
            const QString select = object.attributeNS( ooNS::text, "select-page", QString::null);

            if (select == "previous")
                subtype = 3;    // VST_PGNUM_PREVIOUS
            else if (select == "next")
                subtype = 4;    // VST_PGNUM_NEXT
        }

        QDomElement pgnumElement ( doc.createElement("PGNUM") );
        pgnumElement.setAttribute("subtype", subtype);
        pgnumElement.setAttribute("value", object.text());
        appendKWordVariable(doc, outputFormats, object, pos, "NUMBER", 4, pgnumElement);
    }
    else if (localName == "chapter")
    {
        const QString display = object.attributeNS( ooNS::text, "display", QString::null );
        // display can be name, number, number-and-name, plain-number-and-name, plain-number
        QDomElement pgnumElement ( doc.createElement("PGNUM") );
        pgnumElement.setAttribute("subtype", 2); // VST_CURRENT_SECTION
        pgnumElement.setAttribute("value", object.text());
        appendKWordVariable(doc, outputFormats, object, pos, "STRING", 4, pgnumElement);
    }
    else if (localName == "file-name")
    {
        subtype = 5;

        if (object.hasAttributeNS( ooNS::text, "display"))
        {
            const QString display = object.attributeNS( ooNS::text, "display", QString::null);

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
    else if (localName == "author-name"
             || localName == "author-initials"
             || localName == "subject"
             || localName == "title"
             || localName == "description"
        )
    {
        subtype = 2;        // VST_AUTHORNAME

        if (localName == "author-initials")
            subtype = 16;       // VST_INITIAL
        else if ( localName == "subject" ) // TODO in kword
            subtype = 10; // title
        else if ( localName == "title" )
            subtype = 10;
        else if ( localName == "description" )
            subtype = 11; // Abstract

        QDomElement authorElem = doc.createElement("FIELD");
        authorElem.setAttribute("subtype", subtype);
        authorElem.setAttribute("value", object.text());
        appendKWordVariable(doc, outputFormats, object, pos, "STRING", 8, authorElem);
    }
    else if ( localName.startsWith( "sender-" ) )
    {
        int subtype = -1;
        const Q3CString afterText( localName.latin1() + 5 );
        if ( afterText == "sender-company" )
            subtype = 4; //VST_COMPANYNAME;
        else if ( afterText == "sender-firstname" )
            ; // ## This is different from author-name, but the notion of 'sender' is unclear...
        else if ( afterText == "sender-lastname" )
            ; // ## This is different from author-name, but the notion of 'sender' is unclear...
        else if ( afterText == "sender-initials" )
            ; // ## This is different from author-initials, but the notion of 'sender' is unclear...
        else if ( afterText == "sender-street" )
            subtype = 14; // VST_STREET;
        else if ( afterText == "sender-country" )
            subtype = 9; // VST_COUNTRY;
        else if ( afterText == "sender-postal-code" )
            subtype = 12; //VST_POSTAL_CODE;
        else if ( afterText == "sender-city" )
            subtype = 13; // VST_CITY;
        else if ( afterText == "sender-title" )
            subtype = 15; // VST_AUTHORTITLE; // Small hack (it's supposed to be about the sender, not about the author)
        else if ( afterText == "sender-position" )
            subtype = 15; // VST_AUTHORTITLE; // TODO separate variable
        else if ( afterText == "sender-phone-private" )
            subtype = 7; // VST_TELEPHONE;
        else if ( afterText == "sender-phone-work" )
            subtype = 7; // VST_TELEPHONE; // ### TODO separate type
        else if ( afterText == "sender-fax" )
            subtype = 8; // VST_FAX;
        else if ( afterText == "sender-email" )
            subtype = 3; // VST_EMAIL;
        if ( subtype != -1 )
        {
            QDomElement fieldElem = doc.createElement("FIELD");
            fieldElem.setAttribute("subtype", subtype);
            fieldElem.setAttribute("value", object.text());
            appendKWordVariable(doc, outputFormats, object, pos, "STRING", 8, fieldElem);
        }
    }
    else if ( localName == "variable-set"
              || localName == "user-defined" )
    {
        // We treat both the same. For OO the difference is that
        // - variable-set is related to variable-decls (defined in <body>);
        //                 its value can change in the middle of the document.
        // - user-defined is related to meta::user-defined in meta.xml
        QDomElement customElem = doc.createElement( "CUSTOM" );
        customElem.setAttribute( "name", object.attributeNS( ooNS::text, "name", QString::null ) );
        customElem.setAttribute( "value", object.text() );
        appendKWordVariable(doc, outputFormats, object, pos, "STRING", 6, customElem);
    }
    else
    {
        kWarning(30518) << "Unsupported field " << localName << endl;
    }
// TODO localName == "page-variable-get", "initial-creator" and many more
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
    QString tableName ( parent.attributeNS( ooNS::table, "name", QString::null) ); // TODO: what if empty (non-unique?)
    kDebug(30518) << "Found table " << tableName << endl;

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


    // Left position of the cell/column (similar to RTF's \cellx). The last one defined is the right position of the last cell/column
    Q3MemArray<double> columnLefts(4);
    uint maxColumns=columnLefts.size() - 1;

    uint col=0;
    columnLefts[0]=0.0; // Initialize left of first cell
    QDomElement elem;
    forEachElement( elem, parent )
    {
        if ( elem.localName() == "table-column" && elem.namespaceURI() == ooNS::table )
        {
            uint repeat = elem.attributeNS( ooNS::table, "number-columns-repeated", "1").toUInt(); // Default 1 time
            if (!repeat)
                repeat=1; // At least one column defined!
            const QString styleName ( elem.attributeNS( ooNS::table, "style-name", QString::null) );
            kDebug(30518) << "Column " << col << " style " << styleName << endl;
            const QDomElement* style=m_styles.find(styleName);
            double width=0.0;
            if (style)
            {
                const QDomElement elemProps( KoDom::namedItemNS( *style, ooNS::style, "properties") );
                if (elemProps.isNull())
                {
                    kWarning(30518) << "Could not find table column style properties!" << endl;
                }
                const QString strWidth ( elemProps.attributeNS( ooNS::style, "column-width", QString::null) );
                kDebug(30518) << "- raw style width " << strWidth << endl;
                width = KoUnit::parseValue( strWidth );
            }
            else
                kWarning(30518) << "Could not find table column style!" << endl;

            if (width < 1.0) // Something is wrong with the width
            {
                kWarning(30518) << "Table column width ridiculous, assuming 1 inch!" << endl;
                width=72.0;
            }
            else
                kDebug(30518) << "- style width " << width << endl;

            for (uint j=0; j<repeat; j++)
            {
                ++col;
                if (col>=maxColumns)
                {
                    // We need more columns
                    maxColumns+=4;
                    columnLefts.resize(maxColumns+1, Q3GArray::SpeedOptim);
                }
                columnLefts.at(col) = width + columnLefts.at(col-1);
                kDebug(30518) << "Cell column " << col-1 << " left " << columnLefts.at(col-1) << " right " << columnLefts.at(col) << endl;
            }
        }
    }

    uint row=0;
    uint column=0;
    parseInsideOfTable(doc, parent, currentFramesetElement, tableName, columnLefts, row, column);
}

void OoWriterImport::parseInsideOfTable( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement,
    const QString& tableName, const Q3MemArray<double> & columnLefts, uint& row, uint& column )
{
    kDebug(30518) << "parseInsideOfTable: columnLefts.size()=" << columnLefts.size() << endl;
    QDomElement framesetsPluralElement (doc.documentElement().namedItem("FRAMESETS").toElement());
    if (framesetsPluralElement.isNull())
    {
        kError(30518) << "Cannot find KWord's <FRAMESETS>! Cannot process table!" << endl;
        return;
    }

    QDomElement e;
    forEachElement( e, parent )
    {
        m_styleStack.save();
        const QString localName = e.localName();
        const QString ns = e.namespaceURI();
        if ( ns != ooNS::table ) {
            kWarning(30518) << "Skipping element " << e.tagName() << " (in OoWriterImport::parseInsideOfTable)" << endl;
            continue;
        }

        if ( localName == "table-cell" ) // OOo SPEC 4.8.1 p267
        {
            const QString frameName(i18nc("Frameset name","Table %3, row %1, column %2",row,column,tableName)); // The table name could have a % sequence, so use the table name as last!
            kDebug(30518) << "Trying to create " << frameName << endl;

            // We need to create a frameset for the cell
            QDomElement framesetElement(doc.createElement("FRAMESET"));
            framesetElement.setAttribute("frameType",1);
            framesetElement.setAttribute("frameInfo",0);
            framesetElement.setAttribute("visible",1);
            framesetElement.setAttribute("name",frameName);
            framesetElement.setAttribute("row",row);
            framesetElement.setAttribute("col",column);
            int rowSpan = e.attributeNS( ooNS::table, "number-rows-spanned", QString::null ).toInt();
            framesetElement.setAttribute("rows",rowSpan == 0 ? 1 : rowSpan);
            int colSpan = e.attributeNS( ooNS::table, "number-columns-spanned", QString::null ).toInt();
            framesetElement.setAttribute("cols",colSpan == 0 ? 1 : colSpan);
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

            m_styleStack.save();
            fillStyleStack( e, ooNS::table, "style-name" ); // get the style for the graphics element
            importCommonFrameProperties(frameElementOut);
            m_styleStack.restore();

            framesetElement.appendChild(frameElementOut);

            parseBodyOrSimilar( doc, e, framesetElement ); // We change the frameset!
            column++;
        }
        else if ( localName == "covered-table-cell" )
        {
            column++;
        }
        else if ( localName == "table-row" )
        {
            column=0;
            parseInsideOfTable( doc, e, currentFramesetElement, tableName, columnLefts, row, column);
            row++;
        }
        else if ( localName == "table-header-rows" ) // Provisory (###TODO)
        {
            parseInsideOfTable( doc, e, currentFramesetElement, tableName, columnLefts, row, column);
        }
        else if ( localName == "table-column" )
        {
            // Allready treated in OoWriterImport::parseTable, we do not need to do anything here!
        }
        // TODO sub-table
        else
        {
            kWarning(30518) << "Skipping element " << localName << " (in OoWriterImport::parseInsideOfTable)" << endl;
        }

        m_styleStack.restore();
    }
}

void OoWriterImport::appendBookmark( QDomDocument& doc, int paragId, int pos, const QString& name )
{
    appendBookmark( doc, paragId, pos, paragId, pos, name );
}

void OoWriterImport::appendBookmark( QDomDocument& doc, int paragId, int pos, int endParagId, int endPos, const QString& name )
{
    Q_ASSERT( !m_currentFrameset.isNull() );
    const QString frameSetName = m_currentFrameset.attribute( "name" );
    Q_ASSERT( !frameSetName.isEmpty() );
    QDomElement bookmarks = doc.documentElement().namedItem( "BOOKMARKS" ).toElement();
    if ( bookmarks.isNull() ) {
        bookmarks = doc.createElement( "BOOKMARKS" );
        doc.documentElement().appendChild( bookmarks );
    }
    QDomElement bkItem = doc.createElement( "BOOKMARKITEM" );
    bkItem.setAttribute( "name", name );
    bkItem.setAttribute( "frameset", frameSetName );
    bkItem.setAttribute( "startparag", paragId );
    bkItem.setAttribute( "cursorIndexStart", pos );
    bkItem.setAttribute( "endparag", endParagId );
    bkItem.setAttribute( "cursorIndexEnd", endPos );
    bookmarks.appendChild( bkItem );
}

// OOo SPEC: 3.6.1 p146
void OoWriterImport::importFootnotesConfiguration( QDomDocument& doc, const QDomElement& elem, bool endnote )
{
    QDomElement docElement( doc.documentElement() );
    // can we really be called more than once?
    QString elemName = endnote ? "ENDNOTESETTING" : "FOOTNOTESETTING";
    Q_ASSERT( docElement.namedItem( elemName ).isNull() );
    QDomElement settings = doc.createElement( elemName );
    docElement.appendChild( settings );

    // BUG in OO (both 1.0.1 and 1.1). It saves it with an off-by-one (reported to xml@).
    // So instead of working around it (which would break with the next version, possibly)
    // let's ignore this for now.
#if 0
    if ( elem.hasAttributeNS( ooNS::text, "start-value" ) ) {
        int startValue = elem.attributeNS( ooNS::text, "start-value", QString::null ).toInt();
        settings.setAttribute( "start", startValue );
    }
#endif
    settings.setAttribute( "type", Conversion::importCounterType( elem.attributeNS( ooNS::style, "num-format", QString::null ) ) );
    settings.setAttribute( "lefttext", elem.attributeNS( ooNS::style, "num-prefix", QString::null ) );
    settings.setAttribute( "righttext", elem.attributeNS( ooNS::style, "num-suffix", QString::null ) );
}

void OoWriterImport::appendTOC( QDomDocument& doc, const QDomElement& toc )
{
    // table-of-content OOo SPEC 7.5 p452
    //fillStyleStack( toc, ooNS::text, "style-name" ); that's the section style

    //QDomElement tocSource = KoDom::namedItemNS( toc, ooNS::text, "table-of-content-source" );
    // TODO parse templates and generate "Contents ..." styles from it
    //for ( QDomNode n(tocSource.firstChild()); !text.isNull(); text = text.nextSibling() )
    //{
    //}

    QDomElement tocIndexBody = KoDom::namedItemNS( toc, ooNS::text, "index-body" );
    QDomElement t;
    forEachElement( t, tocIndexBody )
    {
        m_styleStack.save();
        const QString localName = t.localName();
        QDomElement e;
        bool isTextNS = e.namespaceURI() == ooNS::text;
        if ( isTextNS && localName == "index-title" ) {
            parseBodyOrSimilar( doc, t, m_currentFrameset ); // recurse again
        } else if ( isTextNS && localName == "p" ) {
            fillStyleStack( t, ooNS::text, "style-name" );
            e = parseParagraph( doc, t );
        }
        if ( !e.isNull() )
            m_currentFrameset.appendChild( e );
        m_styleStack.restore();
    }

    // KWord has a special attribute to know if a TOC is present
    m_hasTOC = true;
}

// TODO style:num-format, default number format for page styles,
// used for page numbers (2.3.1)

void OoWriterImport::finishDocumentContent( QDomDocument& mainDocument )
{
    QDomElement attributes = mainDocument.createElement( "ATTRIBUTES" );
    QDomElement docElement = mainDocument.documentElement();
    docElement.appendChild( attributes );
    attributes.setAttribute( "hasTOC", m_hasTOC ? 1 : 0 );
    attributes.setAttribute( "hasHeader", m_hasHeader );
    attributes.setAttribute( "hasFooter", m_hasFooter );
    // TODO unit?, tabStopValue
    // TODO activeFrameset, cursorParagraph, cursorIndex

    // Done at the end: write the type of headers/footers,
    // depending on which kind of headers and footers we received.
    QDomElement paperElement = docElement.namedItem("PAPER").toElement();
    Q_ASSERT ( !paperElement.isNull() ); // writePageLayout should have been called!
    if ( !paperElement.isNull() )
    {
        //kDebug(30513) << k_funcinfo << "m_headerFooters=" << m_headerFooters << endl;
        //paperElement.setAttribute("hType", Conversion::headerMaskToHType( m_headerFooters ) );
        //paperElement.setAttribute("fType", Conversion::headerMaskToFType( m_headerFooters ) );
    }
}

QString OoWriterImport::kWordStyleName( const QString& ooStyleName )
{
    if ( ooStyleName.startsWith( "Contents " ) ) {
        QString s( ooStyleName );
        return s.replace( 0, 9, QString("Contents Head ") ); // Awful hack for KWord's broken "update TOC" feature
    } else {
        return ooStyleName;
    }
}

// OOo SPEC: 2.3.3 p59
void OoWriterImport::importHeaderFooter( QDomDocument& doc, const QDomElement& headerFooter, bool hasEvenOdd, QDomElement& style )
{
    const QString localName = headerFooter.localName();
    QDomElement framesetElement = doc.createElement("FRAMESET");
    QDomElement framesetsPluralElement (doc.documentElement().namedItem("FRAMESETS").toElement());
    framesetElement.setAttribute( "frameType", 1 /* text */);
    framesetElement.setAttribute( "frameInfo", Conversion::headerTypeToFrameInfo( localName, hasEvenOdd ) );
    framesetElement.setAttribute( "name", Conversion::headerTypeToFramesetName( localName, hasEvenOdd ) );
    framesetsPluralElement.appendChild(framesetElement);

    bool isHeader = localName.startsWith( "header" );
    if ( isHeader )
        m_hasHeader = true;
    else
        m_hasFooter = true;
    QDomElement frameElementOut = createInitialFrame( framesetElement, 29, 798, isHeader?0:567, isHeader?41:567+41, true, Copy );
    if ( !style.isNull() )
        m_styleStack.push( style );
    importCommonFrameProperties( frameElementOut );
    if ( !style.isNull() )
        m_styleStack.pop(); // don't let it be active when parsing the text

    parseBodyOrSimilar( doc, headerFooter, framesetElement );
}

#include "oowriterimport.moc"
