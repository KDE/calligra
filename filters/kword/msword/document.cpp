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

#include "document.h"
#include "conversion.h"

#include <kdebug.h>

#include <styles.h>
#include <lists.h>
#include <ustring.h>
#include <word97_generated.h>
#include <parser.h>
#include <parserfactory.h>

#include <qfont.h>
#include <qfontinfo.h>
#include <koGlobal.h>


wvWare::U8 KWordCharacterHandler::hardLineBreak()
{
    return '\n';
}

wvWare::U8 KWordCharacterHandler::nonBreakingHyphen()
{
    return '-'; // normal hyphen for now
}

wvWare::U8 KWordCharacterHandler::nonRequiredHyphen()
{
    return 0xad; // soft hyphen, according to kword.dtd
}


Document::Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement )
    : m_mainDocument( mainDocument ), m_mainFramesetElement( mainFramesetElement ), m_index( 0 ),
      m_sectionNumber( 0 ), m_paragStyle( 0L ), m_charHandler( new KWordCharacterHandler ),
      m_parser( wvWare::ParserFactory::createParser( fileName ) )
{
    if ( m_parser ) { // 0 in case of major error (e.g. unsupported format)
        m_parser->setSpecialCharacterHandler( m_charHandler );
        m_parser->setBodyTextHandler( this );
        prepareDocument();
        processStyles();
    }
}

Document::~Document()
{
    delete m_charHandler;
}

void Document::prepareDocument()
{
    const wvWare::Word97::DOP& dop = m_parser->dop();

    QDomElement elementDoc = m_mainDocument.documentElement();

    QDomElement element;
    element = m_mainDocument.createElement("ATTRIBUTES");
    element.setAttribute("processing",0); // WP
    element.setAttribute("hasHeader",0); // TODO
    element.setAttribute("hasFooter",0); // TODO
    element.setAttribute("unit","mm"); // How to figure out the unit to use?

    element.setAttribute("tabStopValue", (double)dop.dxaTab / 20.0 );
    elementDoc.appendChild(element);

    // FOOTNOTESETTING: use nfcFtnRef for the type of counter
    // Hmm there's nfcFtnRef2 too.
}

void Document::processStyles()
{
    QDomElement stylesElem = m_mainDocument.createElement( "STYLES" );
    m_mainDocument.documentElement().appendChild( stylesElem );

    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    unsigned int count = styles.size();
    //kdDebug() << k_funcinfo << "styles count=" << count << endl;
    for ( unsigned int i = 0; i < count ; ++i )
    {
        const wvWare::Style* style = styles.styleByIndex( i );
        Q_ASSERT( style );
        //kdDebug() << k_funcinfo << "style " << i << " " << style << endl;
        if ( style && style->type() == wvWare::Style::sgcPara )
        {
            QDomElement styleElem = m_mainDocument.createElement("STYLE");
            stylesElem.appendChild( styleElem );

            QConstString name = Conversion::string( style->name() );
            QDomElement element = m_mainDocument.createElement("NAME");
            element.setAttribute( "value", name.string() );
            styleElem.appendChild( element );

            //kdDebug() << k_funcinfo << "Style: " << name.string() << endl;

            const wvWare::Style* followingStyle = styles.styleByID( style->followingStyle() );
            if ( followingStyle && followingStyle != style )
            {
                QConstString followingName = Conversion::string( followingStyle->name() );
                element = m_mainDocument.createElement("FOLLOWING");
                element.setAttribute( "name", followingName.string() );
                styleElem.appendChild( element );
            }

#warning Disabled for now
////////////            writeLayout( styleElem, &style->pap() );

            writeFormat( styleElem, &style->chp(), 0L /*all of it, no ref chp*/, 0, 0 );
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

void Document::sectionStart()
{
    wvWare::SharedPtr<const wvWare::Word97::SEP> sep = m_parser->currentSep();

    m_sectionNumber++;

    if ( m_sectionNumber == 1 )
    {
        // KWord doesn't support a different paper format per section.
        // So we use the paper format of the first section,
        // and we apply it to the whole document.

        QDomElement elementDoc = m_mainDocument.documentElement();

        QDomElement elementPaper = m_mainDocument.createElement("PAPER");
        elementPaper.setAttribute("width", (double)sep->xaPage / 20.0);
        elementPaper.setAttribute("height", (double)sep->yaPage / 20.0);
        // TODO: guess paper formats from the size :}
        KoFormat paperFormat = PG_DIN_A4;
        elementPaper.setAttribute("format",paperFormat);
        elementPaper.setAttribute("orientation", sep->dmOrientPage == 2 ? PG_LANDSCAPE : PG_PORTRAIT );
        elementPaper.setAttribute("columns",1); // TODO
        elementPaper.setAttribute("columnspacing", (double)sep->dxaColumns / 20.0);
        elementPaper.setAttribute("hType",0); // TODO
        elementPaper.setAttribute("fType",0); // TODO
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
    else
    {
        // Not the first section. Check for a page break
        if ( sep->bkc >= 1 ) // 1=new column, 2=new page, 3=even page, 4=odd page
        {
            pageBreak();
        }
    }
}

void Document::sectionEnd()
{

}

void Document::paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap )
{
    m_formats = m_mainDocument.createElement( "FORMATS" );
    m_pap = pap;
    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    m_paragStyle = styles.styleByIndex( m_pap->istd );
    Q_ASSERT( m_paragStyle );
}

void Document::paragraphEnd()
{
    if ( m_paragStyle ) {
        QConstString styleName = Conversion::string( m_paragStyle->name() );
        writeOutParagraph( styleName.string(), m_paragraph );
    } else
        writeOutParagraph( "Standard", m_paragraph );
}

void Document::runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    QConstString newText( Conversion::string( text ) );
    kdDebug() << "runOfText: " << newText.string() << endl;
    m_paragraph += newText.string();

    writeFormat( m_formats, chp, m_paragStyle ? &m_paragStyle->chp() : 0, m_index, text.length() );

    m_index += text.length();
}

void Document::writeFormat( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, int pos, int len )
{
    QDomElement format( m_mainDocument.createElement( "FORMAT" ) );
    format.setAttribute( "id", 1 );
    format.setAttribute( "pos", pos );
    format.setAttribute( "len", len );

    if ( !refChp || refChp->ico != chp->ico )
    {
        QColor color = Conversion::color( chp->ico, -1 );
        QDomElement colorElem( m_mainDocument.createElement( "COLOR" ) );
        colorElem.setAttribute( "red", color.red() );
        colorElem.setAttribute( "blue", color.blue() );
        colorElem.setAttribute( "green", color.green() );
        format.appendChild( colorElem );
    }

    // Font name
    // TBD: We only use the Ascii font code. We should work out how/when to use the FE and Other font codes.
    if ( !refChp || refChp->ftcAscii != chp->ftcAscii )
    {
        QString fontName = getFont( chp->ftcAscii );

        if ( !fontName.isEmpty() )
        {
            QDomElement fontElem( m_mainDocument.createElement( "FONT" ) );
            fontElem.setAttribute( "name", fontName );
            format.appendChild( fontElem );
        }
    }

    if ( !refChp || refChp->hps != chp->hps )
    {
        //kdDebug() << "        font size: " << chp->hps/2 << endl;
        QDomElement fontSize( m_mainDocument.createElement( "SIZE" ) );
        fontSize.setAttribute( "value", (int)(chp->hps / 2) ); // hps is in half points
        format.appendChild( fontSize );
    }

    if ( !refChp || refChp->fBold != chp->fBold ) {
        QDomElement weight( m_mainDocument.createElement( "WEIGHT" ) );
        weight.setAttribute( "value", chp->fBold ? 75 : 50 );
        format.appendChild( weight );
    }
    if ( !refChp || refChp->fItalic != chp->fItalic ) {
        QDomElement italic( m_mainDocument.createElement( "ITALIC" ) );
        italic.setAttribute( "value", chp->fItalic ? 1 : 0 );
        format.appendChild( italic );
    }
    if ( !refChp || refChp->kul != chp->kul ) {
        QDomElement underline( m_mainDocument.createElement( "UNDERLINE" ) );
        QString val = (chp->kul == 0) ? "0" : "1";
        switch ( chp->kul ) {
        case 3: // double
            underline.setAttribute( "styleline", "solid" );
            val = "double";
            break;
        case 6: // thick
            underline.setAttribute( "styleline", "solid" );
            val = "single-bold";
            break;
        case 7:
            underline.setAttribute( "styleline", "dash" );
            break;
        case 4: // dotted
        case 8: // dot (not used, says the docu)
            underline.setAttribute( "styleline", "dot" );
            break;
        case 9:
            underline.setAttribute( "styleline", "dashdot" );
            break;
        case 10:
            underline.setAttribute( "styleline", "dashdotdot" );
            break;
        case 1: // single
        case 2: // by word - TODO in kword
        case 5: // hidden - WTH is that?
        case 11: // wave - not in kword
        default:
            underline.setAttribute( "styleline", "solid" );
        };
        underline.setAttribute( "value", val );
        format.appendChild( underline );
    }
    if ( !refChp || refChp->fStrike != chp->fStrike || refChp->fDStrike != chp->fDStrike ) {
        QDomElement strikeout( m_mainDocument.createElement( "STRIKEOUT" ) );
        if ( chp->fDStrike ) // double strikethrough
        {
            strikeout.setAttribute( "value", "double" );
            strikeout.setAttribute( "styleline", "solid" );
        }
        else if ( chp->fStrike )
        {
            strikeout.setAttribute( "value", "single" );
            strikeout.setAttribute( "styleline", "solid" );
        }
        else
            strikeout.setAttribute( "value", "0" );
        format.appendChild( strikeout );
    }

    if ( !refChp || refChp->iss != chp->iss ) { // superscript/subscript
        QDomElement vertAlign( m_mainDocument.createElement( "VERTALIGN" ) );
        // Obviously the values are reversed between the two file formats :)
        int kwordVAlign = (chp->iss==1 ? 2 : chp->iss==2 ? 1 : 0);
        vertAlign.setAttribute( "value", kwordVAlign );
        format.appendChild( vertAlign );
    }

    // background color is known as "highlight" in msword
    if ( !refChp || refChp->fHighlight != chp->fHighlight || refChp->icoHighlight != chp->icoHighlight ) {
        QDomElement bgcolElem( m_mainDocument.createElement( "TEXTBACKGROUNDCOLOR" ) );
        if ( chp->fHighlight )
        {
            QColor color = Conversion::color( chp->icoHighlight, -1 );
            bgcolElem.setAttribute( "red", color.red() );
            bgcolElem.setAttribute( "blue", color.blue() );
            bgcolElem.setAttribute( "green", color.green() );
        } else {
            bgcolElem.setAttribute( "red", -1 );
            bgcolElem.setAttribute( "blue", -1 );
            bgcolElem.setAttribute( "green", -1 );
        }
        format.appendChild( bgcolElem );
    }
    // Shadow text. Only on/off. The properties are defined at the paragraph level (in KWord).
    if ( !refChp || refChp->fShadow != chp->fShadow ) {
        QDomElement weight( m_mainDocument.createElement( "SHADOWTEXT" ) );
        weight.setAttribute( "value", "1" );
        format.appendChild( weight );
    }

    if ( !format.firstChild().isNull() ) // Don't save an empty format tag
        parentElement.appendChild( format );
}

//#define FONT_DEBUG

// Return the name of a font. We have to convert the Microsoft font names to
// something that might just be present under X11.
QString Document::getFont(unsigned fc) const
{
    Q_ASSERT( m_parser );
    if ( !m_parser )
        return QString::null;
    const wvWare::Word97::FFN& ffn ( m_parser->font( fc ) );

    QConstString fontName( Conversion::string( ffn.xszFfn ) );
    QString font = fontName.string();

#ifdef FONT_DEBUG
    kdDebug() << "    MS-FONT: " << font << endl;
#endif

    static const unsigned ENTRIES = 6;
    static const char* const fuzzyLookup[ENTRIES][2] =
    {
        // MS contains      X11 font family
        // substring.       non-Xft name.
        { "times",          "times" },
        { "courier",        "courier" },
        { "andale",         "monotype" },
        { "monotype.com",   "monotype" },
        { "georgia",        "times" },
        { "helvetica",      "helvetica" }
    };

    // When Xft is available, Qt will do a good job of looking up our local
    // equivalent of the MS font. But, we want to work even without Xft.
    // So, first, we do a fuzzy match of some common MS font names.
    unsigned i;

    for (i = 0; i < ENTRIES; i++)
    {
        // The loop will leave unchanged any MS font name not fuzzy-matched.
        if (font.find(fuzzyLookup[i][0], 0, FALSE) != -1)
        {
            font = fuzzyLookup[i][1];
            break;
        }
    }

#ifdef FONT_DEBUG
    kdDebug() << "    FUZZY-FONT: " << font << endl;
#endif

    // Use Qt to look up our canonical equivalent of the font name.
    QFont xFont( font );
    QFontInfo info( xFont );

#ifdef FONT_DEBUG
    kdDebug() << "    QT-FONT: " << info.family() << endl;
#endif

    return info.family();
}

void Document::pageBreak()
{
    // Check if PAGEBREAKING already exists (e.g. due to linesTogether)
    QDomElement pageBreak = m_oldLayout.namedItem( "PAGEBREAKING" ).toElement();
    if ( pageBreak.isNull() )
    {
        pageBreak = m_mainDocument.createElement( "PAGEBREAKING" );
        m_oldLayout.appendChild( pageBreak );
    }
    pageBreak.setAttribute( "hardFrameBreakAfter", "true" );
}

void Document::writeOutParagraph( const QString& styleName, const QString& text )
{
    QDomElement paragraphElementOut=m_mainDocument.createElement("PARAGRAPH");
    m_mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=m_mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElement);
    paragraphElementOut.appendChild( m_formats );
    QDomElement layoutElement=m_mainDocument.createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement nameElement = m_mainDocument.createElement("NAME");
    nameElement.setAttribute("value", styleName);
    layoutElement.appendChild(nameElement);

    if ( m_pap )
    {
        // Write out the properties of the paragraph
        writeLayout( layoutElement, m_pap );
    }

    textElement.appendChild(m_mainDocument.createTextNode(text));

    m_paragraph = QString( "" );
    m_index = 0;
    m_oldLayout = layoutElement; // Keep a reference to the old layout for some hacks
}

void Document::writeLayout( QDomElement& parentElement, const wvWare::Word97::PAP* pap )
{
    // Always write out the alignment, it's required
    QDomElement flowElement = m_mainDocument.createElement("FLOW");
    QString alignment = Conversion::alignment( pap->jc );
    flowElement.setAttribute( "align", alignment );
    parentElement.appendChild( flowElement );

    //kdDebug() << k_funcinfo << " dxaLeft1=" << pap->dxaLeft1 << " dxaLeft=" << pap->dxaLeft << " dxaRight=" << pap->dxaRight << " dyaBefore=" << pap->dyaBefore << " dyaAfter=" << pap->dyaAfter << " lspd=" << pap->lspd.dyaLine << "/" << pap->lspd.fMultLinespace << endl;

    if ( pap->dxaLeft1 || pap->dxaLeft || pap->dxaRight )
    {
        QDomElement indentsElement = m_mainDocument.createElement("INDENTS");
        // 'first' is relative to 'left' in both formats
        indentsElement.setAttribute( "first", (double)pap->dxaLeft1 / 20.0 );
        indentsElement.setAttribute( "left", (double)pap->dxaLeft / 20.0 );
        indentsElement.setAttribute( "right", (double)pap->dxaRight / 20.0 );
        parentElement.appendChild( indentsElement );
    }
    if ( pap->dyaBefore || pap->dyaAfter )
    {
        QDomElement offsetsElement = m_mainDocument.createElement("OFFSETS");
        offsetsElement.setAttribute( "before", (double)pap->dyaBefore / 20.0 );
        offsetsElement.setAttribute( "after", (double)pap->dyaAfter / 20.0 );
        parentElement.appendChild( offsetsElement );
    }

    // Linespacing
    QString lineSpacing = Conversion::lineSpacing( pap->lspd );
    if ( lineSpacing != "0" )
    {
        QDomElement lineSpacingElem = m_mainDocument.createElement( "LINESPACING" );
        lineSpacingElem.setAttribute("value", lineSpacing );
        parentElement.appendChild( lineSpacingElem );
    }

    if ( pap->fKeep || pap->fKeepFollow || pap->fPageBreakBefore )
    {
        QDomElement pageBreak = m_mainDocument.createElement( "PAGEBREAKING" );
        if ( pap->fKeep )
            pageBreak.setAttribute("linesTogether", "true");
        if ( pap->fPageBreakBefore )
            pageBreak.setAttribute("hardFrameBreak", "true" );
        if ( pap->fKeepFollow )
            pageBreak.setAttribute("keepWithNext", "true" );
        parentElement.appendChild( pageBreak );
    }

    // Borders
    if ( pap->brcTop.brcType )
    {
        QDomElement borderElement = m_mainDocument.createElement( "TOPBORDER" );
        Conversion::setBorderAttributes( borderElement, pap->brcTop );
        parentElement.appendChild( borderElement );
    }
    if ( pap->brcBottom.brcType )
    {
        QDomElement borderElement = m_mainDocument.createElement( "BOTTOMBORDER" );
        Conversion::setBorderAttributes( borderElement, pap->brcBottom );
        parentElement.appendChild( borderElement );
    }
    if ( pap->brcLeft.brcType )
    {
        QDomElement borderElement = m_mainDocument.createElement( "LEFTBORDER" );
        Conversion::setBorderAttributes( borderElement, pap->brcLeft );
        parentElement.appendChild( borderElement );
    }
    if ( pap->brcRight.brcType )
    {
        QDomElement borderElement = m_mainDocument.createElement( "RIGHTBORDER" );
        Conversion::setBorderAttributes( borderElement, pap->brcRight );
        parentElement.appendChild( borderElement );
    }

    // Tabulators
    if ( pap->itbdMac )
    {
        for ( int i = 0 ; i < pap->itbdMac ; ++i )
        {
            const wvWare::Word97::TabDescriptor &td = pap->rgdxaTab[i];
            QDomElement tabElement = m_mainDocument.createElement( "TABULATOR" );
            tabElement.setAttribute( "ptpos", (double)td.dxaTab / 20.0 );
            //kdDebug() << "ptpos=" << (double)td.dxaTab / 20.0 << endl;
            // Wow, lucky here. The type enum matches. Only, MSWord has 4=bar,
            // which kword doesn't support. We map it to 0 with a clever '%4' :)
            tabElement.setAttribute( "type", td.tbd.jc % 4 );
            int filling = 0;
            double width = 0.5; // default kword value, see koparaglayout.cc
            switch ( td.tbd.tlc ) {
            case 1: // dots
            case 2: // hyphenated
                filling = 1; // KWord: dots
                break;
            case 3: // single line
                filling = 2; // KWord: line
                break;
            case 4: // heavy line
                filling = 2; // KWord: line
                width = 2; // make it heavy. To be tested.
            }
            tabElement.setAttribute( "filling", filling );
            tabElement.setAttribute( "width", width );
            parentElement.appendChild( tabElement );
        }
    }

    if ( pap->ilfo > 0 )
    {
        const wvWare::ListInfoProvider& listInfo = m_parser->listInfo();
        QDomElement counterElement = m_mainDocument.createElement( "COUNTER" );
        // simplelist -> 'list numbering', otherwise 'chapter numbering'
        counterElement.setAttribute( "numberingtype", listInfo.isSimpleList() ? "0" : "1" );
        counterElement.setAttribute( "start", listInfo.startAt() );
        // Now we need to parse the text, to try and convert msword's powerful list template
        // stuff, into what KWord can do right now.
        int depth = pap->ilvl; /*both are 0 based*/
        counterElement.setAttribute( "depth", depth );
        wvWare::UString text = listInfo.text().text;
        QString prefix, suffix;
        bool depthFound = false;
        // We parse <0>.<2>.<1>. as "level 2 with suffix='.'" (no prefix)
        // But "Section <0>)" has both prefix and suffix.
        for ( int i = 0 ; i < text.length() ; ++i )
        {
            short ch = text[i].unicode();
            kdDebug() << i << ":" << ch << endl;
            if ( ch < 10 ) { // List level place holder
                if ( ch == depth ) {
                    if ( depthFound )
                        kdWarning() << "Depth " << depth << " found twice in listInfo text..." << endl;
                    else
                        depthFound = true;
                    suffix = QString::null;
                } else {
                    prefix = QString::null; // get rid of previous prefixes
                }
            } else { // Normal character
                if ( depthFound )
                    suffix += QChar(ch);
                else
                    prefix += QChar(ch);
            }
        }
        if ( depthFound )
        {
            kdDebug() << " prefix=" << prefix << " suffix=" << suffix << " nfc=" << listInfo.numberFormat()<< endl;
            counterElement.setAttribute( "type", Conversion::numberFormatCode( listInfo.numberFormat() ) );
            counterElement.setAttribute( "lefttext", prefix );
            counterElement.setAttribute( "righttext", suffix );
        }
        else
        {
            // The number isn't in the string -> this isn't a numbered list, more like a bullet
            // MSWord doesn't use special list types for bullets (like KWord does);
            // instead it uses a char from the current font.
            // So we map all bullets to a "custom bullet" in kword.
            if ( text.length() == 1 )
            {
                int code = text[0].unicode();
                if ( code & 0xFF00 == 0xF000 ) // see wv2
                    code &= 0x00FF;
                counterElement.setAttribute( "type", 6 ); // custom
                counterElement.setAttribute( "bullet", code );
                QString paragFont = getFont( m_paragStyle->chp().ftcAscii );
                counterElement.setAttribute( "bulletfont", paragFont );
            }
            else
                kdWarning() << "Not supported: counter text without the depth in it, and longer than 1 char" << endl;
        }
        // listInfo.alignment() is not supported in KWord
        // listInfo.isLegal() hmm
        // listInfo.notRestarted() hmm
        // listInfo.prev() is assumed to be true for paragraph numbering.... :}
        // listInfo.followingchar() ignored, it's always a space in KWord currently
        parentElement.appendChild( counterElement );

    }
    // TODO: COUNTER
    // TODO? FORMAT - unless it all comes from the style, or is all specified for all chars
    // TODO: SHADOW - if any SHADOWTEXT was generated, generate <SHADOW> with hardcoded
    // values that make it look like in MSWord.
}
