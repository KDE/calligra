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

#include "texthandler.h"
#include "conversion.h"

#include <styles.h>
#include <lists.h>
#include <paragraphproperties.h>
#include <functor.h>
#include <functordata.h>
#include <ustring.h>
#include <parser.h>

#include <qfont.h>
#include <qfontinfo.h>
#include <kdebug.h>


wvWare::U8 KWordReplacementHandler::hardLineBreak()
{
    return '\n';
}

wvWare::U8 KWordReplacementHandler::nonBreakingHyphen()
{
    return '-'; // normal hyphen for now
}

wvWare::U8 KWordReplacementHandler::nonRequiredHyphen()
{
    return 0xad; // soft hyphen, according to kword.dtd
}


KWordTextHandler::KWordTextHandler( wvWare::SharedPtr<wvWare::Parser> parser )
    : m_parser( parser ), m_sectionNumber( 0 ), m_currentStyle( 0L ),
      m_shadowTextFound( false ), m_index( 0 )
{
}

void KWordTextHandler::sectionStart( wvWare::SharedPtr<const wvWare::Word97::SEP> sep )
{
    m_sectionNumber++;

    if ( m_sectionNumber == 1 )
    {
        // KWord doesn't support a different paper format per section.
        // So we use the paper format of the first section,
        // and we apply it to the whole document.
        emit firstSectionFound( sep );
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

void KWordTextHandler::sectionEnd()
{

}

void KWordTextHandler::pageBreak()
{
    // Check if PAGEBREAKING already exists (e.g. due to linesTogether)
    QDomElement pageBreak = m_oldLayout.namedItem( "PAGEBREAKING" ).toElement();
    if ( pageBreak.isNull() )
    {
        pageBreak = mainDocument().createElement( "PAGEBREAKING" );
        m_oldLayout.appendChild( pageBreak );
    }
    pageBreak.setAttribute( "hardFrameBreakAfter", "true" );
}

void KWordTextHandler::headersFound( const wvWare::HeaderFunctor& parseHeaders )
{
    // Currently we only care about headers in the first section
    if ( m_sectionNumber == 1 )
    {
        emit subDocFound( parseHeaders );
    }
}

void KWordTextHandler::paragLayoutBegin()
{
    m_shadowTextFound = false;
}

void KWordTextHandler::paragraphStart( wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties )
{
    //kdDebug() << "paragraphStart. style index:" << paragraphProperties->pap().istd << endl;
    m_formats = mainDocument().createElement( "FORMATS" );
    m_paragraphProperties = paragraphProperties;
    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    m_currentStyle = styles.styleByIndex( paragraphProperties->pap().istd );
    Q_ASSERT( m_currentStyle );
    paragLayoutBegin();
    // If the style's format includes shadowtext, then we need a <SHADOW> tag
    // in the parag layout
    if ( m_currentStyle && m_currentStyle->chp().fShadow )
        m_shadowTextFound = true;
}

void KWordTextHandler::paragraphEnd()
{
    if ( m_currentStyle ) {
        QConstString styleName = Conversion::string( m_currentStyle->name() );
        writeOutParagraph( styleName.string(), m_paragraph );
    } else
        writeOutParagraph( "Standard", m_paragraph );
}

void KWordTextHandler::runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    QConstString newText( Conversion::string( text ) );
    kdDebug() << "runOfText: " << newText.string() << endl;
    m_paragraph += newText.string();

    writeFormat( m_formats, chp, m_currentStyle ? &m_currentStyle->chp() : 0, m_index, text.length() );

    m_index += text.length();
}

void KWordTextHandler::writeFormat( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, int pos, int len )
{
    QDomElement format( mainDocument().createElement( "FORMAT" ) );
    format.setAttribute( "id", 1 );
    format.setAttribute( "pos", pos );
    format.setAttribute( "len", len );

    if ( !refChp || refChp->ico != chp->ico )
    {
        QColor color = Conversion::color( chp->ico, -1 );
        QDomElement colorElem( mainDocument().createElement( "COLOR" ) );
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
            QDomElement fontElem( mainDocument().createElement( "FONT" ) );
            fontElem.setAttribute( "name", fontName );
            format.appendChild( fontElem );
        }
    }

    if ( !refChp || refChp->hps != chp->hps )
    {
        //kdDebug() << "        font size: " << chp->hps/2 << endl;
        QDomElement fontSize( mainDocument().createElement( "SIZE" ) );
        fontSize.setAttribute( "value", (int)(chp->hps / 2) ); // hps is in half points
        format.appendChild( fontSize );
    }

    if ( !refChp || refChp->fBold != chp->fBold ) {
        QDomElement weight( mainDocument().createElement( "WEIGHT" ) );
        weight.setAttribute( "value", chp->fBold ? 75 : 50 );
        format.appendChild( weight );
    }
    if ( !refChp || refChp->fItalic != chp->fItalic ) {
        QDomElement italic( mainDocument().createElement( "ITALIC" ) );
        italic.setAttribute( "value", chp->fItalic ? 1 : 0 );
        format.appendChild( italic );
    }
    if ( !refChp || refChp->kul != chp->kul ) {
        QDomElement underline( mainDocument().createElement( "UNDERLINE" ) );
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
        QDomElement strikeout( mainDocument().createElement( "STRIKEOUT" ) );
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
        QDomElement vertAlign( mainDocument().createElement( "VERTALIGN" ) );
        // Obviously the values are reversed between the two file formats :)
        int kwordVAlign = (chp->iss==1 ? 2 : chp->iss==2 ? 1 : 0);
        vertAlign.setAttribute( "value", kwordVAlign );
        format.appendChild( vertAlign );
    }

    // background color is known as "highlight" in msword
    if ( !refChp || refChp->fHighlight != chp->fHighlight || refChp->icoHighlight != chp->icoHighlight ) {
        QDomElement bgcolElem( mainDocument().createElement( "TEXTBACKGROUNDCOLOR" ) );
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
        QDomElement weight( mainDocument().createElement( "SHADOWTEXT" ) );
        weight.setAttribute( "value", chp->fShadow ? "1" : "0" );
        format.appendChild( weight );
        if ( chp->fShadow )
            m_shadowTextFound = true;
    }

    if ( !format.firstChild().isNull() ) // Don't save an empty format tag
        parentElement.appendChild( format );
}

//#define FONT_DEBUG

// Return the name of a font. We have to convert the Microsoft font names to
// something that might just be present under X11.
QString KWordTextHandler::getFont(unsigned fc) const
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

void KWordTextHandler::writeOutParagraph( const QString& styleName, const QString& text )
{
    QDomElement paragraphElementOut=mainDocument().createElement("PARAGRAPH");
    m_framesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=mainDocument().createElement("TEXT");
    paragraphElementOut.appendChild(textElement);
    paragraphElementOut.appendChild( m_formats );
    QDomElement layoutElement=mainDocument().createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement nameElement = mainDocument().createElement("NAME");
    nameElement.setAttribute("value", styleName);
    layoutElement.appendChild(nameElement);

    if ( m_paragraphProperties )
    {
        // Write out the properties of the paragraph
        writeLayout( layoutElement, *m_paragraphProperties, m_currentStyle );
    }

    textElement.appendChild(mainDocument().createTextNode(text));

    m_paragraph = QString( "" );
    m_index = 0;
    m_oldLayout = layoutElement; // Keep a reference to the old layout for some hacks
}

void KWordTextHandler::writeLayout( QDomElement& parentElement, const wvWare::ParagraphProperties& paragraphProperties, const wvWare::Style* style )
{
    const wvWare::Word97::PAP& pap = paragraphProperties.pap();
    // Always write out the alignment, it's required
    QDomElement flowElement = mainDocument().createElement("FLOW");
    QString alignment = Conversion::alignment( pap.jc );
    flowElement.setAttribute( "align", alignment );
    parentElement.appendChild( flowElement );

    //kdDebug() << k_funcinfo << " dxaLeft1=" << pap.dxaLeft1 << " dxaLeft=" << pap.dxaLeft << " dxaRight=" << pap.dxaRight << " dyaBefore=" << pap.dyaBefore << " dyaAfter=" << pap.dyaAfter << " lspd=" << pap.lspd.dyaLine << "/" << pap.lspd.fMultLinespace << endl;

    if ( pap.dxaLeft1 || pap.dxaLeft || pap.dxaRight )
    {
        QDomElement indentsElement = mainDocument().createElement("INDENTS");
        // 'first' is relative to 'left' in both formats
        indentsElement.setAttribute( "first", (double)pap.dxaLeft1 / 20.0 );
        indentsElement.setAttribute( "left", (double)pap.dxaLeft / 20.0 );
        indentsElement.setAttribute( "right", (double)pap.dxaRight / 20.0 );
        parentElement.appendChild( indentsElement );
    }
    if ( pap.dyaBefore || pap.dyaAfter )
    {
        QDomElement offsetsElement = mainDocument().createElement("OFFSETS");
        offsetsElement.setAttribute( "before", (double)pap.dyaBefore / 20.0 );
        offsetsElement.setAttribute( "after", (double)pap.dyaAfter / 20.0 );
        parentElement.appendChild( offsetsElement );
    }

    // Linespacing
    QString lineSpacing = Conversion::lineSpacing( pap.lspd );
    if ( lineSpacing != "0" )
    {
        QDomElement lineSpacingElem = mainDocument().createElement( "LINESPACING" );
        lineSpacingElem.setAttribute("value", lineSpacing );
        parentElement.appendChild( lineSpacingElem );
    }

    if ( pap.fKeep || pap.fKeepFollow || pap.fPageBreakBefore )
    {
        QDomElement pageBreak = mainDocument().createElement( "PAGEBREAKING" );
        if ( pap.fKeep )
            pageBreak.setAttribute("linesTogether", "true");
        if ( pap.fPageBreakBefore )
            pageBreak.setAttribute("hardFrameBreak", "true" );
        if ( pap.fKeepFollow )
            pageBreak.setAttribute("keepWithNext", "true" );
        parentElement.appendChild( pageBreak );
    }

    // Borders
    if ( pap.brcTop.brcType )
    {
        QDomElement borderElement = mainDocument().createElement( "TOPBORDER" );
        Conversion::setBorderAttributes( borderElement, pap.brcTop );
        parentElement.appendChild( borderElement );
    }
    if ( pap.brcBottom.brcType )
    {
        QDomElement borderElement = mainDocument().createElement( "BOTTOMBORDER" );
        Conversion::setBorderAttributes( borderElement, pap.brcBottom );
        parentElement.appendChild( borderElement );
    }
    if ( pap.brcLeft.brcType )
    {
        QDomElement borderElement = mainDocument().createElement( "LEFTBORDER" );
        Conversion::setBorderAttributes( borderElement, pap.brcLeft );
        parentElement.appendChild( borderElement );
    }
    if ( pap.brcRight.brcType )
    {
        QDomElement borderElement = mainDocument().createElement( "RIGHTBORDER" );
        Conversion::setBorderAttributes( borderElement, pap.brcRight );
        parentElement.appendChild( borderElement );
    }

    // Tabulators
    if ( pap.itbdMac )
    {
        for ( int i = 0 ; i < pap.itbdMac ; ++i )
        {
            const wvWare::Word97::TabDescriptor &td = pap.rgdxaTab[i];
            QDomElement tabElement = mainDocument().createElement( "TABULATOR" );
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

    if ( pap.ilfo > 0 )
    {
        writeCounter( parentElement, paragraphProperties, style );
    }

    if ( m_shadowTextFound )
    {
        // SHADOW - if any SHADOWTEXT was generated, generate <SHADOW> with hardcoded
        // values that make it look like in MSWord.
        QDomElement shadowElement = mainDocument().createElement( "SHADOW" );
        shadowElement.setAttribute( "distance", 1 );
        shadowElement.setAttribute( "direction", 5 ); // bottom right
        shadowElement.setAttribute( "red", 190 );
        shadowElement.setAttribute( "blue", 190 );
        shadowElement.setAttribute( "green", 190 );
        parentElement.appendChild( shadowElement );
    }
}

void KWordTextHandler::writeCounter( QDomElement& parentElement, const wvWare::ParagraphProperties& paragraphProperties, const wvWare::Style* style )
{
    const wvWare::ListInfo* listInfo = paragraphProperties.listInfo();
    Q_ASSERT( listInfo );
    if ( !listInfo )
        return;

#ifndef NDEBUG
    listInfo->dump();
#endif

    QDomElement counterElement = mainDocument().createElement( "COUNTER" );
    // numbering type: 0==list 1==chapter. First we determine it for word6 docs.
    // But we can also activate it if the text() looks that way
    int numberingType = listInfo->isWord6() && listInfo->prev() ? 1 : 0;
    wvWare::UString text = listInfo->text().text;
    int nfc = listInfo->numberFormat();
    if ( nfc == 23 ) // bullet
    {
        if ( text.length() == 1 )
        {
            unsigned int code = text[0].unicode();
            if ( (code & 0xFF00) == 0xF000 ) // see wv2
                code &= 0x00FF;
            // Some well-known bullet codes. Better turn them into real
            // KWord bullets, it looks much nicer (than crappy fonts).
            if ( code == 0xB7 ) // Round black bullet
            {
                counterElement.setAttribute( "type", 10 ); // disc bullet
            } else if ( code == 0xD8 ) // Triangle
            {
                counterElement.setAttribute( "type", 11 ); // Box. We have no triangle.
            } else {
                // Map all other bullets to a "custom bullet" in kword.
                kdDebug() << "custom bullet, code=" << QString::number(code,16) << endl;
                counterElement.setAttribute( "type", 6 ); // custom
                counterElement.setAttribute( "bullet", code );
                QString paragFont = getFont( style->chp().ftcAscii );
                counterElement.setAttribute( "bulletfont", paragFont );
            }
        } else
            kdWarning() << "Bullet with more than one character, not supported" << endl;
    }
    else
    {
        counterElement.setAttribute( "start", listInfo->startAt() );
        if ( listInfo->startAtOverridden() )
            counterElement.setAttribute( "restart", "true" );
        const wvWare::Word97::PAP& pap = paragraphProperties.pap();
        int depth = pap.ilvl; /*both are 0 based*/
        // Heading styles don't set the ilvl, but must have a depth coming
        // from their heading level (the style's STI)
        bool isHeading = style->sti() >= 1 && style->sti() <= 9;
        if ( depth == 0 && isHeading )
        {
            depth = style->sti() - 1;
        }
        kdDebug() << "  ilfo=" << pap.ilfo << " ilvl=" << pap.ilvl << " sti=" << style->sti() << " depth=" << depth << " numberingType=" << numberingType << endl;
        counterElement.setAttribute( "depth", depth );

        // Now we need to parse the text, to try and convert msword's powerful list template
        // stuff, into what KWord can do right now.
        QString prefix, suffix;
        bool depthFound = false;
        bool otherDepthFound = false;
        // We parse <0>.<2>.<1>. as "level 2 with suffix='.'" (no prefix)
        // But "Section <0>)" has both prefix and suffix.
        for ( int i = 0 ; i < text.length() ; ++i )
        {
            short ch = text[i].unicode();
            //kdDebug() << i << ":" << ch << endl;
            if ( ch < 10 ) { // List level place holder
                if ( ch == pap.ilvl ) {
                    if ( depthFound )
                        kdWarning() << "ilvl " << pap.ilvl << " found twice in listInfo text..." << endl;
                    else
                        depthFound = true;
                    suffix = QString::null;
                } else {
                    otherDepthFound = true;
                    prefix = QString::null; // get rid of previous prefixes
                }
            } else { // Normal character
                if ( depthFound )
                    suffix += QChar(ch);
                else
                    prefix += QChar(ch);
            }
        }
        if ( otherDepthFound )
        {
            // This is a hierarchical list numbering e.g. <1>.<0>.
            // (unless this is about a heading, in which case we've set numberingtype to 1 already
            // so it will indeed look like that).
            // Instead of importing this as ".<0>.", we drop the prefix,
            // we assume it's part of the upper level's counter text
            prefix = QString::null;
            if ( isHeading )
                numberingType = 1;
        }
        if ( depthFound )
        {
            // Word6 models "1." as nfc=5
            if ( nfc == 5 && suffix.isEmpty() )
                suffix = ".";
            kdDebug() << " prefix=" << prefix << " suffix=" << suffix << endl;
            counterElement.setAttribute( "type", Conversion::numberFormatCode( nfc ) );
            counterElement.setAttribute( "lefttext", prefix );
            counterElement.setAttribute( "righttext", suffix );
        }
        else
        {
            kdWarning() << "Not supported: counter text without the depth in it" << endl;
        }
        // listInfo->alignment() is not supported in KWord
        // listInfo->isLegal() hmm
        // listInfo->notRestarted() [by higher level of lists] not supported
        // listInfo->followingchar() ignored, it's always a space in KWord currently
    }
    counterElement.setAttribute( "numberingtype", numberingType );
    parentElement.appendChild( counterElement );
}

void KWordTextHandler::setFrameSetElement( const QDomElement& frameset )
{
    m_framesetElement = frameset;
}

QDomDocument KWordTextHandler::mainDocument() const
{
    return m_framesetElement.ownerDocument();
}

#include "texthandler.moc"
