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

#include "texthandler.h"
#include "conversion.h"

#include <wv2/styles.h>
#include <wv2/lists.h>
#include <wv2/paragraphproperties.h>
#include <wv2/functor.h>
#include <wv2/functordata.h>
#include <wv2/ustring.h>
#include <wv2/parser.h>
#include <wv2/fields.h>

#include <QFont>
#include <qfontinfo.h>
#include <kdebug.h>
#include <klocale.h>


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
    : m_parser( parser ), m_sectionNumber( 0 ), m_footNoteNumber( 0 ), m_endNoteNumber( 0 ),
      m_previousOutlineLSID( 0 ), m_previousEnumLSID( 0 ),
      m_currentStyle( 0L ), m_index( 0 ),
      m_currentTable( 0L ),
      m_bInParagraph( false ),
      m_insideField( false ), m_fieldAfterSeparator( false ), m_fieldType( 0 )
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
        emit subDocFound( new wvWare::HeaderFunctor( parseHeaders ), 0 );
    }
}

void KWordTextHandler::footnoteFound( wvWare::FootnoteData::Type type,
                                      wvWare::UChar character, wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                                      const wvWare::FootnoteFunctor& parseFootnote )
{
    bool autoNumbered = (character.unicode() == 2);
    QDomElement varElem = insertVariable( 11 /*KWord code for footnotes*/, chp, "STRI" );
    QDomElement footnoteElem = varElem.ownerDocument().createElement( "FOOTNOTE" );
    if ( autoNumbered )
        footnoteElem.setAttribute( "value", 1 ); // KWord will renumber anyway
    else
        footnoteElem.setAttribute( "value", QString(QChar(character.unicode())) );
    footnoteElem.setAttribute( "notetype", type == wvWare::FootnoteData::Endnote ? "endnote" : "footnote" );
    footnoteElem.setAttribute( "numberingtype", autoNumbered ? "auto" : "manual" );
    if ( type == wvWare::FootnoteData::Endnote )
        // Keep name in sync with Document::startFootnote
        footnoteElem.setAttribute( "frameset", i18n("Endnote %1", ++m_endNoteNumber ) );
    else
        // Keep name in sync with Document::startFootnote
        footnoteElem.setAttribute( "frameset", i18n("Footnote %1", ++m_footNoteNumber ) );
    varElem.appendChild( footnoteElem );

    // Remember to parse the footnote text later
    emit subDocFound( new wvWare::FootnoteFunctor( parseFootnote ), type );
}

QDomElement KWordTextHandler::insertVariable( int type, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString& format )
{
    m_paragraph += '#';

    QDomElement formatElem;
    writeFormat( m_formats, chp, m_currentStyle ? &m_currentStyle->chp() : 0, m_index, 1, 4 /*id*/, &formatElem );

    m_index += 1;

    QDomElement varElem = m_formats.ownerDocument().createElement( "VARIABLE" );
    QDomElement typeElem = m_formats.ownerDocument().createElement( "TYPE" );
    typeElem.setAttribute( "type", type );
    typeElem.setAttribute( "key", format );
    varElem.appendChild( typeElem );
    formatElem.appendChild( varElem );
    return varElem;
}

void KWordTextHandler::tableRowFound( const wvWare::TableRowFunctor& functor, wvWare::SharedPtr<const wvWare::Word97::TAP> tap )
{
    if ( !m_currentTable )
    {
        // We need to put the table in a paragraph. For wv2 tables are between paragraphs.
        Q_ASSERT( !m_bInParagraph );
        paragraphStart( 0L );
        static int s_tableNumber = 0;
        m_currentTable = new KWord::Table();
        m_currentTable->name = i18n("Table %1", ++s_tableNumber );
        insertAnchor( m_currentTable->name );
    }

    // Add all cell edges to our array.
    for (int i = 0; i <= tap->itcMac; i++)
        m_currentTable->cacheCellEdge( tap->rgdxaCenter[ i ] );

    KWord::Row row( new wvWare::TableRowFunctor( functor ), tap );
    m_currentTable->rows.append( row );
}

#ifdef IMAGE_IMPORT
void KWordTextHandler::pictureFound( const wvWare::PictureFunctor& pictureFunctor,
                                     wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
                                     wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/ )
{
    static unsigned int s_pictureNumber = 0;
    QString pictureName = "pictures/picture";
    pictureName += QString::number( s_pictureNumber ); // filenames start at 0
    // looks better to the user if frame names start at 1
    QString frameName = i18n("Picture %1", ++s_pictureNumber );
    insertAnchor( frameName );

    switch ( picf->mfp.mm ) {
        case 98:
            pictureName += ".tif"; // not implemented!
            break;
        case 99:
            pictureName += ".bmp";
            break;
        default:
            pictureName += ".wmf";
            break;
    }

    emit pictureFound( frameName, pictureName, new wvWare::PictureFunctor( pictureFunctor ) );
}
#endif // IMAGE_IMPORT

QDomElement KWordTextHandler::insertAnchor( const QString& fsname )
{
    m_paragraph += '#';

    // Can't call writeFormat, we have no chp.
    QDomElement format( mainDocument().createElement( "FORMAT" ) );
    format.setAttribute( "id", 6 );
    format.setAttribute( "pos", m_index );
    format.setAttribute( "len", 1 );
    m_formats.appendChild( format );
    QDomElement formatElem = format;

    m_index += 1;

    QDomElement anchorElem = m_formats.ownerDocument().createElement( "ANCHOR" );
    anchorElem.setAttribute( "type", "frameset" );
    anchorElem.setAttribute( "instance", fsname );
    formatElem.appendChild( anchorElem );
    return anchorElem;
}

void KWordTextHandler::paragLayoutBegin()
{
}

void KWordTextHandler::paragraphStart( wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties )
{
    if ( m_bInParagraph )
        paragraphEnd();
    m_bInParagraph = true;
    //kDebug(30513) << "paragraphStart. style index:" << paragraphProperties->pap().istd << endl;
    m_formats = mainDocument().createElement( "FORMATS" );
    m_paragraphProperties = paragraphProperties;
    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    m_currentStyle = 0;
    if ( paragraphProperties ) // Always set when called by wv2. But not set when called by tableStart.
    {
        m_currentStyle = styles.styleByIndex( paragraphProperties->pap().istd );
        Q_ASSERT( m_currentStyle );
    }
    paragLayoutBegin();
}

void KWordTextHandler::paragraphEnd()
{
    Q_ASSERT( m_bInParagraph );
    if ( m_currentTable )
    {
        emit tableFound( *m_currentTable );
        delete m_currentTable;
        m_currentTable = 0L;
    }
    if ( m_currentStyle ) {
        QConstString styleName = Conversion::string( m_currentStyle->name() );
        writeOutParagraph( styleName.string(), m_paragraph );
    } else
        writeOutParagraph( "Standard", m_paragraph );
    m_bInParagraph = false;
}

void KWordTextHandler::fieldStart( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/ )
{
    m_fieldType = Conversion::fldToFieldType( fld );
    m_insideField = true;
    m_fieldAfterSeparator = false;
    m_fieldValue = "";
}

void KWordTextHandler::fieldSeparator( const wvWare::FLD* /*fld*/, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/ )
{
    m_fieldAfterSeparator = true;
}

void KWordTextHandler::fieldEnd( const wvWare::FLD* /*fld*/, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    // only for handled field type
    if( m_fieldType >= 0 )
    {
        QDomElement varElem = insertVariable( 8, chp, "STRING" );
        QDomElement fieldElem = varElem.ownerDocument().createElement( "FIELD" );
        fieldElem.setAttribute( "subtype", m_fieldType );
        fieldElem.setAttribute( "value", m_fieldValue );
        varElem.appendChild( fieldElem );
    }

    // reset
    m_fieldValue = "";
    m_fieldType = -1;
    m_insideField = false;
    m_fieldAfterSeparator = false;
}

void KWordTextHandler::runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    QConstString newText( Conversion::string( text ) );
    //kDebug(30513) << "runOfText: " << newText.string() << endl;

    // text after fieldStart and before fieldSeparator is useless
    if( m_insideField && !m_fieldAfterSeparator ) return;

    // if we can handle the field, consume the text
    if( m_insideField && m_fieldAfterSeparator && ( m_fieldType >= 0 ) )
    {
        m_fieldValue.append( newText.string() );
        return;
    }

    m_paragraph += newText.string();

    writeFormat( m_formats, chp, m_currentStyle ? &m_currentStyle->chp() : 0, m_index, text.length(), 1, 0L );

    m_index += text.length();

}

void KWordTextHandler::writeFormat( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, int pos, int len, int formatId, QDomElement* pChildElement )
{
    QDomElement format( mainDocument().createElement( "FORMAT" ) );
    format.setAttribute( "id", formatId );
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
        //kDebug(30513) << "        font size: " << chp->hps/2 << endl;
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
        case 11: // wave
            underline.setAttribute( "styleline", "wave" );
            break;
        case 5: // hidden - This makes no sense as an underline property!
            val = "0";
            break;
        case 1: // single
        case 2: // by word - TODO
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

    // font attribute (uppercase, lowercase (not in MSWord), small caps)
    if ( !refChp || refChp->fCaps != chp->fCaps || refChp->fSmallCaps != chp->fSmallCaps )
    {
        QDomElement fontAttrib( mainDocument().createElement( "FONTATTRIBUTE" ) );
        fontAttrib.setAttribute( "value", chp->fSmallCaps ? "smallcaps" : chp->fCaps ? "uppercase" : "none" );
        format.appendChild( fontAttrib );
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
    if ( !refChp || refChp->fShadow != chp->fShadow || refChp->fImprint != chp->fImprint ) {
        QDomElement shadowElem( mainDocument().createElement( "SHADOW" ) );
        QString css = "none";
        // Generate a shadow with hardcoded values that make it look like in MSWord.
        // We need to make the distance depend on the font size though, to look good
        if (chp->fShadow || chp->fImprint)
        {
            int fontSize = (int)(chp->hps / 2);
            int dist = fontSize > 20 ? 2 : 1;
            if (chp->fImprint) // ## no real support for imprint, we use a topleft shadow
                dist = -dist;
            css = QString::fromLatin1("#bebebe %1pt %1pt").arg(dist).arg(dist);
        }
        shadowElem.setAttribute( "text-shadow", css );
        format.appendChild( shadowElem );
    }

    if ( pChildElement || !format.firstChild().isNull() ) // Don't save an empty format tag, unless the caller asked for it
        parentElement.appendChild( format );
    if ( pChildElement )
        *pChildElement = format;
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
    kDebug(30513) << "    MS-FONT: " << font << endl;
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
    kDebug(30513) << "    FUZZY-FONT: " << font << endl;
#endif

    // Use Qt to look up our canonical equivalent of the font name.
    QFont xFont( font );
    QFontInfo info( xFont );

#ifdef FONT_DEBUG
    kDebug(30513) << "    QT-FONT: " << info.family() << endl;
#endif

    return info.family();
}

void KWordTextHandler::writeOutParagraph( const QString& styleName, const QString& text )
{
    if ( m_framesetElement.isNull() )
    {
        if ( !text.isEmpty() ) // vertically merged table cells are ignored, and have empty text -> no warning on those
            kWarning(30513) << "KWordTextHandler: no frameset element to write to! text=" << text << endl;
        return;
    }
    QDomElement paragraphElementOut=mainDocument().createElement("PARAGRAPH");
    m_framesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=mainDocument().createElement("TEXT");
    textElement.setAttribute( "xml:space", "preserve" );
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

    //kDebug(30513) << k_funcinfo << " dxaLeft1=" << pap.dxaLeft1 << " dxaLeft=" << pap.dxaLeft << " dxaRight=" << pap.dxaRight << " dyaBefore=" << pap.dyaBefore << " dyaAfter=" << pap.dyaAfter << " lspd=" << pap.lspd.dyaLine << "/" << pap.lspd.fMultLinespace << endl;

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
            //kDebug(30513) << "ptpos=" << (double)td.dxaTab / 20.0 << endl;
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
}

void KWordTextHandler::writeCounter( QDomElement& parentElement, const wvWare::ParagraphProperties& paragraphProperties, const wvWare::Style* style )
{
    const wvWare::ListInfo* listInfo = paragraphProperties.listInfo();
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
                kDebug(30513) << "custom bullet, code=" << QString::number(code,16) << endl;
                counterElement.setAttribute( "type", 6 ); // custom
                counterElement.setAttribute( "bullet", code );
                QString paragFont = getFont( style->chp().ftcAscii );
                counterElement.setAttribute( "bulletfont", paragFont );
            }
        } else
            kWarning(30513) << "Bullet with more than one character, not supported" << endl;
    }
    else
    {
        const wvWare::Word97::PAP& pap = paragraphProperties.pap();
        counterElement.setAttribute( "start", listInfo->startAt() );

        int depth = pap.ilvl; /*both are 0 based*/
        // Heading styles don't set the ilvl, but must have a depth coming
        // from their heading level (the style's STI)
        bool isHeading = style->sti() >= 1 && style->sti() <= 9;
        if ( depth == 0 && isHeading )
        {
            depth = style->sti() - 1;
        }
        kDebug(30513) << "  ilfo=" << pap.ilfo << " ilvl=" << pap.ilvl << " sti=" << style->sti() << " depth=" << depth << " numberingType=" << numberingType << endl;
        counterElement.setAttribute( "depth", depth );

        // Now we need to parse the text, to try and convert msword's powerful list template
        // stuff, into what KWord can do right now.
        QString prefix, suffix;
        bool depthFound = false;
        int displayLevels = 1;
        // We parse <0>.<2>.<1>. as "level 2 with suffix='.'" (no prefix)
        // But "Section <0>)" has both prefix and suffix.
        // The common case is <0>.<1>.<2> (display-levels=3)
        for ( int i = 0 ; i < text.length() ; ++i )
        {
            short ch = text[i].unicode();
            //kDebug(30513) << i << ":" << ch << endl;
            if ( ch < 10 ) { // List level place holder
                if ( ch == pap.ilvl ) {
                    if ( depthFound )
                        kWarning(30513) << "ilvl " << pap.ilvl << " found twice in listInfo text..." << endl;
                    else
                        depthFound = true;
                    suffix = QString::null;
                } else {
                    Q_ASSERT( ch < pap.ilvl ); // Can't see how level 1 would have a <0> in it...
                    if ( ch < pap.ilvl )
                        ++displayLevels; // we found a 'parent level', to be displayed
                    prefix = QString::null; // get rid of previous prefixes
                }
            } else { // Normal character
                if ( depthFound )
                    suffix += QChar(ch);
                else
                    prefix += QChar(ch);
            }
        }
        if ( displayLevels > 1 )
        {
            // This is a hierarchical list numbering e.g. <0>.<1>.
            // (unless this is about a heading, in which case we've set numberingtype to 1 already
            // so it will indeed look like that).
            // The question is whether the '.' is the suffix of the parent level already..
            if ( depth > 0 && !prefix.isEmpty() && m_listSuffixes[ depth - 1 ] == prefix )  {
                prefix = QString::null; // it's already the parent's suffix -> remove it
                kDebug(30513) << "depth=" << depth << " parent suffix is " << prefix << " -> clearing" << endl;
            }
        }
        if ( isHeading )
            numberingType = 1;
        if ( depthFound )
        {
            // Word6 models "1." as nfc=5
            if ( nfc == 5 && suffix.isEmpty() )
                suffix = ".";
            kDebug(30513) << " prefix=" << prefix << " suffix=" << suffix << endl;
            counterElement.setAttribute( "type", Conversion::numberFormatCode( nfc ) );
            counterElement.setAttribute( "lefttext", prefix );
            counterElement.setAttribute( "righttext", suffix );
            counterElement.setAttribute( "display-levels", displayLevels );
            kDebug(30513) << "storing suffix " << suffix << " for depth " << depth << endl;
            m_listSuffixes[ depth ] = suffix;
        }
        else
        {
            kWarning(30513) << "Not supported: counter text without the depth in it:" << Conversion::string(text).string() << endl;
        }

        if ( listInfo->startAtOverridden() ||
             ( numberingType == 1 && m_previousOutlineLSID != 0 && m_previousOutlineLSID != listInfo->lsid() ) ||
             ( numberingType == 0 &&m_previousEnumLSID != 0 && m_previousEnumLSID != listInfo->lsid() ) )
            counterElement.setAttribute( "restart", "true" );

        // listInfo->alignment() is not supported in KWord
        // listInfo->isLegal() hmm
        // listInfo->notRestarted() [by higher level of lists] not supported
        // listInfo->followingchar() ignored, it's always a space in KWord currently
    }
    if ( numberingType == 1 )
        m_previousOutlineLSID = listInfo->lsid();
    else
        m_previousEnumLSID = listInfo->lsid();
    counterElement.setAttribute( "numberingtype", numberingType );
    parentElement.appendChild( counterElement );
}

void KWordTextHandler::setFrameSetElement( const QDomElement& frameset )
{
    m_framesetElement = frameset;
    for ( uint i = 0 ; i < 9 ; ++i )
        m_listSuffixes[i] = QString::null;
}

QDomDocument KWordTextHandler::mainDocument() const
{
    return m_framesetElement.ownerDocument();
}

#include "texthandler.moc"
