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
#include <ustring.h>
#include <word97_generated.h>
#include <parser.h>
#include <parserfactory.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <qregexp.h>


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
      m_charHandler( new KWordCharacterHandler ), m_parser( wvWare::ParserFactory::createParser( fileName ) )
{
    if ( m_parser ) {
        m_parser->setSpecialCharacterHandler( m_charHandler );
        m_parser->setBodyTextHandler( this );
    }
}

Document::~Document()
{
    delete m_charHandler;
}

bool Document::parse()
{
    if ( m_parser )
        return m_parser->parse();
    return false;
}

void Document::paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap )
{
    m_formats = m_mainDocument.createElement( "FORMATS" );
    m_pap = pap;
}

void Document::paragraphEnd()
{
    // TODO: get style name (and properties) from pap.istd, in paragraphStart
    writeOutParagraph( "Standard", m_paragraph );
}

void Document::runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    QConstString newTextStr( reinterpret_cast<const QChar*>( text.data() ), text.length() );
    QString newText = newTextStr.string();
    kdDebug() << "runOfText: " << newText << endl;
    encodeText( newText );

    m_paragraph += newText;


    QDomElement format( m_mainDocument.createElement( "FORMAT" ) );
    format.setAttribute( "id", 1 );
    format.setAttribute( "pos", m_index );
    format.setAttribute( "len", text.length() );

    // TODO: change the if()s below, to add attributes if different from paragraph format
    // (not if different from 'plain text')
    // This is also why the code below seems to test stuff twice ;)

    QColor color = Conversion::color( chp->ico, -1 );
    QDomElement colorElem( m_mainDocument.createElement( "COLOR" ) );
    colorElem.setAttribute( "red", color.red() );
    colorElem.setAttribute( "blue", color.blue() );
    colorElem.setAttribute( "green", color.green() );
    format.appendChild( colorElem );

    // Font name
    // TBD: We only use the Ascii font code. We should work out how/when to use the FE and Other font codes.
    QString fontName = getFont( chp->ftcAscii );

    if ( !fontName.isEmpty() )
    {
        QDomElement fontElem( m_mainDocument.createElement( "FONT" ) );
        fontElem.setAttribute( "name", fontName );
        format.appendChild( fontElem );
    }

    kdDebug() << "        font size: " << chp->hps/2 << endl;
    QDomElement fontSize( m_mainDocument.createElement( "SIZE" ) );
    fontSize.setAttribute( "value", (int)(chp->hps / 2) ); // hps is in half points
    format.appendChild( fontSize );

    if ( chp->fBold ) {
        QDomElement weight( m_mainDocument.createElement( "WEIGHT" ) );
        weight.setAttribute( "value", chp->fBold ? 75 : 50 );
        format.appendChild( weight );
    }
    if ( chp->fItalic ) {
        QDomElement italic( m_mainDocument.createElement( "ITALIC" ) );
        italic.setAttribute( "value", chp->fItalic ? 1 : 0 );
        format.appendChild( italic );
    }
    if ( chp->kul ) {
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
    if ( chp->fStrike || chp->fDStrike ) {
        kdDebug() << "fStrike=" << chp->fStrike << " chp->fDStrike=" << chp->fDStrike << endl;
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

    if ( chp->iss ) { // superscript/subscript
        QDomElement vertAlign( m_mainDocument.createElement( "VERTALIGN" ) );
        // Obviously the values are reversed between the two file formats :)
        int kwordVAlign = (chp->iss==1 ? 2 : chp->iss==2 ? 1 : 0);
        vertAlign.setAttribute( "value", kwordVAlign );
        format.appendChild( vertAlign );
    }

    // TODO: TEXTBACKGROUNDCOLOR - what's the msword name for it? Can't find it in the CHP.

    // ## Problem with fShadow. Char property in MSWord, parag property in KWord at the moment....

    if ( !format.firstChild().isNull() ) // Don't save an empty format tag
        m_formats.appendChild( format );

    m_index += text.length();
}

//#define FONT_DEBUG

// Return the name of a font. We have to convert the Microsoft font names to
// something that might just be present under X11.
QString Document::getFont(unsigned fc) const
{
    Q_ASSERT( m_parser );
    if ( !m_parser )
        return QString::null;
    const wvWare::Word97::FFN* ffn = m_parser->font( fc );
    Q_ASSERT( ffn );
    if ( !ffn )
        return QString::null;

    QConstString fontName( reinterpret_cast<const QChar*>( ffn->xszFfn.data() ), ffn->xszFfn.length() );
    QString font = fontName.string();

//#ifdef FONT_DEBUG
    kdDebug() << "    MS-FONT: " << font << endl;
//#endif

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

void Document::encodeText( QString &text )
{
    // When encoding the stored form of text to its run-time form,
    // be sure to do the conversion for "&amp;" to "&" first to avoid
    // accidentally converting user text into one of the other escape
    // sequences.

    text.replace(QRegExp("&"), "&amp;");
    text.replace(QRegExp("<"), "&lt;");

    // Strictly, there is no need to encode >, but we do so to for safety.

    text.replace(QRegExp(">"), "&gt;");

    // Strictly, there is no need to encode " or ', but we do so to allow
    // them to co-exist!

    text.replace(QRegExp("\""), "&quot;");
    text.replace(QRegExp("'"), "&apos;");
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

    if ( m_pap ) {
        QDomElement flowElement = m_mainDocument.createElement("FLOW");
        QString alignment = Conversion::alignment( m_pap->jc );
        flowElement.setAttribute("align",alignment);
        layoutElement.appendChild(flowElement);

        kdDebug() << k_funcinfo << " dxaLeft1=" << m_pap->dxaLeft1 << " dxaLeft=" << m_pap->dxaLeft << " dxaRight=" << m_pap->dxaRight << " dyaBefore=" << m_pap->dyaBefore << " dyaAfter=" << m_pap->dyaAfter << " lspd=" << m_pap->lspd.dyaLine << "/" << m_pap->lspd.fMultLinespace << endl;

        // TODO: INDENTS dxaRight dxaLeft dxaLeft1 - in which unit are those?
        // TODO: OFFSETS dyaBefore dyaAfter

        // Linespacing
        QString lineSpacing = Conversion::lineSpacing( m_pap->lspd );
        if ( lineSpacing != "0" ) // ##
        {
            QDomElement lineSpacingElem = m_mainDocument.createElement( "LINESPACING" );
            lineSpacingElem.setAttribute("value", lineSpacing );
            layoutElement.appendChild( lineSpacingElem );
        }

        if ( m_pap->fKeep || m_pap->fKeepFollow || m_pap->fPageBreakBefore )
        {
            QDomElement pageBreak = m_mainDocument.createElement( "PAGEBREAKING" );
            pageBreak.setAttribute("linesTogether", m_pap->fKeep ? "true" : "false");
            pageBreak.setAttribute("hardFrameBreak", m_pap->fPageBreakBefore ? "true" : "false");
            pageBreak.setAttribute("keepWithNext", m_pap->fKeepFollow ? "true" : "false");
            layoutElement.appendChild( pageBreak );
        }

        // TODO: LEFTBORDER|RIGHTBORDER|TOPBORDER|BOTTOMBORDER  - see pap.brcl brcTop brcLeft brcBottom brcRight

        // TODO: COUNTER
        // TODO: FORMAT - unless it all comes from the style
        // TODO: SHADOW [it comes from the text runs...]
        // TODO: TABULATORs itbdMac? (why "Mac"?) rgdxaTab[] rgtbd[]
    }

    textElement.appendChild(m_mainDocument.createTextNode(text));
    //textElement.normalize(); // Put text together (not sure if needed)
    // DF: I don't think so, you created only one text node ;)

    m_paragraph = QString( "" );
    m_index = 0;
    m_oldLayout = layoutElement; // Keep a reference to the old layout for some hacks
}
