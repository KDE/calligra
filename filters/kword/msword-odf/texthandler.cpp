/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

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
#include <KoGenStyle.h>


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


KWordTextHandler::KWordTextHandler( wvWare::SharedPtr<wvWare::Parser> parser, KoXmlWriter* bodyWriter, KoGenStyles* mainStyles )
    : m_parser( parser ), m_sectionNumber( 0 ), m_footNoteNumber( 0 ), m_endNoteNumber( 0 ),
      //m_textStyleNumber( 1 ), m_paragraphStyleNumber( 1 ), m_listStyleNumber( 1 ),
      m_currentListDepth( -1 ), m_currentListID( 0 ), m_currentStyle( 0L ),/* m_index( 0 ),*/
      m_currentTable( 0L ), m_writeTextToStylesDotXml( false ),
      m_bInParagraph( false ), m_bStartNewPage( false ),
      m_insideField( false ), m_fieldAfterSeparator( false ), m_fieldType( 0 )
{
    if(bodyWriter) {
        m_bodyWriter = bodyWriter; //set the pointer to bodyWriter for writing to content.xml in office:text
    }
    else {
	kWarning() << "No bodyWriter!";
    }
    if(mainStyles) {
	m_mainStyles = mainStyles; //for collecting most of the styles
    }
    else {
	kWarning() << "No mainStyles!";
    }
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
	    //TODO I don't know how to implement this yet
	    m_bStartNewPage = true; //tell us to start new page on next element we write
	    kDebug(30513) << "b_StartNewPage = true";
        }
    }
}

void KWordTextHandler::sectionEnd()
{
    kDebug(30513) ;
}

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
    //m_paragraph += '#';

    QDomElement formatElem;
    writeFormattedText( m_formats, chp, m_currentStyle ? &m_currentStyle->chp() : 0,/* m_index,*/ 1, 4 /*id*/, &formatElem );

    //m_index += 1;

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
    //m_paragraph += '#';

    // Can't call writeFormat, we have no chp.
    //QDomElement format( mainDocument().createElement( "FORMAT" ) );
    //format.setAttribute( "id", 6 );
    //format.setAttribute( "pos", m_index );
    //format.setAttribute( "len", 1 );
    //m_formats.appendChild( format );
    //QDomElement formatElem = format;

    //m_index += 1;

    //QDomElement anchorElem = m_formats.ownerDocument().createElement( "ANCHOR" );
    //anchorElem.setAttribute( "type", "frameset" );
    //anchorElem.setAttribute( "instance", fsname );
    //formatElem.appendChild( anchorElem );
    return QDomElement();
}

//sets m_paragraphProperties to the passed-in paragraphProperties
//sets m_currentStyle with PAP->istd (index to STSH structure)
void KWordTextHandler::paragraphStart( wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties )
{
    kDebug(30513) << "**********************************************";
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
	//TODO is there anything I need to actually do with styleName?
	QConstString styleName = Conversion::string( m_currentStyle->name() );
    }
    //write the paragraph formatting
    writeLayout( m_currentStyle );
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
    //close the <text:p> tag we opened in writeLayout()
    if ( !m_writeTextToStylesDotXml )
        m_bodyWriter->endElement();
    else
	m_tmpWriter->endElement();
    //clear our paragraph flag
    m_bInParagraph = false;
}//end paragraphEnd()

void KWordTextHandler::fieldStart( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/ )
{
    kDebug(30513) ;
    m_fieldType = Conversion::fldToFieldType( fld );
    m_insideField = true;
    m_fieldAfterSeparator = false;
    m_fieldValue = "";
}//end fieldStart()

void KWordTextHandler::fieldSeparator( const wvWare::FLD* /*fld*/, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/ )
{
    kDebug(30513) ;
    m_fieldAfterSeparator = true;
} //end fieldSeparator()

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
} //end fieldEnd()

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
    writeFormattedText( m_formats, chp, m_currentStyle ? &m_currentStyle->chp() : 0,/* m_index,*/ text.length(), 1, 0L );

    //reset m_runOfText since we've already written it out
    m_runOfText = "";
    
} //end runOfText()

//this function writes the formatting for a runOfText inside <text:span>
void KWordTextHandler::writeFormattedText( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, /*int pos,*/ int len, int formatId, QDomElement* pChildElement )
{
    //chp is the actual style for this run of text
    //refChp comes from m_currentStyle (0 if nothing there)
    //if statement logic: I guess the assumption is that m_currentStyle (refChp) has already
    // been applied, so only use chp if refChp doesn't exist or it's different

    kDebug(30513) ;

    //set up styleName
    //QString styleName( "T" );
    //styleName.append( QString::number( m_textStyleNumber ) );
    //m_textStyleNumber++; //increment for the next style we need to write
    //start style tag for this run of text (closed at the end of the function)
    //m_automaticStylesWriter->startElement( "style:style" );
    //m_automaticStylesWriter->addAttribute( "style:name", styleName.toUtf8() );
    //m_automaticStylesWriter->addAttribute( "style:family", "text" );
    //m_automaticStylesWriter->startElement( "style:text-properties" );
    
    //create the kogenstyle object with a text family
    KoGenStyle textStyle( KoGenStyle::StyleAuto, "text" );

    //ico = color of text
    if ( !refChp || refChp->ico != chp->ico )
    {
        QString color = Conversion::color( chp->ico, -1 );
	textStyle.addProperty(QString("fo:color"), color, KoGenStyle::TextType);
	//m_automaticStylesWriter->addAttribute( "fo:color", color.toUtf8() );
    }

    // Font name
    // TBD: We only use the Ascii font code. We should work out how/when to use the FE and Other font codes.
    // ftcAscii = (rgftc[0]) font for ASCII text
    if ( !refChp || refChp->ftcAscii != chp->ftcAscii )
    {
        QString fontName = getFont( chp->ftcAscii );

        if ( !fontName.isEmpty() )
        {
	    m_mainStyles->addFontFace( fontName );
	    textStyle.addProperty(QString("style:font-name"), fontName, KoGenStyle::TextType);
	    //m_automaticStylesWriter->addAttribute( "style:font-name", fontName.toUtf8() );
        }
    }

    //hps = font size in half points
    if ( !refChp || refChp->hps != chp->hps )
    {
	textStyle.addPropertyPt(QString("fo:font-size"), (int)(chp->hps/2), KoGenStyle::TextType);
	//m_automaticStylesWriter->addAttribute( "fo:font-size", size.append( "pt" ).toUtf8() );
    }

    //fBold = bold text if 1
    if ( !refChp || refChp->fBold != chp->fBold )
	textStyle.addProperty(QString("fo:font-weight"), chp->fBold ? QString("bold") : QString("normal"), KoGenStyle::TextType);
	//m_automaticStylesWriter->addAttribute( "fo:font-weight", chp->fBold ? "bold" : "normal" );

    //fItalic = italic text if 1
    if ( !refChp || refChp->fItalic != chp->fItalic )
	textStyle.addProperty(QString("fo:style"), chp->fItalic ? QString("italic") : QString("normal"), KoGenStyle::TextType);
	//m_automaticStylesWriter->addAttribute( "fo:font-style", chp->fItalic ? "italic" : "normal" );

    //kul: underline code
    if ( !refChp || refChp->kul != chp->kul )
    {
        switch ( chp->kul ) {
	case 0: //none
	    textStyle.addProperty(QString("style:text-underline-style"), QString("none") , KoGenStyle::TextType);
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "none" );
	    break;
        case 1: // single
	    textStyle.addProperty( "style:text-underline-style", "solid" , KoGenStyle::TextType);
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "solid" );
	    break;
        case 2: // by word
	    textStyle.addProperty( "style:text-underline-style", "solid" , KoGenStyle::TextType);
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "solid" );
	    textStyle.addProperty( "style:text-underline-mode", "skip-white-space" , KoGenStyle::TextType);
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-mode", "skip-white-space" );
	    break;
	case 3: // double
	    textStyle.addProperty( "style:text-underline-style", "solid" , KoGenStyle::TextType);
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "solid" );
	    textStyle.addProperty( "style:text-underline-type", "double", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-type", "double" );
            break;
        case 4: // dotted
	    textStyle.addProperty( "style:text-underline-style", "dotted", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "dotted" );
	    break;
        case 5: // hidden - This makes no sense as an underline property!
            //val = "0";
	    //I guess we could change this to have an underline the same color
	    //as the background?
	    textStyle.addProperty( "style:text-underline-type", "none", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-type", "none" );
            break;
        case 6: // thick
            //underline.setAttribute( "styleline", "solid" );
            //val = "single-bold";
	    textStyle.addProperty( "style:text-underline-style", "solid", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "solid" );
	    textStyle.addProperty( "style:text-underline-weight", "thick", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-weight", "thick" );
            break;
        case 7: //dash
            //underline.setAttribute( "styleline", "dash" );
	    textStyle.addProperty( "style:text-underline-style", "dash", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "dash" );
            break;
        case 8: //dot (not used, says the docu)
            //underline.setAttribute( "styleline", "dot" );
            break;
        case 9: //dot dash
            //underline.setAttribute( "styleline", "dashdot" );
	    textStyle.addProperty( "style:text-underline-style", "dot-dash", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "dot-dash" );
            break;
        case 10: //dot dot dash
            //underline.setAttribute( "styleline", "dashdotdot" );
	    textStyle.addProperty( "style:text-underline-style", "dot-dot-dash", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "dot-dot-dash" );
            break;
        case 11: //wave
            //underline.setAttribute( "styleline", "wave" );
	    textStyle.addProperty( "style:text-underline-style", "wave", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "wave" );
            break;
        default:
	    textStyle.addProperty( "style:text-underline-style", "none", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-underline-style", "none" );
        };
    }
    //fstrike = use strikethrough if 1
    //fDStrike = use double strikethrough if 1
    if ( !refChp || refChp->fStrike != chp->fStrike || refChp->fDStrike != chp->fDStrike )
    {
	if ( chp->fStrike )
	    textStyle.addProperty( "style:text-line-through-type", "single", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-line-through-type", "single" );
	else if ( chp->fDStrike )
	    textStyle.addProperty( "style:text-line-through-type", "double", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-line-through-type", "double" );
	else
	    textStyle.addProperty( "style:text-line-through-type", "none", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-line-through-type", "none" );
    }

    //font attribute (uppercase, lowercase (not in MSWord), small caps)
    //fCaps = displayed with all caps when 1, no caps when 0
    //fSmallCaps = displayed with small caps when 1, no small caps when 0
    if ( !refChp || refChp->fCaps != chp->fCaps || refChp->fSmallCaps != chp->fSmallCaps )
    {
	if ( chp->fCaps )
	    textStyle.addProperty( "fo:text-transform", "uppercase", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "fo:text-transform", "uppercase" );
	if ( chp->fSmallCaps )
	    textStyle.addProperty( "fo:font-variant", "small-caps", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "fo:font-variant", "small-caps" );
    }

    //iss = superscript/subscript indices
    if ( !refChp || refChp->iss != chp->iss )
    { 
	if ( chp->iss == 1 ) //superscript
	    textStyle.addProperty( "style:text-position", "super", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-position", "super" );
	else if ( chp->iss == 2 ) //subscript
	    textStyle.addProperty( "style:text-position", "sub", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:text-position", "sub" );
    }

    //fHighlight = when 1, characters are highlighted with color specified by chp.icoHighlight
    //icoHighlight = highlight color (see chp.ico)
    if ( !refChp || refChp->fHighlight != chp->fHighlight || refChp->icoHighlight != chp->icoHighlight ) {
        if ( chp->fHighlight )
        {
	    QString color = Conversion::color( chp->icoHighlight, -1 );
	    textStyle.addProperty( "fo:background-color", color, KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "fo:background-color", color );
        } else {
	    //TODO this should really be the surrounding background color
	    textStyle.addProperty( "fo:background-color", QString("#FFFFFF"), KoGenStyle::TextType);
	    //m_automaticStylesWriter->addAttribute( "fo:background-color", "#FFFFFF" );
        }
    }

    //fShadow = text has shadow if 1
    //fImprint = text engraved if 1
    if ( !refChp || refChp->fShadow != chp->fShadow || refChp->fImprint != chp->fImprint )
    {
        if ( chp->fShadow )
	    textStyle.addProperty( "style:text-shadow", "1pt", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "fo:text-shadow", "1pt" );
	if ( chp->fImprint )
	    textStyle.addProperty( "style:font-relief", "engraved", KoGenStyle::TextType );
	    //m_automaticStylesWriter->addAttribute( "style:font-relief", "engraved" );
    }

    //write the text (it may go to styles.xml for a header/footer)
    if ( m_writeTextToStylesDotXml )
    {
	//if we're writing the text to styles.xml, the style needs to be there as well
	textStyle.setAutoStyleInStylesDotXml( true );
	//put style into m_mainStyles & get its name
	QString styleName = m_mainStyles->lookup( textStyle );
	//now I just need to write the text:span to the header tag
	//that'll probably just be to the temporary writer, but I should construct that elsewhere
	m_tmpWriter->startElement( "text:span" );
	m_tmpWriter->addAttribute( "text:style-name", styleName.toUtf8() );
	m_tmpWriter->addTextNode( m_runOfText.toUtf8() );
	m_tmpWriter->endElement();
    }
    else
    {
	//put style into m_mainStyles & get its name
	QString styleName = m_mainStyles->lookup( textStyle );
	m_bodyWriter->startElement( "text:span" );
	m_bodyWriter->addAttribute( "text:style-name", styleName.toUtf8() );
        m_bodyWriter->addTextNode( m_runOfText.toUtf8() );
	m_bodyWriter->endElement(); //text:span
    }
} //end writeFormattedText()

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
}//end getFont()

//this is where we actually write the formatting for the paragraph
//Style* style is actually m_currentStyle 
void KWordTextHandler::writeLayout( const wvWare::Style* style )
{
    kDebug(30513);

    //we could be writing to the body as normal, or
    //we may need to write this to styles.xml
    KoXmlWriter* writer;
    if ( !m_writeTextToStylesDotXml )
	writer = m_bodyWriter;
    else
	writer = m_tmpWriter;

    //if we don't actually have paragraph properties, just return
    if ( !m_paragraphProperties )
    {
	//open text:p anyway, because we always close it in paragraphEnd()
	//we may be writing to styles.xml for a header/footer
	writer->startElement( "text:p" );
	kDebug(30513) << " we don't have any paragraph properties.";
	return;
    }

    //pap is our paragraph properties object
    const wvWare::Word97::PAP& pap = (*m_paragraphProperties).pap();
    
    //this needs to be before we open text:p, b/c text:list-item surrounds text:p
    //ilfo = when non-zero, (1-based) index into the pllfo identifying the list to which the paragraph belongs
    if ( pap.ilfo > 0 )
    {
	/*//we're in a list in the word document
	//listInfo is our list properties object
	const wvWare::ListInfo* listInfo = (*m_paragraphProperties).listInfo();
	bool writeListLevelStyle = false; //little flag to tell us whether or not to write that tag
        if ( !listInfo )
	{
	    kWarning() << "pap.ilfo is non-zero but there's no listInfo!";
	}
	//process the different places we could be in a list
	if ( m_currentListID == 0 )
	{
	    //we're starting a new list...
	    //set the list ID
	    m_currentListID = listInfo->lsid();
	    kDebug(30513) << "opening list " << m_currentListID;
	    //open <text:list> in the body
	    writer->startElement( "text:list" );
	    //set up the list style name
	    QString listStyleName( "L" );
	    listStyleName.append( QString::number( m_listStyleNumber ) );
	    m_listStyleNumber++;
	    //write styleName to the text:list tag
	    writer->addAttribute( "text:style-name", listStyleName ); 
	    //set up the text:list-style to be put in styles.xml
	    m_listStylesWriter->startElement( "text:list-style" );
	    m_listStylesWriter->addAttribute( "style:name", listStyleName );
	    //set flag to true because it's a new list, so we need to write that tag
	    writeListLevelStyle = true;
	}
	else if ( pap.ilvl > m_currentListDepth )
	{
	    //we're going to a new level in the list
	    kDebug(30513) << "going to a new level in list" << m_currentListID;
	    //open a new <text:list>
	    m_bodyWriter->startElement( "text:list" );
	    //it's a new level, so we need to configure this level
	    writeListLevelStyle = true;
	}
	else if ( pap.ilvl < m_currentListDepth )
	{
	    //we're backing out a level in the list
	    kDebug(30513) << "backing out a level in list" << m_currentListID;
	    //close the last <text:list-item of the level
	    m_bodyWriter->endElement();
	    //close <text:list> for the level
	    m_bodyWriter->endElement();
	    //close the <text:list-item> from the surrounding level
	    m_bodyWriter->endElement();
	}
	else
	{
	    kDebug(30513) << "just another item on the same level in the list";
	    //close <text:list-item> from the previous item
	    m_bodyWriter->endElement();
	}

	//write the style configuration tag if needed
	if ( writeListLevelStyle )
	{
	    kDebug(30513) << "writing the list level style";
	    //create this list style
	    KoGenStyle* listStyle( KoGenStyle::StyleAutoList );
	    int nfc = listInfo->numberFormat();
	    //text() returns a struct consisting of a UString text string (called text) & a pointer to a CHP (called chp)
	    wvWare::UString text = listInfo->text().text;
	    if ( nfc == 23 ) //bullets
	    {
		kDebug(30513) << "bullets...";
	        m_listStylesWriter->startElement( "text:list-level-style-bullet" );
		m_listStylesWriter->addAttribute( "text:level", pap.ilvl+1 );
		if ( text.length() == 1 )
		{
		    //with bullets, text can only be one character, which tells us what kind of bullet to use
		    unsigned int code = text[0].unicode();
		    if ( (code & 0xFF00) == 0xF000 ) // see wv2
			code &= 0x00FF;
		    m_listStylesWriter->addAttribute( "text:bullet-char", QString::QString( code ).toUtf8() );
		}
		else
		    kWarning(30513) << "Bullet with more than one character, not supported";

		m_listStylesWriter->startElement( "style:list-level-properties" );
		//TODO this is just hardcoded for now
		m_listStylesWriter->addAttribute( "text:min-label-width", "0.25in" );
		m_listStylesWriter->endElement(); //style:list-level-properties
		//close element
		m_listStylesWriter->endElement(); //text:list-level-style-bullet
	    }
	    else //numbered/outline list
	    {
		kDebug(30513) << "numbered/outline... nfc = " << nfc;
		m_listStylesWriter->startElement( "text:list-level-style-number" );
		m_listStylesWriter->addAttribute( "text:level", pap.ilvl+1 );
		//*************************************
		int depth = pap.ilvl; //both are 0 based
		int numberingType = listInfo->isWord6() && listInfo->prev() ? 1 : 0;
		// Heading styles don't set the ilvl, but must have a depth coming
		// from their heading level (the style's STI)
		bool isHeading = style->sti() >= 1 && style->sti() <= 9;
		if ( depth == 0 && isHeading )
		{
		    depth = style->sti() - 1;
		}
		// Now we need to parse the text, to try and convert msword's powerful list template stuff
		QString prefix, suffix;
		bool depthFound = false;
		int displayLevels = 1;
		// We parse <0>.<2>.<1>. as "level 2 with suffix='.'" (no prefix)
	        // But "Section <0>)" has both prefix and suffix.
		// The common case is <0>.<1>.<2> (display-levels=3)
		//loop through all of text
		//this just sets depthFound & displayLevels & the suffix & prefix
		for ( int i = 0 ; i < text.length() ; ++i )
		{
		    short ch = text[i].unicode();
		    //kDebug(30513) << i <<":" << ch;
		    if ( ch < 10 )
		    { // List level place holder
		        if ( ch == pap.ilvl )
			{
			    if ( depthFound )
			        kWarning(30513) << "ilvl " << pap.ilvl << " found twice in listInfo text...";
			    else
			        depthFound = true;
			    suffix.clear();
			}
			else
			{
			    Q_ASSERT( ch < pap.ilvl ); // Can't see how level 1 would have a <0> in it...
			    if ( ch < pap.ilvl )
			        ++displayLevels; // we found a 'parent level', to be displayed
			    prefix.clear(); // get rid of previous prefixes
			}
		    }
		    //if it's not a number < 10
		    else
		    {
			//add it to suffix if we've found the level that we're at
			if ( depthFound )
			    suffix += QChar(ch);
			//or add it to prefix if we haven't
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
		    //do I still need to keep the m_listSuffixes stuff?
		    if ( depth > 0 && !prefix.isEmpty() && m_listSuffixes[ depth - 1 ] == prefix )
		    {
		        prefix.clear(); // it's already the parent's suffix -> remove it
		        kDebug(30513) <<"depth=" << depth <<" parent suffix is" << prefix <<" -> clearing";
		    }
		}
		if ( isHeading )
		    numberingType = 1;
		//this is where we actually write the information
		if ( depthFound )
		{
		    // Word6 models "1." as nfc=5
		    if ( nfc == 5 && suffix.isEmpty() )
		        suffix = ".";
		    kDebug(30513) <<" prefix=" << prefix <<" suffix=" << suffix;
		    //counterElement.setAttribute( "type", Conversion::numberFormatCode( nfc ) );
		    m_listStylesWriter->addAttribute( "style:num-format", Conversion::numberFormatCode( nfc ) );
		    //counterElement.setAttribute( "lefttext", prefix );
		    m_listStylesWriter->addAttribute( "style:num-prefix", prefix );
		    //counterElement.setAttribute( "righttext", suffix );
		    m_listStylesWriter->addAttribute( "style:num-suffix", suffix );
		    //counterElement.setAttribute( "display-levels", displayLevels );
		    kDebug(30513) <<"storing suffix" << suffix <<" for depth" << depth;
		    m_listSuffixes[ depth ] = suffix;
		}
		else
		{
		    kWarning(30513) << "Not supported: counter text without the depth in it:" << Conversion::string(text).string();
		}

		if ( listInfo->startAtOverridden() ) //||
		     //( numberingType == 1 && m_previousOutlineLSID != 0 && m_previousOutlineLSID != listInfo->lsid() ) ||
		     //( numberingType == 0 &&m_previousEnumLSID != 0 && m_previousEnumLSID != listInfo->lsid() ) )
		{
		    //counterElement.setAttribute( "restart", "true" );
		}

		//listInfo->alignment() is not supported in KWord
		//listInfo->isLegal() hmm
		//listInfo->notRestarted() [by higher level of lists] not supported
		//listInfo->followingchar() ignored, it's always a space in KWord currently
	    //*************************************
		m_listStylesWriter->startElement( "style:list-level-properties" );
		//TODO this is just hardcoded for now
		m_listStylesWriter->addAttribute( "text:min-label-width", "0.25in" );
		m_listStylesWriter->endElement(); //style:list-level-properties
		//close element
		m_listStylesWriter->endElement(); //text:list-level-style-number
	    } //end numbered list stuff
	}//end write list level stuff
	//now update m_currentListDepth
	m_currentListDepth = pap.ilvl;
	//update the list depth in Document
	kDebug(30513) << "emiting updateListDepth signal with depth " << m_currentListDepth;
	emit updateListDepth( m_currentListDepth );
	//we always want to open this tag
	m_bodyWriter->startElement( "text:list-item" );
	*/
    } //end pap.ilfo > 0 (ie. we're in a list)
    else
    {}
	/*
	//not in a list at all in the word document, so check if we need to close one in the odt
	if ( m_currentListID != 0 )
	{
	    kDebug(30513) << "closing list " << m_currentListID;
	    //if pap.ilfo is 0, we should be done with the list, so close <text:list> & <text:list-style>
	    //TODO should probably test this more, to make sure it does work this way
	    //for level 0, we need to close the last item and the list
	    //for level 1, we need to close the last item and the list, and the last item and the list
	    //for level 2, we need to close the last item and the list, and the last item adn the list, and again
	    for (int i = 0; i <= m_currentListDepth; i++)
	    {
		m_bodyWriter->endElement(); //close the last text:list-item
		m_bodyWriter->endElement(); //text:list
	    }
	    m_listStylesWriter->endElement(); //text:list-style
	    m_currentListID = 0;
	    m_currentListDepth = -1;
	    kDebug(30513) << "emiting updateListDepth signal with depth " << m_currentListDepth;
	    emit updateListDepth( m_currentListDepth );
	}
	*/
    //start the <text:p> tag - it's closed in paragraphEnd()
    writer->startElement( "text:p" );

    KoGenStyle paragraphStyle( KoGenStyle::StyleAuto, "paragraph" );

    //paragraph alignment
    //jc = justification code
    if ( pap.jc == 1 ) //1 = center justify
	paragraphStyle.addProperty( "fo:text-align", "center" );
    else if (pap.jc == 2 ) //2 = right justify
	paragraphStyle.addProperty( "fo:text-align", "end" );
    else if (pap.jc == 3 ) //3 = left & right justify
	paragraphStyle.addProperty( "fo:text-align", "justify" );
    else //0 = left justify
	paragraphStyle.addProperty( "fo:text-align", "start" );

    //kDebug(30513) <<" dxaLeft1=" << pap.dxaLeft1 <<" dxaLeft=" << pap.dxaLeft <<" dxaRight=" << pap.dxaRight <<" dyaBefore=" << pap.dyaBefore <<" dyaAfter=" << pap.dyaAfter <<" lspd=" << pap.lspd.dyaLine <<"/" << pap.lspd.fMultLinespace;

    //dxaLeft1 = first-line indent from left margin (signed, relative to dxaLeft)
    //dxaLeft = indent from left margin (signed)
    //dxaRight = indent from right margin (signed)
    if ( pap.dxaLeft1 || pap.dxaLeft || pap.dxaRight )
    {
	paragraphStyle.addProperty( "fo:margin-left", (int)pap.dxaLeft );
	paragraphStyle.addProperty( "fo:margin-right", (int)pap.dxaRight );
	paragraphStyle.addProperty( "fo:text-indent", (int)pap.dxaLeft1 );
    }

    //dyaBefore = vertical spacing before paragraph (unsigned)
    //dyaAfter = vertical spacing after paragraph (unsigned)
    if ( pap.dyaBefore || pap.dyaAfter )
    {
	paragraphStyle.addProperty( "fo:margin-top", (int)pap.dyaBefore );
	paragraphStyle.addProperty( "fo:margin-bottom", (int)pap.dyaAfter );
    }

    // Linespacing
    //lspd = line spacing descriptor
    //Conversion::lineSpacing() returns "0" (default), "oneandhalf," or "double" 
    //QString lineSpacingAttribute = Conversion::lineSpacing( pap.lspd );
    if ( pap.lspd.fMultLinespace == 1 ) //Word will reserve for each line the 
				    //(maximal height of the line*lspd.dyaLine)/240
    {
	//get the proportion & turn it into a percentage for the attribute
	QString proportionalLineSpacing( QString::number( ((double)pap.lspd.dyaLine/240.0)*100.0 ) );
	paragraphStyle.addProperty( "fo:line-height", proportionalLineSpacing.append( "%" ) );
    }
    else if ( pap.lspd.fMultLinespace == 0 )//magnitude of lspd.dyaLine specifies the amount of space 
				    //that will be provided for lines in the paragraph in twips
    {
	// see sprmPDyaLine in generator_wword8.htm
	double value = QABS((double)pap.lspd.dyaLine / 20.0); // twip -> pt
	// lspd.dyaLine > 0 means "at least", < 0 means "exactly"
	if ( pap.lspd.dyaLine > 0 )
	    paragraphStyle.addPropertyPt( "fo:line-height-at-least", value );
	else if (pap.lspd.dyaLine < 0 )
	    paragraphStyle.addPropertyPt( "fo:line-height", value );
    }
    else
	    kWarning(30513) << "Unhandled LSPD::fMultLinespace value: " << pap.lspd.fMultLinespace;
    //end linespacing

    //fKeep = keep entire paragraph on one page if possible
    //fKeepFollow = keep paragraph on same page with next paragraph if possible
    //fPageBreakBefore = start this paragraph on new page
    if ( pap.fKeep || pap.fKeepFollow || pap.fPageBreakBefore )
    {
	if ( pap.fKeep )
	    paragraphStyle.addProperty( "fo:keep-together", "always" );
	if ( pap.fKeepFollow )
	    paragraphStyle.addProperty( "fo:keep-with-next", "always" );
	if ( pap.fPageBreakBefore )
	    paragraphStyle.addProperty( "fo:break-before", "page" );
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
	paragraphStyle.addProperty( "fo:border-top", Conversion::setBorderAttributes( pap.brcTop ) );
    }
    if ( pap.brcBottom.brcType )
    {
	paragraphStyle.addProperty( "fo:border-bottom", Conversion::setBorderAttributes( pap.brcBottom ) );
    }
    if ( pap.brcLeft.brcType )
    {
	paragraphStyle.addProperty( "fo:border-left", Conversion::setBorderAttributes( pap.brcLeft ) );
    }
    if ( pap.brcRight.brcType )
    {
	paragraphStyle.addProperty( "fo:border-right", Conversion::setBorderAttributes( pap.brcRight ) );
    }

    //close style:paragraph-properties, b/c we're done with that and we need to write more tags
    //m_automaticStylesWriter->endElement(); //style:paragraph-properties

    // Tabulators
    //itbdMac = number of tabs stops defined for paragraph. Must be >= 0 and <= 64.
    /*if ( pap.itbdMac )
    {
	m_automaticStylesWriter->startElement( "style:tab-stops" );
        for ( int i = 0 ; i < pap.itbdMac ; ++i )
        {
	    m_automaticStylesWriter->startElement( "style:tab-stop" );

	    //rgdxaTab = array of { positions of itbdMac tab stops ; itbdMac tab descriptors } itbdMax == 64.
            const wvWare::Word97::TabDescriptor &td = pap.rgdxaTab[i];
	    //td.dxaTab = position in twips
	    QString pos( QString::number( (double)td.dxaTab / 20.0 ) );
	    m_automaticStylesWriter->addAttribute( "style:position", pos.append( "pt" ) );

	    //td.tbd.jc = justification code
	    if ( td.tbd.jc ) //0 = left-aligned = default, so that can just be ignored
	    {
		if ( td.tbd.jc == 1 ) //centered
		    m_automaticStylesWriter->addAttribute( "style:type", "center" );
		else if ( td.tbd.jc == 2 ) //right-aligned
		    m_automaticStylesWriter->addAttribute( "style:type", "right" );
		else //3 = decimal tab -> align on decimal point
		    //4 = bar -> just creates a vertical bar at that point that's always visible
		    kWarning(30513) << "Unhandled tab justification code: " << td.tbd.jc;
	    }
	    //td.tbd.tlc = tab leader code
	    if ( td.tbd.tlc )//0 = no leader, which is default & can just be ignored
	    {
		if ( td.tbd.tlc == 1 ) //1 dotted leader 
		    m_automaticStylesWriter->addAttribute( "style:leader-text", "." );
		else if (td.tbd.tlc == 2 ) //2 hyphenated leader 
		    m_automaticStylesWriter->addAttribute( "style:leader-text", "-" );
		//TODO 3 single line leader 
		//TODO 4 heavy line leader
	    }

	    m_automaticStylesWriter->endElement(); //style:tab-stop
        }
	m_automaticStylesWriter->endElement(); //style:tab-stops
    }*/
    
    //add style to m_mainStyles & get style name
    QString styleName = "";
    if(!paragraphStyle.isEmpty()) {
	styleName = m_mainStyles->lookup(paragraphStyle);
    }
    else {
	kWarning() << "paragraphStyle is empty!";
    }
    //add the attribute for our style in <text:p>
    writer->addAttribute( "text:style-name", styleName.toUtf8() );
}

#include "texthandler.moc"
