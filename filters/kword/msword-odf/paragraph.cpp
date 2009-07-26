/* This file is part of the KOffice project
   Copyright (C) 2009 Benjamin Cail <cricketc@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

*/

#include "paragraph.h"
#include "conversion.h"

#include <kdebug.h>

Paragraph::Paragraph( KoGenStyles* mainStyles, bool inStylesDotXml, bool isHeading, int outlineLevel )
    : m_paragraphProperties(0),
    m_paragraphProperties2(0),
    m_paragraphStyle(0),
    m_paragraphStyle2(0),
    m_mainStyles(0),
    m_parentStyle(0),
    m_parentStyle2(0),
    m_inStylesDotXml(false),
    m_isHeading(false),
    m_outlineLevel(0)
{
    kDebug(30513);
    m_mainStyles = mainStyles;
    m_paragraphStyle = new KoGenStyle(KoGenStyle::StyleAuto, "paragraph");
    if ( inStylesDotXml )
    {
        kDebug(30513) << "this paragraph is in styles.xml";
        m_inStylesDotXml = true;
        //if we're writing to styles.xml, the style should go there, too
        m_paragraphStyle->setAutoStyleInStylesDotXml(true);
    }
    else
    {
        m_inStylesDotXml = false;
    }
    if ( isHeading )
    {
        kDebug(30513) << "this paragraph is a heading";
        m_isHeading = true;
        m_outlineLevel = ( outlineLevel > 0 ? outlineLevel : 1 );
    }
    else
    {
        m_isHeading = false;
        m_outlineLevel = -1;
    }
}

Paragraph::~Paragraph()
{
    delete m_paragraphStyle;
    m_paragraphStyle = 0;
}

void Paragraph::addRunOfText(QString text,  wvWare::SharedPtr<const wvWare::Word97::CHP> chp, QString fontName, const wvWare::StyleSheet& styles)
{
    //check for column break in this text string
    int colBreak = text.indexOf(QChar(0xE));
    //I think this break always comes at the beginning of the text string
    if ( colBreak == 0 )
    {
        kDebug(30513) << "colBreak = " << colBreak;
        //add needed attribute to paragraph style
        //Note: this logic breaks down if this isn't the first string in the paragraph, or there
        // are other strings with another colBreak later in the same paragraph
        m_paragraphStyle->addProperty( "fo:break-before", "column", KoGenStyle::ParagraphType );
        //remove character that signaled a column break
        text.remove(QChar(0xE));
    }

    //add text string to list
    m_textStrings.push_back(QString(text));

    // Now find out what style to assoiciate with the string


    if (chp == 0 ) {
        // if inner paragraph - just add a null style & return from function
        KoGenStyle* style = 0;
        m_textStyles.push_back( style );
        return;
    }

    const wvWare::Style* msTextStyle = styles.styleByIndex( chp->istd );
    Q_ASSERT( msTextStyle );
    QString msTextStyleName = Conversion::string( msTextStyle->name() );
    kDebug(30513) << "text has characterstyle " << msTextStyleName;

    //need to replace all non-alphanumeric characters with hex representation
    for(int i = 0; i < msTextStyleName.size(); i++)
    {
        if(!msTextStyleName[i].isLetterOrNumber())
        {
            msTextStyleName.remove(i, 1);
            i--;
        }
    }

    KoGenStyle *textStyle = m_mainStyles->styleForModification(msTextStyleName);

    // modify the character style if we detect any diif between the chp of the paragraph and the summed chp
    const wvWare::Style* parentStyle = styles.styleByIndex( msTextStyle->m_std->istdBase );
    if (parentStyle) {
        parseCharacterProperties( chp, textStyle, m_parentStyle );
        //if we have a new font, process that
        const wvWare::Word97::CHP* refChp = &m_parentStyle->chp();
        if ( !refChp || refChp->ftcAscii != chp->ftcAscii )
        {
            if ( !fontName.isEmpty() )
            {
                textStyle->addProperty(QString("style:font-name"), fontName, KoGenStyle::TextType);
            }
        }
    } else {
        textStyle = new KoGenStyle(KoGenStyle::StyleTextAuto, "text");
        if (m_inStylesDotXml) {
            textStyle->setAutoStyleInStylesDotXml(true);
        }
        parseCharacterProperties( chp, textStyle, m_parentStyle );
    }

    //add text style to list
    m_textStyles.push_back( textStyle );
}

void Paragraph::writeToFile( KoXmlWriter* writer )
{
    kDebug(30513);

    //set up paragraph style
    parseParagraphProperties(*m_paragraphProperties, m_paragraphStyle);

    //open paragraph or heading tag
    if ( m_isHeading )
    {
        writer->startElement( "text:h", false );
        writer->addAttribute( "text:outline-level", m_outlineLevel );
    }
    else
    {
        writer->startElement("text:p", false);
    }

    //add paragraph style to the collection and its name to the content
    kDebug(30513) << "adding paragraphStyle";
    //add the attribute for our style in <text:p>
    QString styleName("P");
    styleName = m_mainStyles->lookup(*m_paragraphStyle, styleName);
    writer->addAttribute( "text:style-name", styleName.toUtf8() );

    //just close the paragraph if there's no content
    if ( m_textStrings.empty() )
    {
        writer->endElement(); //text:p
        return;
    }

    //loop through each text strings and styles (equal # of both)
    kDebug(30513) << "writing text spans now";
    QString oldStyleName;
    bool startedSpan = false;
    for ( int i = 0; i < m_textStrings.size(); i++ ) {
        if ( m_textStyles[i] == 0 ) {
            //if style is null, we have an inner paragraph and add the
            // complete paragraph element to writer
            //need to get const char* from the QString
            kDebug(30513) << "complete element: " <<
                m_textStrings[i].toLocal8Bit().constData();
            writer->addCompleteElement( m_textStrings[i].toLocal8Bit().constData() );
        } else {
            //add text style to collection
            //put style into m_mainStyles & get its name
            //kDebug(30513) << m_textStyles[i]->type();
            styleName = "T";
            styleName = m_mainStyles->lookup(*m_textStyles[i], styleName);

            if(oldStyleName != styleName) {
                if (startedSpan) {
                    writer->endElement(); //text:span
                    startedSpan = false;
                }
                if(styleName != "DefaultParagraphFont") {
                    writer->startElement( "text:span" );
                    writer->addAttribute( "text:style-name", styleName.toUtf8() );
                    startedSpan = true;
                }
                oldStyleName = styleName;
            }
            //write text string to writer
            //now I just need to write the text:span to the header tag
            kDebug(30513) << "Writing \"" << m_textStrings[i] << "\"";
            writer->addTextSpan(m_textStrings[i]);
            //cleanup
            //delete m_textStyles[i];
            m_textStyles[i] = 0;
        }
    }
    if (startedSpan) // we wrote a span of text so make sure we close the span tag
        writer->endElement(); //text:span

    //close the <text:p> or <text:h> tag we opened
    writer->endElement();
}

void Paragraph::setParentStyle( const wvWare::Style* parentStyle, QString parentStyleName )
{
    kDebug(30513);
    m_parentStyle = parentStyle;
    m_paragraphStyle->addAttribute( "style:parent-style-name", parentStyleName );
}

KoGenStyle* Paragraph::getParagraphStyle()
{
    return m_paragraphStyle;
}

//open/closeInnerParagraph functions:
//lets us process a paragraph inside another paragraph,
//as in the case of footnotes. openInnerParagraph should
//be called from texthandler.paragraphStart, and
//closeInnerParagraph should be called from paragraphEnd,
//but only after calling writeToFile to write the content
//to another xml writer (eg. m_footnoteWriter).
void Paragraph::openInnerParagraph()
{
    kDebug(30513);

    //copy parent and paragraph styles
    m_paragraphStyle2 = m_paragraphStyle;
    m_paragraphStyle = new KoGenStyle(KoGenStyle::StyleAuto, "paragraph");
    m_parentStyle2 = m_parentStyle;
    m_paragraphProperties2 = m_paragraphProperties;
    m_paragraphProperties = 0;

    //move m_textStyles and m_textStrings content to
    //m_textStyles2 and m_textStrings2
    m_textStyles2 = m_textStyles;
    m_textStrings2 = m_textStrings;
    m_textStyles.clear();
    m_textStrings.clear();
}

void Paragraph::closeInnerParagraph()
{
    kDebug(30513);

    //clear temp variables and restore originals
    delete m_paragraphStyle;
    m_paragraphStyle = m_paragraphStyle2;
    m_paragraphStyle2 = 0;
    m_parentStyle = m_parentStyle2;
    m_parentStyle2 = 0;
    m_paragraphProperties = m_paragraphProperties2;
    m_paragraphProperties2 = 0;
    m_textStyles.clear();
    m_textStrings.clear();
    m_textStyles = m_textStyles2;
    m_textStrings = m_textStrings2;
    m_textStyles2.clear();
    m_textStrings2.clear();
}

void Paragraph::setParagraphProperties( wvWare::SharedPtr<const wvWare::ParagraphProperties> properties )
{
    m_paragraphProperties = properties;
}

void Paragraph::parseParagraphProperties( const wvWare::ParagraphProperties& properties,
                KoGenStyle* style)
{
    kDebug(30513);

    //pap is our paragraph properties object
    const wvWare::Word97::PAP& pap = properties.pap();

    //paragraph alignment
    //jc = justification code
    if ( pap.jc == 1 ) //1 = center justify
        style->addProperty( "fo:text-align", "center", KoGenStyle::ParagraphType );
    else if (pap.jc == 2 ) //2 = right justify
        style->addProperty( "fo:text-align", "end", KoGenStyle::ParagraphType );
    else if (pap.jc == 3 ) //3 = left & right justify
        style->addProperty( "fo:text-align", "justify", KoGenStyle::ParagraphType );
    else //0 = left justify
        style->addProperty( "fo:text-align", "start", KoGenStyle::ParagraphType );

    //kDebug(30513) <<" dxaLeft1=" << pap.dxaLeft1 <<" dxaLeft=" << pap.dxaLeft <<" dxaRight=" << pap.dxaRight <<" dyaBefore=" << pap.dyaBefore <<" dyaAfter=" << pap.dyaAfter <<" lspd=" << pap.lspd.dyaLine <<"/" << pap.lspd.fMultLinespace;

    //dxaLeft1 = first-line indent from left margin (signed, relative to dxaLeft)
    //dxaLeft = indent from left margin (signed)
    //dxaRight = indent from right margin (signed)
    if ( pap.dxaLeft1 || pap.dxaLeft || pap.dxaRight )
    {
        style->addProperty( "fo:margin-left", (int)pap.dxaLeft, KoGenStyle::ParagraphType );
        style->addProperty( "fo:margin-right", (int)pap.dxaRight, KoGenStyle::ParagraphType );
        style->addProperty( "fo:text-indent", (int)pap.dxaLeft1, KoGenStyle::ParagraphType );
    }

    //dyaBefore = vertical spacing before paragraph (unsigned)
    //dyaAfter = vertical spacing after paragraph (unsigned)
    if ( pap.dyaBefore || pap.dyaAfter )
    {
        style->addProperty( "fo:margin-top", (int)pap.dyaBefore, KoGenStyle::ParagraphType );
        style->addProperty( "fo:margin-bottom", (int)pap.dyaAfter, KoGenStyle::ParagraphType );
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
        style->addProperty( "fo:line-height", proportionalLineSpacing.append( "%" ), KoGenStyle::ParagraphType );
    }
    else if ( pap.lspd.fMultLinespace == 0 )//magnitude of lspd.dyaLine specifies the amount of space
                                    //that will be provided for lines in the paragraph in twips
    {
        // see sprmPDyaLine in generator_wword8.htm
        double value = qAbs((double)pap.lspd.dyaLine / 20.0); // twip -> pt
        // lspd.dyaLine > 0 means "at least", < 0 means "exactly"
        if ( pap.lspd.dyaLine > 0 )
            style->addPropertyPt( "fo:line-height-at-least", value, KoGenStyle::ParagraphType );
        else if (pap.lspd.dyaLine < 0 )
            style->addPropertyPt( "fo:line-height", value, KoGenStyle::ParagraphType);
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
            style->addProperty( "fo:keep-together", "always", KoGenStyle::ParagraphType );
        if ( pap.fKeepFollow )
            style->addProperty( "fo:keep-with-next", "always", KoGenStyle::ParagraphType );
        if ( pap.fPageBreakBefore )
            style->addProperty( "fo:break-before", "page", KoGenStyle::ParagraphType );
    }

    // Borders
    //brcTop = specification for border above paragraph
    //brcBottom = specification for border below paragraph
    //brcLeft = specification for border to the left of paragraph
    //brcRight = specification for border to the right of paragraph
    //brcType: 0=none, 1=single, 2=thick, 3=double, 5=hairline, 6=dot, 7=dash large gap,
    //  8=dot dash, 9=dot dot dash, 10=triple, 11=thin-thick small gap, ...
    if ( pap.brcTop.brcType )
    {
        style->addProperty( "fo:border-top", Conversion::setBorderAttributes( pap.brcTop ), KoGenStyle::ParagraphType );
    }
    if ( pap.brcBottom.brcType )
    {
        style->addProperty( "fo:border-bottom", Conversion::setBorderAttributes( pap.brcBottom ), KoGenStyle::ParagraphType );
    }
    if ( pap.brcLeft.brcType )
    {
        style->addProperty( "fo:border-left", Conversion::setBorderAttributes( pap.brcLeft ), KoGenStyle::ParagraphType );
    }
    if ( pap.brcRight.brcType )
    {
        style->addProperty( "fo:border-right", Conversion::setBorderAttributes( pap.brcRight ), KoGenStyle::ParagraphType );
    }

    // Tabulators
    //itbdMac = number of tabs stops defined for paragraph. Must be >= 0 and <= 64.
    if ( pap.itbdMac )
    {
        kDebug(30513) << "processing tab stops";
        //looks like we need to write these out with an xmlwriter
        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        KoXmlWriter tmpWriter(&buf, 3);//root, office:automatic-styles, style:style
        tmpWriter.startElement("style:tab-stops");
        for ( int i = 0 ; i < pap.itbdMac ; ++i )
        {
            tmpWriter.startElement("style:tab-stop");

            //rgdxaTab = array of { positions of itbdMac tab stops ; itbdMac tab descriptors } itbdMax == 64.
            const wvWare::Word97::TabDescriptor &td = pap.rgdxaTab[i];
            //td.dxaTab = position in twips
            //QString pos( QString::number( (double)td.dxaTab / 20.0 ) );
            tmpWriter.addAttributePt("style:position", (double)td.dxaTab / 20.0);

            //td.tbd.jc = justification code
            if ( td.tbd.jc ) //0 = left-aligned = default, so that can just be ignored
            {
                if ( td.tbd.jc == 1 ) { //centered
                    tmpWriter.addAttribute("style:type", "center");
                }
                else if ( td.tbd.jc == 2 ) { //right-aligned
                    tmpWriter.addAttribute("style:type", "right");
                }
                else { //3 = decimal tab -> align on decimal point
                    //4 = bar -> just creates a vertical bar at that point that's always visible
                    kWarning(30513) << "Unhandled tab justification code: " << td.tbd.jc;
                }
            }
            //td.tbd.tlc = tab leader code
            if ( td.tbd.tlc )//0 = no leader, which is default & can just be ignored
            {
                if ( td.tbd.tlc == 1 ) { //1 dotted leader
                    tmpWriter.addAttribute("style:leader-text", ".");
                }
                else if (td.tbd.tlc == 2 ) { //2 hyphenated leader
                    tmpWriter.addAttribute("style:leader-text", "-");
                }
                //TODO 3 single line leader
                //TODO 4 heavy line leader
            }
            tmpWriter.endElement();//style:tab-stop
        }
        tmpWriter.endElement();//style:tab-stops
        buf.close();
        QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        //now write the tab info to the paragraph style
        style->addChildElement("style:tab-stops", contents);
    }
} //end parseParagraphProperties

void Paragraph::parseCharacterProperties( const wvWare::Word97::CHP* chp, KoGenStyle* style, const wvWare::Style* parentStyle )
{
    //if we have a named style, set its CHP as the refChp
    const wvWare::Word97::CHP* refChp;
    if ( parentStyle )
    {
        refChp = &parentStyle->chp();
    }
    else
    {
        refChp = 0;
    }

    //ico = color of text
    if ( !refChp || refChp->ico != chp->ico )
    {
        QString color = Conversion::color( chp->ico, -1 );
        style->addProperty(QString("fo:color"), color, KoGenStyle::TextType);
    }

    //hps = font size in half points
    if ( !refChp || refChp->hps != chp->hps )
    {
        style->addPropertyPt(QString("fo:font-size"), (int)(chp->hps/2), KoGenStyle::TextType);
    }

    //fBold = bold text if 1
    if ( !refChp || refChp->fBold != chp->fBold )
        style->addProperty(QString("fo:font-weight"), chp->fBold ? QString("bold") : QString("normal"), KoGenStyle::TextType);

    //fItalic = italic text if 1
    if ( !refChp || refChp->fItalic != chp->fItalic )
        style->addProperty(QString("fo:font-style"), chp->fItalic ? QString("italic") : QString("normal"), KoGenStyle::TextType);

    //kul: underline code
    if ( !refChp || refChp->kul != chp->kul )
    {
        switch ( chp->kul ) {
        case 0: //none
            style->addProperty(QString("style:text-underline-style"), QString("none") , KoGenStyle::TextType);
            break;
        case 1: // single
            style->addProperty( "style:text-underline-style", "solid" , KoGenStyle::TextType);
            break;
        case 2: // by word
            style->addProperty( "style:text-underline-style", "solid" , KoGenStyle::TextType);
            style->addProperty( "style:text-underline-mode", "skip-white-space" , KoGenStyle::TextType);
            break;
        case 3: // double
            style->addProperty( "style:text-underline-style", "solid" , KoGenStyle::TextType);
            style->addProperty( "style:text-underline-type", "double", KoGenStyle::TextType );
            break;
        case 4: // dotted
            style->addProperty( "style:text-underline-style", "dotted", KoGenStyle::TextType );
            break;
        case 5: // hidden - This makes no sense as an underline property!
            //I guess we could change this to have an underline the same color
            //as the background?
            style->addProperty( "style:text-underline-type", "none", KoGenStyle::TextType );
            break;
        case 6: // thick
            style->addProperty( "style:text-underline-style", "solid", KoGenStyle::TextType );
            style->addProperty( "style:text-underline-weight", "thick", KoGenStyle::TextType );
            break;
        case 7: //dash
            style->addProperty( "style:text-underline-style", "dash", KoGenStyle::TextType );
            break;
        case 8: //dot (not used, says the docu)
            break;
        case 9: //dot dash
            style->addProperty( "style:text-underline-style", "dot-dash", KoGenStyle::TextType );
            break;
        case 10: //dot dot dash
            style->addProperty( "style:text-underline-style", "dot-dot-dash", KoGenStyle::TextType );
            break;
        case 11: //wave
            style->addProperty( "style:text-underline-style", "wave", KoGenStyle::TextType );
            break;
        default:
            style->addProperty( "style:text-underline-style", "none", KoGenStyle::TextType );
        };
    }
    //fstrike = use strikethrough if 1
    //fDStrike = use double strikethrough if 1
    if ( !refChp || refChp->fStrike != chp->fStrike || refChp->fDStrike != chp->fDStrike )
    {
        if ( chp->fStrike )
            style->addProperty( "style:text-line-through-type", "single", KoGenStyle::TextType );
        else if ( chp->fDStrike )
            style->addProperty( "style:text-line-through-type", "double", KoGenStyle::TextType );
        else
            style->addProperty( "style:text-line-through-type", "none", KoGenStyle::TextType );
    }

    //font attribute (uppercase, lowercase (not in MSWord), small caps)
    //fCaps = displayed with all caps when 1, no caps when 0
    //fSmallCaps = displayed with small caps when 1, no small caps when 0
    if ( !refChp || refChp->fCaps != chp->fCaps || refChp->fSmallCaps != chp->fSmallCaps )
    {
        if ( chp->fCaps )
            style->addProperty( "fo:text-transform", "uppercase", KoGenStyle::TextType );
        if ( chp->fSmallCaps )
            style->addProperty( "fo:font-variant", "small-caps", KoGenStyle::TextType );
    }

    //iss = superscript/subscript indices
    if ( !refChp || refChp->iss != chp->iss )
    {
        if ( chp->iss == 1 ) //superscript
            style->addProperty( "style:text-position", "super", KoGenStyle::TextType );
        else if ( chp->iss == 2 ) //subscript
            style->addProperty( "style:text-position", "sub", KoGenStyle::TextType );
    }

    //fHighlight = when 1, characters are highlighted with color specified by chp.icoHighlight
    //icoHighlight = highlight color (see chp.ico)
    if ( !refChp || refChp->fHighlight != chp->fHighlight || refChp->icoHighlight != chp->icoHighlight ) {
        if ( chp->fHighlight )
        {
            QString color = Conversion::color( chp->icoHighlight, -1 );
            style->addProperty( "fo:background-color", color, KoGenStyle::TextType );
        } else {
            //TODO this should really be the surrounding background color
            style->addProperty( "fo:background-color", QString("#FFFFFF"), KoGenStyle::TextType);
        }
    }

    //fShadow = text has shadow if 1
    //fImprint = text engraved if 1
    if ( !refChp || refChp->fShadow != chp->fShadow || refChp->fImprint != chp->fImprint )
    {
        if ( chp->fShadow )
            style->addProperty( "style:text-shadow", "1pt", KoGenStyle::TextType );
        if ( chp->fImprint )
            style->addProperty( "style:font-relief", "engraved", KoGenStyle::TextType );
    }

}

