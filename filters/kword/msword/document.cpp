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

#include "document.h"
#include "conversion.h"
#include "texthandler.h"
#include "graphicshandler.h"
#include "versionmagic.h"

#include <KoRect.h>
#include <KoUnit.h>
#include <KoPageLayout.h>
#include <kdebug.h>

#include <wv2/styles.h>
#include <wv2/ustring.h>
#include <wv2/word97_generated.h>
#include <wv2/parser.h>
#include <wv2/parserfactory.h>
#include <wv2/paragraphproperties.h>
#include <wv2/associatedstrings.h>
#include <klocale.h>
#include <KoStore.h>
#include <KoFilterChain.h>


Document::Document( const std::string& fileName, QDomDocument& mainDocument, QDomDocument& documentInfo, QDomElement& framesetsElement, KoFilterChain* chain )
    : m_mainDocument( mainDocument ), m_documentInfo ( documentInfo ),
      m_framesetsElement( framesetsElement ),
      m_replacementHandler( new KWordReplacementHandler ), m_tableHandler( new KWordTableHandler ),
      m_pictureHandler( new KWordPictureHandler( this ) ), m_textHandler( 0 ),
      m_chain( chain ),
      m_parser( wvWare::ParserFactory::createParser( fileName ) ), m_headerFooters( 0 ), m_bodyFound( false ),
      m_footNoteNumber( 0 ), m_endNoteNumber( 0 )
{
    if ( m_parser ) // 0 in case of major error (e.g. unsupported format)
    {
        m_textHandler = new KWordTextHandler( m_parser );
        connect( m_textHandler, SIGNAL( subDocFound( const wvWare::FunctorBase*, int ) ),
                 this, SLOT( slotSubDocFound( const wvWare::FunctorBase*, int ) ) );
        connect( m_textHandler, SIGNAL( tableFound( const KWord::Table& ) ),
                 this, SLOT( slotTableFound( const KWord::Table& ) ) );
        connect( m_textHandler, SIGNAL( pictureFound( const QString&, const QString&, const wvWare::FunctorBase* ) ),
                 this, SLOT( slotPictureFound( const QString&, const QString&, const wvWare::FunctorBase* ) ) );
        m_parser->setSubDocumentHandler( this );
        m_parser->setTextHandler( m_textHandler );
        m_parser->setTableHandler( m_tableHandler );
#ifdef IMAGE_IMPORT
        m_parser->setPictureHandler( m_pictureHandler );
#endif
        m_parser->setInlineReplacementHandler( m_replacementHandler );
        processStyles();
        processAssociatedStrings();
        connect( m_tableHandler, SIGNAL( sigTableCellStart( int, int, int, int, const KoRect&, const QString&, const wvWare::Word97::BRC&, const wvWare::Word97::BRC&, const wvWare::Word97::BRC&, const wvWare::Word97::BRC&, const wvWare::Word97::SHD& ) ),
                 this, SLOT( slotTableCellStart( int, int, int, int, const KoRect&, const QString&, const wvWare::Word97::BRC&, const wvWare::Word97::BRC&, const wvWare::Word97::BRC&, const wvWare::Word97::BRC&, const wvWare::Word97::SHD& ) ) );
        connect( m_tableHandler, SIGNAL( sigTableCellEnd() ),
                 this, SLOT( slotTableCellEnd() ) );
    }
}

Document::~Document()
{
    delete m_textHandler;
    delete m_pictureHandler;
    delete m_tableHandler;
    delete m_replacementHandler;
}

void Document::finishDocument()
{
    const wvWare::Word97::DOP& dop = m_parser->dop();

    QDomElement elementDoc = m_mainDocument.documentElement();

    QDomElement element;
    element = m_mainDocument.createElement("ATTRIBUTES");
    element.setAttribute("processing",0); // WP
    char allHeaders = ( wvWare::HeaderData::HeaderEven |
                        wvWare::HeaderData::HeaderOdd |
                        wvWare::HeaderData::HeaderFirst );
    element.setAttribute("hasHeader", m_headerFooters & allHeaders ? 1 : 0 );
    char allFooters = ( wvWare::HeaderData::FooterEven |
                        wvWare::HeaderData::FooterOdd |
                        wvWare::HeaderData::FooterFirst );
    element.setAttribute("hasFooter", m_headerFooters & allFooters ? 1 : 0 );
    //element.setAttribute("unit","mm"); // How to figure out the unit to use?

    element.setAttribute("tabStopValue", (double)dop.dxaTab / 20.0 );
    elementDoc.appendChild(element);

    element = m_mainDocument.createElement("FOOTNOTESETTING");
    elementDoc.appendChild(element);
    element.setAttribute( "start", dop.nFtn ); // initial footnote number for document. Starts at 1.
    element.setAttribute( "type", Conversion::numberFormatCode( dop.nfcFtnRef2 ) );

    element = m_mainDocument.createElement("ENDNOTESETTING");
    elementDoc.appendChild(element);
    element.setAttribute( "start", dop.nEdn ); // initial endnote number for document. Starts at 1.
    element.setAttribute( "type", Conversion::numberFormatCode( dop.nfcEdnRef2 ) );

    // Done at the end: write the type of headers/footers,
    // depending on which kind of headers and footers we received.
    QDomElement paperElement = elementDoc.namedItem("PAPER").toElement();
    Q_ASSERT ( !paperElement.isNull() ); // slotFirstSectionFound should have been called!
    if ( !paperElement.isNull() )
    {
        kdDebug(30513) << k_funcinfo << "m_headerFooters=" << m_headerFooters << endl;
        paperElement.setAttribute("hType", Conversion::headerMaskToHType( m_headerFooters ) );
        paperElement.setAttribute("fType", Conversion::headerMaskToFType( m_headerFooters ) );
    }

    // Write out <PICTURES> tag
    QDomElement picturesElem = m_mainDocument.createElement("PICTURES");
    elementDoc.appendChild( picturesElem );
    for( QStringList::Iterator it = m_pictureList.begin(); it != m_pictureList.end(); ++it ) {
        QDomElement keyElem = m_mainDocument.createElement("KEY");
        picturesElem.appendChild( keyElem );
        keyElem.setAttribute( "filename", *it );
        keyElem.setAttribute( "name", *it );
    }
}

void Document::processAssociatedStrings() {
    wvWare::AssociatedStrings strings( m_parser->associatedStrings() );

    QDomElement infodoc = m_documentInfo.createElement( "document-info" );
    QDomElement author = m_documentInfo.createElement( "author" );
    QDomElement fullname = m_documentInfo.createElement( "full-name" );
    QDomElement title = m_documentInfo.createElement( "title" );
    QDomElement about = m_documentInfo.createElement( "about" );

    m_documentInfo.appendChild(infodoc);

    if ( !strings.author().isNull()) {
	fullname.appendChild(
	    m_documentInfo.createTextNode(
		Conversion::string (
		    strings.author()
		    ).string()));
	author.appendChild(fullname);
	infodoc.appendChild(author);
    }

    if ( !strings.title().isNull()) {
	title.appendChild(
	    m_documentInfo.createTextNode(
		Conversion::string (
		    strings.title()
		    ).string()));
	about.appendChild(title);
	infodoc.appendChild(about);
    }

}

void Document::processStyles()
{
    QDomElement stylesElem = m_mainDocument.createElement( "STYLES" );
    m_mainDocument.documentElement().appendChild( stylesElem );

    m_textHandler->setFrameSetElement( stylesElem ); /// ### naming!
    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    unsigned int count = styles.size();
    //kdDebug(30513) << k_funcinfo << "styles count=" << count << endl;
    for ( unsigned int i = 0; i < count ; ++i )
    {
        const wvWare::Style* style = styles.styleByIndex( i );
        Q_ASSERT( style );
        //kdDebug(30513) << k_funcinfo << "style " << i << " " << style << endl;
        if ( style && style->type() == wvWare::Style::sgcPara )
        {
            QDomElement styleElem = m_mainDocument.createElement("STYLE");
            stylesElem.appendChild( styleElem );

            QConstString name = Conversion::string( style->name() );
            QDomElement element = m_mainDocument.createElement("NAME");
            element.setAttribute( "value", name.string() );
            styleElem.appendChild( element );

            kdDebug(30513) << k_funcinfo << "Style " << i << ": " << name.string() << endl;

            const wvWare::Style* followingStyle = styles.styleByID( style->followingStyle() );
            if ( followingStyle && followingStyle != style )
            {
                QConstString followingName = Conversion::string( followingStyle->name() );
                element = m_mainDocument.createElement("FOLLOWING");
                element.setAttribute( "name", followingName.string() );
                styleElem.appendChild( element );
            }

            m_textHandler->paragLayoutBegin(); // new style, reset some vars

            // It's important to do that one first, for m_shadowTextFound
            m_textHandler->writeFormat( styleElem, &style->chp(), 0L /*all of it, no ref chp*/, 0, 0, 1, 0L );

            m_textHandler->writeLayout( styleElem, style->paragraphProperties(), style );
        }
        // KWord doesn't support character styles yet
    }
}

bool Document::parse()
{
    if ( m_parser )
        return m_parser->parse();
    return false;
}

void Document::bodyStart()
{
    kdDebug(30513) << k_funcinfo << endl;

    QDomElement mainFramesetElement = m_mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    // TODO: "name" attribute (needs I18N)
    m_framesetsElement.appendChild(mainFramesetElement);

    // Those values are unused. The paper margins make recalcFrames() resize this frame.
    createInitialFrame( mainFramesetElement, 29, 798, 42, 566, false, Reconnect );

    m_textHandler->setFrameSetElement( mainFramesetElement );
    connect( m_textHandler, SIGNAL( firstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ),
             this, SLOT( slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ) );
    m_bodyFound = true;
}

void Document::bodyEnd()
{
    kdDebug(30513) << k_funcinfo << endl;
    disconnect( m_textHandler, SIGNAL( firstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ),
             this, SLOT( slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ) );
}


void Document::slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> sep )
{
    kdDebug(30513) << k_funcinfo << endl;
    QDomElement elementDoc = m_mainDocument.documentElement();

    QDomElement elementPaper = m_mainDocument.createElement("PAPER");
    bool landscape = (sep->dmOrientPage == 2);
    double width = (double)sep->xaPage / 20.0;
    double height = (double)sep->yaPage / 20.0;
    elementPaper.setAttribute("width", width);
    elementPaper.setAttribute("height", height);

    // guessFormat takes millimeters
    width = POINT_TO_MM( width );
    height = POINT_TO_MM( height );
    KoFormat paperFormat = KoPageFormat::guessFormat( landscape ? height : width, landscape ? width : height );
    elementPaper.setAttribute("format",paperFormat);

    elementPaper.setAttribute("orientation", landscape ? PG_LANDSCAPE : PG_PORTRAIT );
    elementPaper.setAttribute("columns", sep->ccolM1 + 1 );
    elementPaper.setAttribute("columnspacing", (double)sep->dxaColumns / 20.0);
    elementPaper.setAttribute("spHeadBody", (double)sep->dyaHdrTop / 20.0);
    elementPaper.setAttribute("spFootBody", (double)sep->dyaHdrBottom / 20.0);
    // elementPaper.setAttribute("zoom",100); // not a doc property in kword
    elementDoc.appendChild(elementPaper);

    QDomElement element = m_mainDocument.createElement("PAPERBORDERS");
    element.setAttribute("left", (double)sep->dxaLeft / 20.0);
    element.setAttribute("top",(double)sep->dyaTop / 20.0);
    element.setAttribute("right", (double)sep->dxaRight / 20.0);
    element.setAttribute("bottom", (double)sep->dyaBottom / 20.0);
    elementPaper.appendChild(element);

    // TODO apply brcTop/brcLeft etc. to the main FRAME
    // TODO use sep->fEndNote to set the 'use endnotes or footnotes' flag
}

void Document::headerStart( wvWare::HeaderData::Type type )
{
    kdDebug(30513) << "startHeader type=" << type << " (" << Conversion::headerTypeToFramesetName( type ) << ")" << endl;
    // Werner says the headers are always emitted in the order of the Type enum.

    QDomElement framesetElement = m_mainDocument.createElement("FRAMESET");
    framesetElement.setAttribute( "frameType", 1 );
    framesetElement.setAttribute( "frameInfo", Conversion::headerTypeToFrameInfo( type ) );
    framesetElement.setAttribute( "name", Conversion::headerTypeToFramesetName( type ) );
    m_framesetsElement.appendChild(framesetElement);

    bool isHeader = Conversion::isHeader( type );

    createInitialFrame( framesetElement, 29, 798, isHeader?0:567, isHeader?41:567+41, true, Copy );

    m_textHandler->setFrameSetElement( framesetElement );

    m_headerFooters |= type;

    /*if ( Conversion::isHeader( type ) )
        m_hasHeader = true;
    else
        m_hasFooter = true;*/
}

void Document::headerEnd()
{
    m_textHandler->setFrameSetElement( QDomElement() );
}

void Document::footnoteStart()
{
    // Grab data that was stored with the functor, that triggered this parsing
    SubDocument subdoc( m_subdocQueue.front() );
    int type = subdoc.data;

    // Create footnote/endnote frameset
    QDomElement framesetElement = m_mainDocument.createElement("FRAMESET");
    framesetElement.setAttribute( "frameType", 1 /* text */ );
    framesetElement.setAttribute( "frameInfo", 7 /* footnote/endnote */ );
    if ( type == wvWare::FootnoteData::Endnote )
        // Keep name in sync with KWordTextHandler::footnoteFound
        framesetElement.setAttribute("name", i18n("Endnote %1").arg( ++m_endNoteNumber ) );
    else
        // Keep name in sync with KWordTextHandler::footnoteFound
        framesetElement.setAttribute("name", i18n("Footnote %1").arg( ++m_footNoteNumber ) );
    m_framesetsElement.appendChild(framesetElement);

    createInitialFrame( framesetElement, 29, 798, 567, 567+41, true, NoFollowup );

    m_textHandler->setFrameSetElement( framesetElement );
}

void Document::footnoteEnd()
{
    kdDebug(30513) << k_funcinfo << endl;
    m_textHandler->setFrameSetElement( QDomElement() );
}

void Document::slotTableCellStart( int row, int column, int rowSpan, int columnSpan, const KoRect& cellRect, const QString& tableName, const wvWare::Word97::BRC& brcTop, const wvWare::Word97::BRC& brcBottom, const wvWare::Word97::BRC& brcLeft, const wvWare::Word97::BRC& brcRight, const wvWare::Word97::SHD& shd )
{
    // Create footnote/endnote frameset
    QDomElement framesetElement = m_mainDocument.createElement("FRAMESET");
    framesetElement.setAttribute( "frameType", 1 /* text */ );
    framesetElement.setAttribute( "frameInfo", 0 /* normal text */ );
    framesetElement.setAttribute( "grpMgr", tableName );
    QString name = i18n("Table_Name Cell row,column", "%1 Cell %2,%3").arg(tableName).arg(row).arg(column);
    framesetElement.setAttribute( "name", name );
    framesetElement.setAttribute( "row", row );
    framesetElement.setAttribute( "col", column );
    framesetElement.setAttribute( "rows", rowSpan );
    framesetElement.setAttribute( "cols", columnSpan );
    m_framesetsElement.appendChild(framesetElement);

    QDomElement frameElem = createInitialFrame( framesetElement, cellRect.left(), cellRect.right(), cellRect.top(), cellRect.bottom(), true, NoFollowup );
    generateFrameBorder( frameElem, brcTop, brcBottom, brcLeft, brcRight, shd );

    m_textHandler->setFrameSetElement( framesetElement );
}

void Document::slotTableCellEnd()
{
    m_textHandler->setFrameSetElement( QDomElement() );
}

QDomElement Document::createInitialFrame( QDomElement& parentFramesetElem, double left, double right, double top, double bottom, bool autoExtend, NewFrameBehavior nfb )
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

void Document::generateFrameBorder( QDomElement& frameElementOut, const wvWare::Word97::BRC& brcTop, const wvWare::Word97::BRC& brcBottom, const wvWare::Word97::BRC& brcLeft, const wvWare::Word97::BRC& brcRight, const wvWare::Word97::SHD& shd )
{
    // Frame borders

    if ( brcTop.ico != 255 && brcTop.dptLineWidth != 255 ) // see tablehandler.cpp
        Conversion::setBorderAttributes( frameElementOut, brcTop, "t" );
    if ( brcBottom.ico != 255 && brcBottom.dptLineWidth != 255 ) // see tablehandler.cpp
        Conversion::setBorderAttributes( frameElementOut, brcBottom, "b" );
    if ( brcLeft.ico != 255 && brcLeft.dptLineWidth != 255 ) // could still be 255, for first column
        Conversion::setBorderAttributes( frameElementOut, brcLeft, "l" );
    if ( brcRight.ico != 255 && brcRight.dptLineWidth != 255 ) // could still be 255, for last column
        Conversion::setBorderAttributes( frameElementOut, brcRight, "r" );

    // Frame background brush (color and fill style)
    if ( shd.icoFore != 0 || shd.icoBack != 0 )
    {
        // If ipat = 0 (solid fill), icoBack is the background color.
        // But otherwise, icoFore is the one we need to set as bkColor
        // (and icoBack is usually white; it's the other colour of the pattern,
        // something that we can't set in Qt apparently).
        int bkColor = shd.ipat ? shd.icoFore : shd.icoBack;
        kdDebug(30513) << "generateFrameBorder: " << " icoFore=" << shd.icoFore << " icoBack=" << shd.icoBack << " ipat=" << shd.ipat << " -> bkColor=" << bkColor << endl;

        // Reverse-engineer MSWord's own hackery: it models various gray levels
        // using dithering. But this looks crappy with Qt. So we go back to a QColor.
        bool grayHack = ( shd.ipat && shd.icoFore == 1 && shd.icoBack == 8 );
        if ( grayHack )
        {
            bool ok;
            int grayLevel = Conversion::ditheringToGray( shd.ipat, &ok );
            if ( ok )
            {
                QColor color( 0, 0, grayLevel, QColor::Hsv );
                QString prefix = "bk";
                frameElementOut.setAttribute( "bkRed", color.red() );
                frameElementOut.setAttribute( "bkBlue", color.blue() );
                frameElementOut.setAttribute( "bkGreen", color.green() );
            }
            else grayHack = false;
        }
        if ( !grayHack )
        {
            Conversion::setColorAttributes( frameElementOut, bkColor, "bk", true );
            // Fill style
            int brushStyle = Conversion::fillPatternStyle( shd.ipat );
            frameElementOut.setAttribute( "bkStyle", brushStyle );
        }
    }
}

void Document::slotSubDocFound( const wvWare::FunctorBase* functor, int data )
{
    SubDocument subdoc( functor, data, QString::null, QString::null );
    m_subdocQueue.push( subdoc );
}

void Document::slotTableFound( const KWord::Table& table )
{
    m_tableQueue.push( table );
}

void Document::slotPictureFound( const QString& frameName, const QString& pictureName,
                                 const wvWare::FunctorBase* pictureFunctor )
{
    SubDocument subdoc( pictureFunctor, 0, frameName, pictureName );
    m_subdocQueue.push( subdoc );
}

void Document::processSubDocQueue()
{
    // Table cells can contain footnotes, and footnotes can contain tables [without footnotes though]
    // This is why we need to repeat until there's nothing more do to (#79024)
    while ( !m_subdocQueue.empty() || !m_tableQueue.empty() )
    {
        while ( !m_subdocQueue.empty() )
        {
            SubDocument subdoc( m_subdocQueue.front() );
            Q_ASSERT( subdoc.functorPtr );
            (*subdoc.functorPtr)(); // call it
            delete subdoc.functorPtr; // delete it
            m_subdocQueue.pop();
        }
        while ( !m_tableQueue.empty() )
        {
            KWord::Table& table = m_tableQueue.front();
            m_tableHandler->tableStart( &table );
            QValueList<KWord::Row> &rows = table.rows;
            for( QValueList<KWord::Row>::Iterator it = rows.begin(); it != rows.end(); ++it ) {
                KWord::TableRowFunctorPtr f = (*it).functorPtr;
                Q_ASSERT( f );
                (*f)(); // call it
                delete f; // delete it
            }
            m_tableHandler->tableEnd();
            m_tableQueue.pop();
        }
    }
}

KoStoreDevice* Document::createPictureFrameSet( const KoSize& size )
{
    // Grab data that was stored with the functor, that triggered this parsing
    SubDocument subdoc( m_subdocQueue.front() );

    QDomElement framesetElement = m_mainDocument.createElement("FRAMESET");
    framesetElement.setAttribute( "frameType", 2 /*picture*/ );
    framesetElement.setAttribute( "frameInfo", 0 );
    framesetElement.setAttribute( "name", subdoc.name );
    m_framesetsElement.appendChild(framesetElement);

    // The position doesn't matter as long as the picture is inline
    // FIXME for non-inline pics ####
    // To determine the size, look at OOo's filter (WW8PicDesc in ww8graf2.cxx, version 1.50, line 406)
    // Hint: #i17200#, a bit of guesswork I'm afraid
    //        if (aPic.dxaGoal == 1000 && aPic.mx == 1)  //100% hack ? (from ww8graf2.cxx)
    createInitialFrame( framesetElement, 0, size.width(), 0, size.height(), false, NoFollowup );

    QDomElement pictureElem = m_mainDocument.createElement("PICTURE");
    framesetElement.appendChild( pictureElem );

    QDomElement keyElem = m_mainDocument.createElement("KEY");
    pictureElem.appendChild( keyElem );
    keyElem.setAttribute( "filename", subdoc.extraName );
    m_pictureList.append( subdoc.extraName );

    kdDebug(30513) << "Preparing to write picture for '" << subdoc.name << "' into " << subdoc.extraName << endl;
    return m_chain->storageFile( subdoc.extraName, KoStore::Write );
}


#include "document.moc"
