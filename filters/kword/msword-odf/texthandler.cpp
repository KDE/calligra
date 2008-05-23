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


KWordTextHandler::KWordTextHandler( wvWare::SharedPtr<wvWare::Parser> parser, KoXmlWriter* contentWriter, KoXmlWriter* bodyWriter )
    : m_parser( parser ), m_sectionNumber( 0 ), m_footNoteNumber( 0 ), m_endNoteNumber( 0 ),
      m_textStyleNumber( 1 ), m_paragraphStyleNumber( 1 ),
      m_previousOutlineLSID( 0 ), m_previousEnumLSID( 0 ),
      m_currentStyle( 0L ), m_index( 0 ),
      m_currentTable( 0L ),
      m_bInParagraph( false ), m_bStartNewPage( false ),
      m_insideField( false ), m_fieldAfterSeparator( false ), m_fieldType( 0 )
{
    m_contentWriter = contentWriter; //set the pointer to contentWriter for add styles
    m_bodyWriter = bodyWriter; //set the pointer to bodyWriter for writing text
}

//increment m_sectionNumber
//emit firstSectionFound or check for pagebreak
void KWordTextHandler::sectionStart( wvWare::SharedPtr<const wvWare::Word97::SEP> sep )
{
    kDebug(30513);
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
            //pageBreak();
	    m_bStartNewPage = true; //tell us to start new page on next element we write
	    kDebug(30513) << "b_StartNewPage = true";
        }
    }
}

void KWordTextHandler::sectionEnd()
{
    kDebug(30513) ;
}

//void KWordTextHandler::pageBreak()
//{
    // Check if PAGEBREAKING already exists (e.g. due to linesTogether)
    //QDomElement pageBreak = m_oldLayout.namedItem( "PAGEBREAKING" ).toElement();
    //if ( pageBreak.isNull() )
    //{
        //pageBreak = mainDocument().createElement( "PAGEBREAKING" );
    //    m_oldLayout.appendChild( pageBreak );
    //}
    //pageBreak.setAttribute( "hardFrameBreakAfter", "true" );

//}

//signal that there's another subDoc to parse
void KWordTextHandler::headersFound( const wvWare::HeaderFunctor& parseHeaders )
{
    kDebug(30513) ;
    // Currently we only care about headers in the first section
    if ( m_sectionNumber == 1 )
    {
        emit subDocFound( new wvWare::HeaderFunctor( parseHeaders ), 0 );
    }
}

//handles in text part of the footnote, but then parses text of it later?
void KWordTextHandler::footnoteFound( wvWare::FootnoteData::Type type,
                                      wvWare::UChar character, wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                                      const wvWare::FootnoteFunctor& parseFootnote )
{
    kDebug(30513) ;
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

//create an element for the variable
QDomElement KWordTextHandler::insertVariable( int type, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString& format )
{
    kDebug(30513) ;
    m_paragraph += '#';

    QDomElement formatElem;
    writeFormattedText( m_formats, chp, m_currentStyle ? &m_currentStyle->chp() : 0, m_index, 1, 4 /*id*/, &formatElem );

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
    kDebug(30513) ;
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
    kDebug(30513) ;
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
    kDebug(30513) ;
    m_paragraph += '#';

    // Can't call writeFormat, we have no chp.
    //QDomElement format( mainDocument().createElement( "FORMAT" ) );
    //format.setAttribute( "id", 6 );
    //format.setAttribute( "pos", m_index );
    //format.setAttribute( "len", 1 );
    //m_formats.appendChild( format );
    //QDomElement formatElem = format;

    m_index += 1;

    //QDomElement anchorElem = m_formats.ownerDocument().createElement( "ANCHOR" );
    //anchorElem.setAttribute( "type", "frameset" );
    //anchorElem.setAttribute( "instance", fsname );
    //formatElem.appendChild( anchorElem );
    return QDomElement();
}

void KWordTextHandler::paragLayoutBegin()
{
    kDebug(30513) ;
}

//sets m_paragraphProperties to the passed-in paragraphProperties
//sets m_currentStyle with PAP->istd (index to STSH structure)
void KWordTextHandler::paragraphStart( wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties )
{
    kDebug(30513) ;
    if ( m_bInParagraph )
        paragraphEnd();
    m_bInParagraph = true;
    //kDebug(30513) <<"paragraphStart. style index:" << paragraphProperties->pap().istd;
    //m_formats = mainDocument().createElement( "FORMATS" );
    m_paragraphProperties = paragraphProperties;
    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    m_currentStyle = 0;
    if ( paragraphProperties ) // Always set when called by wv2. But not set when called by tableStart.
    {
        m_currentStyle = styles.styleByIndex( paragraphProperties->pap().istd );
        Q_ASSERT( m_currentStyle );
	QConstString styleName = Conversion::string( m_currentStyle->name() );
        writeOutParagraph( styleName.string(), m_paragraph );
    }
    else
	writeOutParagraph( "Standard", m_paragraph );
    paragLayoutBegin();
}

void KWordTextHandler::paragraphEnd()
{
    kDebug(30513) ;
    Q_ASSERT( m_bInParagraph );
    if ( m_currentTable )
    {
        emit tableFound( *m_currentTable );
        delete m_currentTable;
        m_currentTable = 0L;
    }
    //we're writing the paragraph style at the beginning of the paragraph now! (see paragraphStart())
    //if ( m_currentStyle ) {
        //QConstString styleName = Conversion::string( m_currentStyle->name() );
        //writeOutParagraph( styleName.string(), m_paragraph );
    //} else
        //writeOutParagraph( "Standard", m_paragraph );
    m_bodyWriter->endElement(); //close the <text:p> tag we opened in writeLayout()
    m_bInParagraph = false;
}

void KWordTextHandler::fieldStart( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/ )
{
    kDebug(30513) ;
    m_fieldType = Conversion::fldToFieldType( fld );
    m_insideField = true;
    m_fieldAfterSeparator = false;
    m_fieldValue = "";
}

void KWordTextHandler::fieldSeparator( const wvWare::FLD* /*fld*/, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/ )
{
    kDebug(30513) ;
    m_fieldAfterSeparator = true;
}

void KWordTextHandler::fieldEnd( const wvWare::FLD* /*fld*/, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    kDebug(30513) ;
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

//this handles a basic section of text
void KWordTextHandler::runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    QConstString newText( Conversion::string( text ) );
    kDebug(30513) << newText.string();

    // text after fieldStart and before fieldSeparator is useless
    if( m_insideField && !m_fieldAfterSeparator ) return;

    // if we can handle the field, consume the text
    if( m_insideField && m_fieldAfterSeparator && ( m_fieldType >= 0 ) )
    {
        m_fieldValue.append( newText.string() );
        return;
    }

    m_runOfText += newText.string();

    //write the text with its formatting inside a <text:span> tag
    writeFormattedText( m_formats, chp, m_currentStyle ? &m_currentStyle->chp() : 0, m_index, text.length(), 1, 0L );

    //reset m_runOfText since we've already written it out
    m_runOfText = "";
    
    //m_index += text.length();
    
    //m_bodyWriter->startElement("text:p");
    //we may need to start new page with this text
    //if( m_bStartNewPage )
    //{
	//write style
	//m_contentWriter->startElement( "style:style" );
	//m_contentWriter->addAttribute( "style:name", "P1" );//TODO make this a dynamic name!
	//m_contentWriter->addAttribute( "style:family", "paragraph" );
	//m_contentWriter->startElement( "style:paragraph-properties" );
	//m_contentWriter->addAttribute( "fo:break-before", "page" );
	//m_contentWriter->endElement(); //style:paragraph-properties
	//m_contentWriter->endElement(); //style:style
	//add attribute to <text:p>
	//m_bodyWriter->addAttribute( "text:style-name", "P1" );
	//now set flag to false since we've started the new page
	//m_bStartNewPage = false;
	//kDebug(30513) << "b_StartNewPage = false";
    //}
    //m_bodyWriter->addTextNode( newText.string() );
    //m_bodyWriter->endElement(); //text:p
}

//called by runOfText to write the formatting information for that text?
//called by insertVariable() as well
//this function writes the formatting for a runOfText
//I'm going to make this actually write the text, too, inside a <text:span> tag (style:family=text)
void KWordTextHandler::writeFormattedText( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, int pos, int len, int formatId, QDomElement* pChildElement )
{
    //chp is the actual style for this run of text
    //refChp comes from m_currentStyle (0 if nothing there)
    //if statement logic: I guess the assumption is that m_currentStyle (refChp) has already
    // been applied, so only use chp if refChp doesn't exist or it's different

    kDebug(30513) ;

    //set up styleName
    QString styleName( "T" );
    styleName.append( QString::number( m_textStyleNumber ) );
    m_textStyleNumber++; //increment for the next style we need to write
    //start style tag for this run of text (closed at the end of the function)
    m_contentWriter->startElement( "style:style" );
    m_contentWriter->addAttribute( "style:name", styleName.toUtf8() );
    m_contentWriter->addAttribute( "style:family", "text" );
    m_contentWriter->startElement( "style:text-properties" );

    //write the text
    m_bodyWriter->startElement( "text:span" );
    m_bodyWriter->addAttribute( "text:style-name", styleName.toUtf8() );
    m_bodyWriter->addTextNode( m_runOfText.toUtf8() );
    m_bodyWriter->endElement(); //text:span

    //QDomElement format( mainDocument().createElement( "FORMAT" ) );
    //format.setAttribute( "id", formatId );
    //format.setAttribute( "pos", pos );
    //format.setAttribute( "len", len );

    //ico = color of text
    if ( !refChp || refChp->ico != chp->ico )
    {
        QString color = Conversion::color( chp->ico, -1 );
	m_contentWriter->addAttribute( "fo:color", color.toUtf8() );
        //QDomElement colorElem( mainDocument().createElement( "COLOR" ) );
        //colorElem.setAttribute( "red", color.red() );
        //colorElem.setAttribute( "blue", color.blue() );
        //colorElem.setAttribute( "green", color.green() );
        //format.appendChild( colorElem );
    }

    // Font name
    // TBD: We only use the Ascii font code. We should work out how/when to use the FE and Other font codes.
    // ftcAscii = (rgftc[0]) font for ASCII text
    if ( !refChp || refChp->ftcAscii != chp->ftcAscii )
    {
        QString fontName = getFont( chp->ftcAscii );

        if ( !fontName.isEmpty() )
        {
	    //TODO need to have a style:font-face-decl tag with the name of fontName!
	    m_contentWriter->addAttribute( "style:font-name", fontName.toUtf8() );
            //QDomElement fontElem( mainDocument().createElement( "FONT" ) );
            //fontElem.setAttribute( "name", fontName );
            //format.appendChild( fontElem );
        }
    }
    //hps = font size in half points
    if ( !refChp || refChp->hps != chp->hps )
    {
	QString size = QString::number( (int)(chp->hps/2) );
	m_contentWriter->addAttribute( "fo:font-size", size.append( "pt" ).toUtf8() );

        //kDebug(30513) <<"        font size:" << chp->hps/2;
        //QDomElement fontSize( mainDocument().createElement( "SIZE" ) );
        //fontSize.setAttribute( "value", (int)(chp->hps / 2) ); // hps is in half points
        //format.appendChild( fontSize );
    }
    //fBold = bold text if 1
    if ( !refChp || refChp->fBold != chp->fBold ) {
	m_contentWriter->addAttribute( "fo:font-weight", chp->fBold ? "bold" : "normal" );
        //QDomElement weight( mainDocument().createElement( "WEIGHT" ) );
        //weight.setAttribute( "value", chp->fBold ? 75 : 50 );
        //format.appendChild( weight );
    }
    //fItalic = italic text if 1
    if ( !refChp || refChp->fItalic != chp->fItalic ) {
	m_contentWriter->addAttribute( "fo:font-style", chp->fItalic ? "italic" : "normal" );
        //QDomElement italic( mainDocument().createElement( "ITALIC" ) );
        //italic.setAttribute( "value", chp->fItalic ? 1 : 0 );
        //format.appendChild( italic );
    }
    //kul: underline code
    if ( !refChp || refChp->kul != chp->kul ) {
        //QDomElement underline( mainDocument().createElement( "UNDERLINE" ) );
        //QString val = (chp->kul == 0) ? "0" : "1";
        switch ( chp->kul ) {
	case 0: //none
	    m_contentWriter->addAttribute( "style:text-underline-style", "none" );
	    break;
        case 1: // single
	    m_contentWriter->addAttribute( "style:text-underline-style", "solid" );
	    break;
        case 2: // by word
	    m_contentWriter->addAttribute( "style:text-underline-style", "solid" );
	    m_contentWriter->addAttribute( "style:text-underline-mode", "skip-white-space" );
	    break;
	case 3: // double
            //underline.setAttribute( "styleline", "solid" );
            //val = "double";
	    m_contentWriter->addAttribute( "style:text-underline-style", "solid" );
	    m_contentWriter->addAttribute( "style:text-underline-type", "double" );
            break;
        case 4: // dotted
	    m_contentWriter->addAttribute( "style:text-underline-style", "dotted" );
	    break;
        case 5: // hidden - This makes no sense as an underline property!
            //val = "0";
	    //I guess we could change this to have an underline the same color
	    //as the background?
	    m_contentWriter->addAttribute( "style:text-underline-type", "none" );
            break;
        case 6: // thick
            //underline.setAttribute( "styleline", "solid" );
            //val = "single-bold";
	    m_contentWriter->addAttribute( "style:text-underline-style", "solid" );
	    m_contentWriter->addAttribute( "style:text-underline-weight", "thick" );
            break;
        case 7: //dash
            //underline.setAttribute( "styleline", "dash" );
	    m_contentWriter->addAttribute( "style:text-underline-style", "dash" );
            break;
        case 8: //dot (not used, says the docu)
            //underline.setAttribute( "styleline", "dot" );
            break;
        case 9: //dot dash
            //underline.setAttribute( "styleline", "dashdot" );
	    m_contentWriter->addAttribute( "style:text-underline-style", "dot-dash" );
            break;
        case 10: //dot dot dash
            //underline.setAttribute( "styleline", "dashdotdot" );
	    m_contentWriter->addAttribute( "style:text-underline-style", "dot-dot-dash" );
            break;
        case 11: //wave
            //underline.setAttribute( "styleline", "wave" );
	    m_contentWriter->addAttribute( "style:text-underline-style", "wave" );
            break;
        default:
	    m_contentWriter->addAttribute( "style:text-underline-style", "none" );
        };
        //underline.setAttribute( "value", val );
        //format.appendChild( underline );
    }
    //fstrike = use strikethrough if 1
    //fDStrike = use double strikethrough if 1
    if ( !refChp || refChp->fStrike != chp->fStrike || refChp->fDStrike != chp->fDStrike ) {
	if ( chp->fStrike )
	    m_contentWriter->addAttribute( "style:text-line-through-type", "single" );
	else if ( chp->fDStrike )
	    m_contentWriter->addAttribute( "style:text-line-through-type", "double" );
	else
	    m_contentWriter->addAttribute( "style:text-line-through-type", "none" );
        /*QDomElement strikeout( mainDocument().createElement( "STRIKEOUT" ) );
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
        format.appendChild( strikeout );*/
    }

    // font attribute (uppercase, lowercase (not in MSWord), small caps)
    //fCaps = displayed with caps when 1, no caps when 0
    //fSmallCaps = displayed with small caps when 1, no small caps when 0
    if ( !refChp || refChp->fCaps != chp->fCaps || refChp->fSmallCaps != chp->fSmallCaps )
    {
        //QDomElement fontAttrib( mainDocument().createElement( "FONTATTRIBUTE" ) );
        //fontAttrib.setAttribute( "value", chp->fSmallCaps ? "smallcaps" : chp->fCaps ? "uppercase" : "none" );
        //format.appendChild( fontAttrib );
    }
    //iss = superscript/subscript indices
    if ( !refChp || refChp->iss != chp->iss ) { // superscript/subscript
	if ( chp->iss == 1 ) //superscript
	    m_contentWriter->addAttribute( "style:text-position", "super" );
	else if ( chp->iss == 2 ) //subscript
	    m_contentWriter->addAttribute( "style:text-position", "sub" );
        //QDomElement vertAlign( mainDocument().createElement( "VERTALIGN" ) );
        // Obviously the values are reversed between the two file formats :)
        //int kwordVAlign = (chp->iss==1 ? 2 : chp->iss==2 ? 1 : 0);
        //vertAlign.setAttribute( "value", kwordVAlign );
        //format.appendChild( vertAlign );
    }

    //fHighlight = when 1, characters are highlighted with color specified by chp.icoHighlight
    //icoHighlight = highlight color (see chp.ico)
    if ( !refChp || refChp->fHighlight != chp->fHighlight || refChp->icoHighlight != chp->icoHighlight ) {
        //QDomElement bgcolElem( mainDocument().createElement( "TEXTBACKGROUNDCOLOR" ) );
        if ( chp->fHighlight )
        {
	    QString color = Conversion::color( chp->icoHighlight, -1 );

            //bgcolElem.setAttribute( "red", color.red() );
            //bgcolElem.setAttribute( "blue", color.blue() );
            //bgcolElem.setAttribute( "green", color.green() );
        } else {
            //bgcolElem.setAttribute( "red", -1 );
            //bgcolElem.setAttribute( "blue", -1 );
            //bgcolElem.setAttribute( "green", -1 );
        }
        //format.appendChild( bgcolElem );
    }

    // Shadow text. Only on/off. The properties are defined at the paragraph level (in KWord).
    if ( !refChp || refChp->fShadow != chp->fShadow || refChp->fImprint != chp->fImprint ) {
        //QDomElement shadowElem( mainDocument().createElement( "SHADOW" ) );
        //QString css = "none";
        // Generate a shadow with hardcoded values that make it look like in MSWord.
        // We need to make the distance depend on the font size though, to look good
        if (chp->fShadow || chp->fImprint)
        {
            //int fontSize = (int)(chp->hps / 2);
            //int dist = fontSize > 20 ? 2 : 1;
            //if (chp->fImprint) // ## no real support for imprint, we use a topleft shadow
            //    dist = -dist;
            //css = QString::fromLatin1("#bebebe %1pt %1pt").arg(dist).arg(dist);
        }
        //shadowElem.setAttribute( "text-shadow", css );
        //format.appendChild( shadowElem );
    }

    //if ( pChildElement || !format.firstChild().isNull() ) // Don't save an empty format tag, unless the caller asked for it
        //parentElement.appendChild( format );
    //if ( pChildElement )
        //*pChildElement = format;
	

    //now close style tag for this run of text
    m_contentWriter->endElement(); //style:text-properties
    m_contentWriter->endElement(); //style:style
}

//#define FONT_DEBUG

// Return the name of a font. We have to convert the Microsoft font names to
// something that might just be present under X11.
QString KWordTextHandler::getFont(unsigned fc) const
{
    kDebug(30513) ;
    Q_ASSERT( m_parser );
    if ( !m_parser )
        return QString();
    const wvWare::Word97::FFN& ffn ( m_parser->font( fc ) );

    QConstString fontName( Conversion::string( ffn.xszFfn ) );
    QString font = fontName.string();

#ifdef FONT_DEBUG
    kDebug(30513) <<"    MS-FONT:" << font;
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
        if (font.find(fuzzyLookup[i][0], 0, false) != -1)
        {
            font = fuzzyLookup[i][1];
            break;
        }
    }

#ifdef FONT_DEBUG
    kDebug(30513) <<"    FUZZY-FONT:" << font;
#endif

    // Use Qt to look up our canonical equivalent of the font name.
    QFont xFont( font );
    QFontInfo info( xFont );

#ifdef FONT_DEBUG
    kDebug(30513) <<"    QT-FONT:" << info.family();
#endif

    return info.family();
}

//called by paragraphEnd()
void KWordTextHandler::writeOutParagraph( const QString& styleName, const QString& text )
{
    kDebug(30513) ;
    //if ( m_framesetElement.isNull() )
    //{
    //    if ( !text.isEmpty() ) // vertically merged table cells are ignored, and have empty text -> no warning on those
    //        kWarning(30513) << "KWordTextHandler: no frameset element to write to! text=" << text;
    //    return;
    //}
    /*QDomElement paragraphElementOut=mainDocument().createElement("PARAGRAPH");
    m_framesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=mainDocument().createElement("TEXT");
    textElement.setAttribute( "xml:space", "preserve" );
    paragraphElementOut.appendChild(textElement);
    paragraphElementOut.appendChild( m_formats );
    QDomElement layoutElement=mainDocument().createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement nameElement = mainDocument().createElement("NAME");
    nameElement.setAttribute("value", styleName);
    layoutElement.appendChild(nameElement);*/

    //if ( m_paragraphProperties )
    //{
        // Write out the properties of the paragraph - all the time now!
    writeLayout( /**m_paragraphProperties,*/ m_currentStyle );
    //}

    //textElement.appendChild(mainDocument().createTextNode(text)); - here's where we actually write the text

    m_paragraph = QString( "" );
    m_index = 0;
    //m_oldLayout = layoutElement; // Keep a reference to the old layout for some hacks
}

//only called by writeOutParagraph
//looks like this is where we actually write the formatting for the paragraph
void KWordTextHandler::writeLayout( /*QDomElement& parentElement, const wvWare::ParagraphProperties& paragraphProperties,*/ const wvWare::Style* style )
{
    kDebug(30513);

    //startthe <text:p> tag - it's closed in paragraphEnd()
    m_bodyWriter->startElement( "text:p" );

    //if we don't actually have paragraph properties, just return
    if ( !m_paragraphProperties )
    {
	kDebug(30513) << " we don't have any paragraph properties.";
	return;
    }

    //we do have properties, so setup the style tag
    //set up styleName
    QString styleName( "P" );
    styleName.append( QString::number( m_paragraphStyleNumber ) );
    m_paragraphStyleNumber++; //increment for the next style we need to write
    //start style tag for this run of text (closed at the end of the function)
    m_contentWriter->startElement( "style:style" );
    m_contentWriter->addAttribute( "style:name", styleName.toUtf8() );
    m_contentWriter->addAttribute( "style:family", "paragraph" );
    //does all the styling go in this tag? may need to collect the options & write both tags at the end
    m_contentWriter->startElement( "style:paragraph-properties" );

    //add the attribute for our style in <text:p>
    m_bodyWriter->addAttribute( "text:style-name", styleName.toUtf8() );
    

    const wvWare::Word97::PAP& pap = (*m_paragraphProperties).pap();
    // Always write out the alignment, it's required
    //QDomElement flowElement = mainDocument().createElement("FLOW");
    QString alignment = Conversion::alignment( pap.jc );
    //flowElement.setAttribute( "align", alignment );
    //parentElement.appendChild( flowElement );

    //kDebug(30513) <<" dxaLeft1=" << pap.dxaLeft1 <<" dxaLeft=" << pap.dxaLeft <<" dxaRight=" << pap.dxaRight <<" dyaBefore=" << pap.dyaBefore <<" dyaAfter=" << pap.dyaAfter <<" lspd=" << pap.lspd.dyaLine <<"/" << pap.lspd.fMultLinespace;

    //dxaLeft1 = first-line indent from left margin (signed, relative to dxaLeft)
    //dxaLeft = indent from left margin (signed)
    //dxaRight = index from right margin
    if ( pap.dxaLeft1 || pap.dxaLeft || pap.dxaRight )
    {
        //QDomElement indentsElement = mainDocument().createElement("INDENTS");
        // 'first' is relative to 'left' in both formats
        //indentsElement.setAttribute( "first", (double)pap.dxaLeft1 / 20.0 );
        //indentsElement.setAttribute( "left", (double)pap.dxaLeft / 20.0 );
        //indentsElement.setAttribute( "right", (double)pap.dxaRight / 20.0 );
        //parentElement.appendChild( indentsElement );
    }
    //dyaBefore = vertical spacing before paragraph (unsigned)
    //dyaAfter = vertical spacing after paragraph (unsigned)
    if ( pap.dyaBefore || pap.dyaAfter )
    {
        //QDomElement offsetsElement = mainDocument().createElement("OFFSETS");
        //offsetsElement.setAttribute( "before", (double)pap.dyaBefore / 20.0 );
        //offsetsElement.setAttribute( "after", (double)pap.dyaAfter / 20.0 );
        //parentElement.appendChild( offsetsElement );
    }

    // Linespacing
    //lspd = line spacing descriptor
    //Conversion::lineSpacing() returns "0" (default), "oneandhalf," or "double" 
    QString lineSpacing = Conversion::lineSpacing( pap.lspd );
    if ( lineSpacing != "0" )
    {
        //QDomElement lineSpacingElem = mainDocument().createElement( "LINESPACING" );
        //lineSpacingElem.setAttribute("value", lineSpacing );
        //parentElement.appendChild( lineSpacingElem );
    }
    //fKeep = keep entire paragraph on one page if possible
    //fKeepFollow = keep paragraph on same page with next paragraph if possible
    //fPageBreakBefore = start this paragraph on new page
    if ( pap.fKeep || pap.fKeepFollow || pap.fPageBreakBefore )
    {
	if ( pap.fPageBreakBefore )
	    m_contentWriter->addAttribute( "fo:break-before", "page" );
	/*QDomElement pageBreak = mainDocument().createElement( "PAGEBREAKING" );
        if ( pap.fKeep )
            pageBreak.setAttribute("linesTogether", "true");
        if ( pap.fPageBreakBefore )
            pageBreak.setAttribute("hardFrameBreak", "true" );
        if ( pap.fKeepFollow )
            pageBreak.setAttribute("keepWithNext", "true" );
        parentElement.appendChild( pageBreak );*/
    }

    // Borders
    //brcTop = specification for border above paragraph
    //brcBottom = specification for border below paragraph
    //brcLeft = specification for border to the left of paragraph
    //brcRight = specification for border to the right of paragraph
    //brcType: 0=none, 1=single, 2=thick, 3=double, 5=hairline, 6=dot, 7=dash large gap,
    //	8=dot dash, 9=dot dot dash, 10=triple, 11=thin-thick small gap, ...
    if ( pap.brcTop.brcType )
    {
        //QDomElement borderElement = mainDocument().createElement( "TOPBORDER" );
        //Conversion::setBorderAttributes( borderElement, pap.brcTop );
        //parentElement.appendChild( borderElement );
    }
    if ( pap.brcBottom.brcType )
    {
        //QDomElement borderElement = mainDocument().createElement( "BOTTOMBORDER" );
        //Conversion::setBorderAttributes( borderElement, pap.brcBottom );
        //parentElement.appendChild( borderElement );
    }
    if ( pap.brcLeft.brcType )
    {
        //QDomElement borderElement = mainDocument().createElement( "LEFTBORDER" );
        //Conversion::setBorderAttributes( borderElement, pap.brcLeft );
        //parentElement.appendChild( borderElement );
    }
    if ( pap.brcRight.brcType )
    {
        //QDomElement borderElement = mainDocument().createElement( "RIGHTBORDER" );
        //Conversion::setBorderAttributes( borderElement, pap.brcRight );
        //parentElement.appendChild( borderElement );
    }

    // Tabulators
    //itbdMac = number of tabs stops defined for paragraph. Must be >= 0 and <= 64.
    if ( pap.itbdMac )
    {
        for ( int i = 0 ; i < pap.itbdMac ; ++i )
        {
	    //rgdxaTab = array of { positions of itbdMac tab stops ; itbdMac tab descriptors } itbdMax == 64. The SPEC doesn't have such a structure, obviously. This is a wv2 change. (This data is never in the file as is, it comes from the SPRMs).
            const wvWare::Word97::TabDescriptor &td = pap.rgdxaTab[i];
            //QDomElement tabElement = mainDocument().createElement( "TABULATOR" );
            //tabElement.setAttribute( "ptpos", (double)td.dxaTab / 20.0 );
            //kDebug(30513) <<"ptpos=" << (double)td.dxaTab / 20.0;
            // Wow, lucky here. The type enum matches. Only, MSWord has 4=bar,
            // which kword doesn't support. We map it to 0 with a clever '%4' :)
            //tabElement.setAttribute( "type", td.tbd.jc % 4 );
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
            //tabElement.setAttribute( "filling", filling );
            //tabElement.setAttribute( "width", width );
            //parentElement.appendChild( tabElement );
        }
    }
    //ilfo = when non-zero, (1-based) index into the pllfo identifying the list to which the paragraph belongs
    if ( pap.ilfo > 0 )
    {
        writeCounter( /*parentElement,*/ *m_paragraphProperties, style );
    }
    //now close style:style tag for this paragraph
    m_contentWriter->endElement(); //style:paragraph-properties
    m_contentWriter->endElement(); //style:style
}

//only called by writeLayout()
//this is for lists (bullets & numbers)
void KWordTextHandler::writeCounter( /*QDomElement& parentElement,*/ const wvWare::ParagraphProperties& paragraphProperties, const wvWare::Style* style )
{
    kDebug(30513) ;
    const wvWare::ListInfo* listInfo = paragraphProperties.listInfo();
    if ( !listInfo )
        return;

#ifndef NDEBUG
    listInfo->dump();
#endif

    //QDomElement counterElement = mainDocument().createElement( "COUNTER" );
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
                //counterElement.setAttribute( "type", 10 ); // disc bullet
            } else if ( code == 0xD8 ) // Triangle
            {
                //counterElement.setAttribute( "type", 11 ); // Box. We have no triangle.
            } else {
                // Map all other bullets to a "custom bullet" in kword.
                kDebug(30513) <<"custom bullet, code=" << QString::number(code,16);
                //counterElement.setAttribute( "type", 6 ); // custom
                //counterElement.setAttribute( "bullet", code );
                QString paragFont = getFont( style->chp().ftcAscii );
                //counterElement.setAttribute( "bulletfont", paragFont );
            }
        } else
            kWarning(30513) << "Bullet with more than one character, not supported";
    }
    else
    {
        const wvWare::Word97::PAP& pap = paragraphProperties.pap();
        //counterElement.setAttribute( "start", listInfo->startAt() );

        int depth = pap.ilvl; /*both are 0 based*/
        // Heading styles don't set the ilvl, but must have a depth coming
        // from their heading level (the style's STI)
        bool isHeading = style->sti() >= 1 && style->sti() <= 9;
        if ( depth == 0 && isHeading )
        {
            depth = style->sti() - 1;
        }
        kDebug(30513) <<"  ilfo=" << pap.ilfo <<" ilvl=" << pap.ilvl <<" sti=" << style->sti() <<" depth=" << depth <<" numberingType=" << numberingType;
        //counterElement.setAttribute( "depth", depth );

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
            //kDebug(30513) << i <<":" << ch;
            if ( ch < 10 ) { // List level place holder
                if ( ch == pap.ilvl ) {
                    if ( depthFound )
                        kWarning(30513) << "ilvl " << pap.ilvl << " found twice in listInfo text...";
                    else
                        depthFound = true;
                    suffix.clear();
                } else {
                    Q_ASSERT( ch < pap.ilvl ); // Can't see how level 1 would have a <0> in it...
                    if ( ch < pap.ilvl )
                        ++displayLevels; // we found a 'parent level', to be displayed
                    prefix.clear(); // get rid of previous prefixes
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
                prefix.clear(); // it's already the parent's suffix -> remove it
                kDebug(30513) <<"depth=" << depth <<" parent suffix is" << prefix <<" -> clearing";
            }
        }
        if ( isHeading )
            numberingType = 1;
        if ( depthFound )
        {
            // Word6 models "1." as nfc=5
            if ( nfc == 5 && suffix.isEmpty() )
                suffix = ".";
            kDebug(30513) <<" prefix=" << prefix <<" suffix=" << suffix;
            //counterElement.setAttribute( "type", Conversion::numberFormatCode( nfc ) );
            //counterElement.setAttribute( "lefttext", prefix );
            //counterElement.setAttribute( "righttext", suffix );
            //counterElement.setAttribute( "display-levels", displayLevels );
            kDebug(30513) <<"storing suffix" << suffix <<" for depth" << depth;
            m_listSuffixes[ depth ] = suffix;
        }
        else
        {
            kWarning(30513) << "Not supported: counter text without the depth in it:" << Conversion::string(text).string();
        }
/*
        if ( listInfo->startAtOverridden() ||
             ( numberingType == 1 && m_previousOutlineLSID != 0 && m_previousOutlineLSID != listInfo->lsid() ) ||
             ( numberingType == 0 &&m_previousEnumLSID != 0 && m_previousEnumLSID != listInfo->lsid() ) )
            //counterElement.setAttribute( "restart", "true" );

        listInfo->alignment() is not supported in KWord
        listInfo->isLegal() hmm
        listInfo->notRestarted() [by higher level of lists] not supported
        listInfo->followingchar() ignored, it's always a space in KWord currently*/
    }
    if ( numberingType == 1 )
        m_previousOutlineLSID = listInfo->lsid();
    else
        m_previousEnumLSID = listInfo->lsid();
    //counterElement.setAttribute( "numberingtype", numberingType );
    //parentElement.appendChild( counterElement );
}

void KWordTextHandler::setFrameSetElement( const QDomElement& frameset )
{
    kDebug(30513) ;
    //m_framesetElement = frameset;
    for ( uint i = 0 ; i < 9 ; ++i )
        m_listSuffixes[i].clear();
}

/*QDomDocument KWordTextHandler::mainDocument() const
{
    return m_framesetElement.ownerDocument();
}*/

#include "texthandler.moc"
