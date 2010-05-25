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

Paragraph::Paragraph(KoGenStyles* mainStyles, bool inStylesDotXml, bool isHeading, bool inHeaderFooter, int outlineLevel)
        : m_paragraphProperties(0),
        m_paragraphProperties2(0),
        m_odfParagraphStyle(0),
        m_odfParagraphStyle2(0),
        m_mainStyles(0),
        m_paragraphStyle(0),
        m_paragraphStyle2(0),
        m_inStylesDotXml(inStylesDotXml),
        m_isHeading(isHeading),
        m_outlineLevel(0),
        m_dropCapStatus(NoDropCap),
        m_inHeaderFooter(inHeaderFooter),
        m_containsPageNumberField(false),
	m_combinedCharacters(false)
{
    kDebug(30513);
    m_mainStyles = mainStyles;
    m_odfParagraphStyle = new KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");
    if (inStylesDotXml) {
        kDebug(30513) << "this paragraph is in styles.xml";
        m_inStylesDotXml = true;
        //if we're writing to styles.xml, the style should go there, too
        m_odfParagraphStyle->setAutoStyleInStylesDotXml(true);
    }

    if (isHeading)     {
        kDebug(30513) << "this paragraph is a heading";
        m_outlineLevel = (outlineLevel > 0 ? outlineLevel : 1);
    } else {
        m_outlineLevel = -1;
    }
}

Paragraph::~Paragraph()
{
    delete m_odfParagraphStyle;
    m_odfParagraphStyle = 0;
}

void Paragraph::addRunOfText(QString text,  wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                             QString fontName, const wvWare::StyleSheet& styles,
                             bool addCompleteElement)
{
    // Check for column break in this text string
    int colBreak = text.indexOf(QChar(0xE));

    //I think this break always comes at the beginning of the text string
    if (colBreak == 0) {
        kDebug(30513) << "colBreak = " << colBreak;

        // Add needed attribute to paragraph style.
        //
        // Note: This logic breaks down if this isn't the first string
        //       in the paragraph, or there are other strings with
        //       another colBreak later in the same paragraph.
        m_odfParagraphStyle->addProperty("fo:break-before", "column", KoGenStyle::ParagraphType);
        // Remove character that signaled a column break
        text.remove(QChar(0xE));
    }

    // if it's inner paragraph, push back true
    // this is an m_textStyles.push_back(NULL) complement if we still need the style applied
    m_addCompleteElement.push_back(addCompleteElement);

    // Add text string to list.
    //m_textStrings.push_back(QString(text));
    m_textStrings.append(QString(text));

    // Now find out what style to associate with the string.

    if (chp == 0) {
        // if inner paragraph - just add a null style & return from function
        KoGenStyle* style = 0;
        m_textStyles.push_back(style);
        return;
    }

    const wvWare::Style* msTextStyle = styles.styleByIndex(chp->istd);
    Q_ASSERT(msTextStyle);
    QString msTextStyleName = Conversion::styleNameString(msTextStyle->name());
    kDebug(30513) << "text has characterstyle " << msTextStyleName;

    KoGenStyle *textStyle = m_mainStyles->styleForModification(msTextStyleName);
    if (!textStyle) {
        kWarning() << "Couldn't retrieve style for modification!";
    }

    bool suppresFontSize = false;
    if (m_textStyles.size() == 0 && m_paragraphProperties->pap().dcs.lines > 1) {
        suppresFontSize = true;
    }

    // Modify the character style if we detect any diff between the
    // chp of the paragraph and the summed chp.
    const wvWare::Style* parentStyle = styles.styleByIndex(msTextStyle->m_std->istdBase);
    if (parentStyle) {
        applyCharacterProperties(chp, textStyle, m_paragraphStyle, suppresFontSize, m_combinedCharacters);
        //if we have a new font, process that
        const wvWare::Word97::CHP* refChp = &m_paragraphStyle->chp();
        if (!refChp || refChp->ftcAscii != chp->ftcAscii) {
            if (!fontName.isEmpty()) {
                textStyle->addProperty(QString("style:font-name"), fontName, KoGenStyle::TextType);
            }
        }
    } else {
        textStyle = new KoGenStyle(KoGenStyle::TextAutoStyle, "text");
        if (m_inStylesDotXml) {
            textStyle->setAutoStyleInStylesDotXml(true);
        }
        if (!fontName.isEmpty()) {
            textStyle->addProperty(QString("style:font-name"), fontName, KoGenStyle::TextType);
        }
        applyCharacterProperties(chp, textStyle, m_paragraphStyle, suppresFontSize, m_combinedCharacters);
    }

    //add text style to list
    m_textStyles.push_back(textStyle);
}

void Paragraph::writeToFile(KoXmlWriter* writer)
{
    kDebug(30513);

    // Set up the paragraph style.
    applyParagraphProperties(*m_paragraphProperties, m_odfParagraphStyle, m_paragraphStyle,
                             m_inHeaderFooter && m_containsPageNumberField, this);
 
    // MS Word puts dropcap characters in its own paragraph with the
    // rest of the text in the subsequent paragraph. On the other
    // hand, ODF wants the whole paragraph to be one unit.
    //
    // So if this paragraph is only one string and we have a dropcap,
    // then we should combine this paragraph with the next, and write
    // them together.  That means we should just return here now so
    // that the next call to writeToFile() they will be in the same
    // paragraph.
    if (m_dropCapStatus == IsDropCapPara) {
        kDebug(30513) << "returning with drop cap paragraph";
        return;
    }

    // If there is a dropcap defined, then write it to the style.
    if (m_dropCapStatus == HasDropCapIntegrated) {
        kDebug(30513) << "Creating drop cap style";

        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        KoXmlWriter tmpWriter(&buf, 3);
        tmpWriter.startElement("style:drop-cap");
        tmpWriter.addAttribute("style:lines", m_dcs_lines);
        tmpWriter.addAttributePt("style:distance", m_dropCapDistance);
        // We have only support for fdct=1, i.e. regular dropcaps.
        // There is no support for fdct=2, i.e. dropcaps in the margin (ODF doesn't support this).
        tmpWriter.addAttribute("style:length", m_dcs_fdct > 0 ? 1 : 0);
        tmpWriter.endElement();//style:drop-cap
        buf.close();

        QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        m_odfParagraphStyle->addChildElement("style:drop-cap", contents);

        m_dropCapStatus = NoDropCap;
    }

    QString textStyleName;
    //add paragraph style to the collection and its name to the content
    kDebug(30513) << "adding paragraphStyle";
    //add the attribute for our style in <text:p>
    textStyleName = "P";
    textStyleName = m_mainStyles->insert(*m_odfParagraphStyle, textStyleName);

    // check if the paragraph is inside frame
    if (m_paragraphProperties->pap().dxaAbs != 0 || m_paragraphProperties->pap().dyaAbs ) {
        KoGenStyle userStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        QString drawStyleName;

        writer->startElement("text:p", false);
        writer->addAttribute("text:style-name", textStyleName.toUtf8());

        int dxaAbs = 0;
        const wvWare::Word97::PAP& pap = m_paragraphProperties->pap();
        //MS-DOC - sprmPDxaAbs - relative horizontal position to anchor
        if (pap.dxaAbs == -4)   // center
            userStyle.addProperty("style:horizontal-pos","center");
        else if (pap.dxaAbs == -8)  // right
            userStyle.addProperty("style:horizontal-pos","right");
        else if (pap.dxaAbs == -12)  // inside
            userStyle.addProperty("style:horizontal-pos","inside");
        else if (pap.dxaAbs == -16)  // outside
            userStyle.addProperty("style:horizontal-pos","outside");
        else { //
            dxaAbs = pap.dxaAbs;
            userStyle.addProperty("style:horizontal-pos","from-left");
        }

        int dyaAbs = 0;
        //MS-DOC - sprmPDyaAbs - relative vertical position to anchor
        if (pap.dyaAbs == -4)   // top
            userStyle.addProperty("style:vertical-pos","top");
        else if (pap.dyaAbs == -8)  // middle
            userStyle.addProperty("style:vertical-pos","middle");
        else if (pap.dyaAbs == -12)  // bottom
            userStyle.addProperty("style:vertical-pos","bottom");
        else if (pap.dyaAbs == -16)  // inside
            userStyle.addProperty("style:vertical-pos","inline");
        else if (pap.dyaAbs == -20)  // outside
            userStyle.addProperty("style:vertical-pos","inline");
        else {//
            dyaAbs = pap.dyaAbs;
            userStyle.addProperty("style:vertical-pos","from-top");
        }

        //MS-DOC - PositionCodeOperand - anchor vertical position
        if (pap.pcVert == 0)   // margin
            userStyle.addProperty("style:vertical-rel","page-content");
        else if (pap.pcVert == 1)  // page
            userStyle.addProperty("style:vertical-rel","page");
        else if (pap.pcVert == 2)  // paragraph
            userStyle.addProperty("style:vertical-rel","paragraph");

        //MS-DOC - PositionCodeOperand - anchor horizontal position
        if (pap.pcHorz == 0)   // current column
            userStyle.addProperty("style:horizontal-rel","paragraph");
        else if (pap.pcHorz == 1)  // margin
            userStyle.addProperty("style:horizontal-rel","page-content");
        else if (pap.pcHorz == 2)  // page
            userStyle.addProperty("style:horizontal-rel","page");

        drawStyleName = "fr";
        drawStyleName = m_mainStyles->insert(userStyle, drawStyleName);
        writer->startElement("draw:frame");
        writer->addAttribute("draw:style-name", drawStyleName.toUtf8());
        writer->addAttribute("text:anchor-type", "paragraph");
        if (pap.dxaWidth != 0)
            writer->addAttributePt("svg:width", (double)pap.dxaWidth/20);
        if (pap.dyaHeight != 0)
            writer->addAttributePt("svg:height", (double)pap.dyaHeight/20);
        writer->addAttributePt("svg:x", (double)dxaAbs/20);
        writer->addAttributePt("svg:y", (double)dyaAbs/20);
        writer->startElement("draw:text-box");
    }

    // Open paragraph or heading tag.
    if (m_isHeading) {
        writer->startElement("text:h", false);
        writer->addAttribute("text:outline-level", m_outlineLevel);
    } else {
        writer->startElement("text:p", false);
    }

    writer->addAttribute("text:style-name", textStyleName.toUtf8());

    //just close the paragraph if there's no content
    //if (m_textStrings.empty()) {
    if (m_textStrings.isEmpty()) {
        writer->endElement(); //text:p
        return;
    }

    // Loop through each text strings and styles (equal # of both) and
    // write them to the file.
    kDebug(30513) << "writing text spans now";
    QString oldStyleName;
    bool startedSpan = false;
    for (int i = 0; i < m_textStrings.size(); i++) {
        if (m_textStyles[i] == 0) {
            //if style is null, we have an inner paragraph and add the
            // complete paragraph element to writer
            //need to get const char* from the QString
            kDebug(30513) << "complete element: " << m_textStrings[i].toLocal8Bit().constData();
            writer->addCompleteElement(m_textStrings[i].toLocal8Bit().constData());
        } else {
            //add text style to collection
            //put style into m_mainStyles & get its name
            //kDebug(30513) << m_textStyles[i]->type();
            textStyleName = 'T';
            textStyleName = m_mainStyles->insert(*m_textStyles[i], textStyleName);

            if (oldStyleName != textStyleName) {
                if (startedSpan) {
                    writer->endElement(); //text:span
                    startedSpan = false;
                }
                if (textStyleName != "DefaultParagraphFont") {
                    writer->startElement("text:span");
                    writer->addAttribute("text:style-name", textStyleName.toUtf8());
                    startedSpan = true;
                }
                oldStyleName = textStyleName;
            }
            // Write text string to writer.
            // Now I just need to write the text:span to the header tag.
            kDebug(30513) << "Writing \"" << m_textStrings[i] << "\"";
            if (m_addCompleteElement[i] == false) {
                writer->addTextSpan(m_textStrings[i]);
            } else {            // special case we need style applied and complete element added
                writer->addCompleteElement(m_textStrings[i].toLocal8Bit().constData());
            }

            //cleanup
            //delete m_textStyles[i];
            m_textStyles[i] = 0;
        }
    }

    // If we have an unfinished text:span, finish it now.
    if (startedSpan) {
        writer->endElement(); //text:span
        startedSpan = false;
    }

    //close the <text:p> or <text:h> tag we opened
    writer->endElement();

    if (m_paragraphProperties->pap().dxaAbs != 0 || m_paragraphProperties->pap().dyaAbs ) {
        writer->endElement(); //draw:text-box
        writer->endElement(); // draw:frame
        writer->endElement(); // close the <text:p>
    }
}

void Paragraph::setParagraphStyle(const wvWare::Style* paragraphStyle)
{
    kDebug(30513);
    m_paragraphStyle = paragraphStyle;
    m_odfParagraphStyle->addAttribute("style:parent-style-name",
                                      Conversion::styleNameString(m_paragraphStyle->name()));
}

KoGenStyle* Paragraph::getOdfParagraphStyle()
{
    return m_odfParagraphStyle;
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
    m_odfParagraphStyle2 = m_odfParagraphStyle;
    m_odfParagraphStyle = new KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");
    m_paragraphStyle2 = m_paragraphStyle;
    m_paragraphProperties2 = m_paragraphProperties;
    m_paragraphProperties = 0;

    //move m_textStyles and m_textStrings content to
    //m_textStyles2 and m_textStrings2
    m_textStyles2 = m_textStyles;
    m_textStrings2 = m_textStrings;
    m_addCompleteElement2 = m_addCompleteElement;
    m_textStyles.clear();
    m_textStrings.clear();
    m_addCompleteElement.clear();
}

void Paragraph::closeInnerParagraph()
{
    kDebug(30513);

    //clear temp variables and restore originals
    delete m_odfParagraphStyle;
    m_odfParagraphStyle = m_odfParagraphStyle2;
    m_odfParagraphStyle2 = 0;
    m_paragraphStyle = m_paragraphStyle2;
    m_paragraphStyle2 = 0;
    m_paragraphProperties = m_paragraphProperties2;
    m_paragraphProperties2 = 0;
    m_textStyles.clear();
    m_textStrings.clear();
    m_addCompleteElement.clear();
    m_textStyles = m_textStyles2;
    m_textStrings = m_textStrings2;
    m_addCompleteElement = m_addCompleteElement2;
    m_textStyles2.clear();
    m_textStrings2.clear();
    m_addCompleteElement2.clear();
}

void Paragraph::setParagraphProperties(wvWare::SharedPtr<const wvWare::ParagraphProperties> properties)
{
    m_paragraphProperties = properties;
}

void Paragraph::applyParagraphProperties(const wvWare::ParagraphProperties& properties,
                                         KoGenStyle* style, const wvWare::Style* parentStyle,
                                         bool setDefaultAlign, Paragraph *paragraph)
{
    kDebug(30513);

    const wvWare::Word97::PAP* refPap;
    if (parentStyle) {
        refPap = &parentStyle->paragraphProperties().pap();
    } else {
        refPap = 0;
    }

    //pap is our paragraph properties object
    const wvWare::Word97::PAP& pap = properties.pap();

    //paragraph alignment
    //jc = justification code
    if (!refPap || refPap->jc != pap.jc) {
        if (pap.jc == 1)   //1 = center justify
            style->addProperty("fo:text-align", "center", KoGenStyle::ParagraphType);
        else if (pap.jc == 2)  //2 = right justify
            style->addProperty("fo:text-align", "end", KoGenStyle::ParagraphType);
        else if (pap.jc == 3)  //3 = left & right justify
            style->addProperty("fo:text-align", "justify", KoGenStyle::ParagraphType);
        else if (pap.jc == 4)  //4 = distributed .. fake it as justify
            style->addProperty("fo:text-align", "justify", KoGenStyle::ParagraphType);
        else //0 = left justify
            style->addProperty("fo:text-align", "start", KoGenStyle::ParagraphType);
    } else if (setDefaultAlign) {
        // Set default align for page number field in header or footer
        kDebug(30513) << "setting default align for page number field in header or footer";
        style->addProperty("fo:text-align", "center", KoGenStyle::ParagraphType);
    }

    if (!refPap || refPap->fBiDi != pap.fBiDi) {
        if (pap.fBiDi == 1)   //1 = right to left
            style->addProperty("style:writing-mode", "rl-tb", KoGenStyle::ParagraphType);
        else //0 = normal
            style->addProperty("style:writing-mode", "lr-tb", KoGenStyle::ParagraphType);
    }

    if (!refPap ||                                                  // if there is no parent style OR
        refPap->shd.cvBack != pap.shd.cvBack ||                     // the parent and child background color don't match OR
        (refPap->shd.shdAutoOrNill && !pap.shd.shdAutoOrNill) ) {   // parent color was invalid, childs color is valid
        if (!pap.shd.shdAutoOrNill) {                               // is the color valid? (don't compare to black - 0xff000000 !!!)
            style->addProperty(QString("fo:background-color"), '#' + QString::number(pap.shd.cvBack | 0xff000000, 16).right(6).toUpper());
        } else {
            style->addProperty("fo:background-color", "transparent", KoGenStyle::ParagraphType);
        }
    }

    //dxaLeft1 = first-line indent from left margin (signed, relative to dxaLeft)
    //dxaLeft = indent from left margin (signed)
    //dxaRight = indent from right margin (signed)
    if (!refPap || refPap->dxaLeft != pap.dxaLeft) {
        // apply twip -> pt conversion
        style->addPropertyPt("fo:margin-left", (double)pap.dxaLeft / 20.0, KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->dxaRight != pap.dxaRight) {
        // apply twip -> pt conversion
        style->addPropertyPt("fo:margin-right", (double)pap.dxaRight / 20.0, KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->dxaLeft1 != pap.dxaLeft1) {
        // apply twip -> pt conversion
        style->addPropertyPt("fo:text-indent", (double)pap.dxaLeft1 / 20.0, KoGenStyle::ParagraphType);
    }

    //dyaBefore = vertical spacing before paragraph (unsigned)
    //dyaAfter = vertical spacing after paragraph (unsigned)
    if (!refPap || refPap->dyaBefore != pap.dyaBefore) {
        // apply twip -> pt conversion
        style->addPropertyPt("fo:margin-top", (double)pap.dyaBefore / 20.0, KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->dyaAfter != pap.dyaAfter) {
        // apply twip -> pt conversion
        style->addPropertyPt("fo:margin-bottom", (double)pap.dyaAfter / 20.0, KoGenStyle::ParagraphType);
    }

    // Linespacing
    //lspd = line spacing descriptor
    //Conversion::lineSpacing() returns "0" (default), "oneandhalf," or "double"
    //QString lineSpacingAttribute = Conversion::lineSpacing( pap.lspd );
    if (!refPap || refPap->lspd.fMultLinespace != pap.lspd.fMultLinespace
            || refPap->lspd.dyaLine != pap.lspd.dyaLine) {

        if (pap.lspd.fMultLinespace == 1) {
            // Word will reserve for each line the (maximal height of
            // the line*lspd.dyaLine)/240
            //
            // Get the proportion & turn it into a percentage for the
            // attribute.
            QString proportionalLineSpacing(QString::number(((qreal)pap.lspd.dyaLine
                                                  / (qreal)240.0)*(qreal)120.0));
            style->addProperty("fo:line-height", proportionalLineSpacing.append("%"), KoGenStyle::ParagraphType);
        } else if (pap.lspd.fMultLinespace == 0) {
            // Magnitude of lspd.dyaLine specifies the amount of space
            // that will be provided for lines in the paragraph in twips.
            //
            // See sprmPDyaLine in generator_wword8.htm
            qreal value = qAbs((qreal)pap.lspd.dyaLine / (qreal)20.0); // twip -> pt
            // lspd.dyaLine > 0 means "at least", < 0 means "exactly"
            if (pap.lspd.dyaLine > 0)
                style->addPropertyPt("fo:line-height-at-least", value, KoGenStyle::ParagraphType);
            else if (pap.lspd.dyaLine < 0 && pap.dcs.fdct==0)
                style->addPropertyPt("fo:line-height", value, KoGenStyle::ParagraphType);
        } else
            kWarning(30513) << "Unhandled LSPD::fMultLinespace value: "
            << pap.lspd.fMultLinespace;
    }
    // end linespacing

    //fKeep = keep entire paragraph on one page if possible
    //fKeepFollow = keep paragraph on same page with next paragraph if possible
    //fPageBreakBefore = start this paragraph on new page
    if (!refPap || refPap->fKeep != pap.fKeep) {
        if (pap.fKeep)
            style->addProperty("fo:keep-together", "always", KoGenStyle::ParagraphType);
        else
            style->addProperty("fo:keep-together", "auto", KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->fKeepFollow != pap.fKeepFollow) {
        if (pap.fKeepFollow)
            style->addProperty("fo:keep-with-next", "always", KoGenStyle::ParagraphType);
        else
            style->addProperty("fo:keep-with-next", "auto", KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->fPageBreakBefore != pap.fPageBreakBefore) {
        if (pap.fPageBreakBefore)
            style->addProperty("fo:break-before", "page", KoGenStyle::ParagraphType);
        else
            style->addProperty("fo:break-before", "auto", KoGenStyle::ParagraphType);
    }

    // Borders
    //brcTop = specification for border above paragraph
    //brcBottom = specification for border below paragraph
    //brcLeft = specification for border to the left of paragraph
    //brcRight = specification for border to the right of paragraph
    //brcType: 0=none, 1=single, 2=thick, 3=double, 5=hairline, 6=dot,
    //  7=dash large gap, 8=dot dash, 9=dot dot dash, 10=triple,
    //  11=thin-thick small gap, ...
    //
    // TODO: Check if we can use fo:border instead of all the
    //       fo:border-{top,bottom,left,right}
    // TODO: Check if borderStyle is "double" and add attributes for that.
    if (!refPap || refPap->brcTop.brcType != pap.brcTop.brcType) {
        style->addProperty("fo:border-top", Conversion::setBorderAttributes(pap.brcTop), KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->brcBottom.brcType != pap.brcBottom.brcType) {
        style->addProperty("fo:border-bottom", Conversion::setBorderAttributes(pap.brcBottom), KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->brcLeft.brcType != pap.brcLeft.brcType) {
        style->addProperty("fo:border-left", Conversion::setBorderAttributes(pap.brcLeft), KoGenStyle::ParagraphType);
    }
    if (!refPap || refPap->brcRight.brcType != pap.brcRight.brcType) {
        style->addProperty("fo:border-right", Conversion::setBorderAttributes(pap.brcRight), KoGenStyle::ParagraphType);
    }

    // Drop Cap Style (DCS)
    if (!refPap || refPap->dcs.fdct != pap.dcs.fdct || refPap->dcs.lines != pap.dcs.lines) {
        if (paragraph) {
            kDebug(30513) << "Processing drop cap";
            if (paragraph->m_textStrings.size() > 0)
                kDebug(30513) << "String = """ << paragraph->m_textStrings[0] << """";
            else
                kDebug(30513) << "No drop cap string";

            paragraph->m_dropCapStatus = IsDropCapPara;
            paragraph->m_dcs_fdct  = pap.dcs.fdct;
            paragraph->m_dcs_lines = pap.dcs.lines;
            paragraph->m_dropCapDistance = (qreal)pap.dxaFromText / (qreal)20.0;
        }
#if 0
        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        KoXmlWriter tmpWriter(&buf, 3);
         tmpWriter.startElement("style:drop-cap");
        tmpWriter.addAttribute("style:lines", pap.dcs.lines);
        tmpWriter.addAttributePt("style:distance", (qreal)pap.dxaFromText / (qreal)20.0);
        tmpWriter.addAttribute("style:length", pap.dcs.fdct > 0 ? 1 : 0);
        tmpWriter.endElement();//style:drop-cap
        buf.close();

        QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        style->addChildElement("style:drop-cap", contents);
#endif
    }

//TODO introduce diff for tabs too like in: if(!refPap || refPap->fKeep != pap

    // Tabulators
    //itbdMac = number of tabs stops defined for paragraph. Must be >= 0 and <= 64.
    if (pap.itbdMac) {
        kDebug(30513) << "processing tab stops";
        //looks like we need to write these out with an xmlwriter
        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        KoXmlWriter tmpWriter(&buf, 3);//root, office:automatic-styles, style:style
        tmpWriter.startElement("style:tab-stops");
        for (int i = 0 ; i < pap.itbdMac ; ++i) {
            tmpWriter.startElement("style:tab-stop");

            //rgdxaTab = array of { positions of itbdMac tab stops ; itbdMac tab descriptors } itbdMax == 64.
            const wvWare::Word97::TabDescriptor &td = pap.rgdxaTab[i];
            //td.dxaTab = position in twips
            //QString pos( QString::number( (double)td.dxaTab / 20.0 ) );
            tmpWriter.addAttributePt("style:position", (double)td.dxaTab / 20.0);

            //td.tbd.jc = justification code
            if (td.tbd.jc) { //0 = left-aligned = default, so that can just be ignored
                if (td.tbd.jc == 1) {   //centered
                    tmpWriter.addAttribute("style:type", "center");
                } else if (td.tbd.jc == 2) {  //right-aligned
                    tmpWriter.addAttribute("style:type", "right");
                } else { //3 = decimal tab -> align on decimal point
                    //4 = bar -> just creates a vertical bar at that point that's always visible
                    kWarning(30513) << "Unhandled tab justification code: " << td.tbd.jc;
                }
            }
            //td.tbd.tlc = tab leader code
            if (td.tbd.tlc) { //0 = no leader, which is default & can just be ignored
                if (td.tbd.tlc == 1) {   //1 dotted leader
                    tmpWriter.addAttribute("style:leader-text", ".");
                } else if (td.tbd.tlc == 2) { //2 hyphenated leader
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
} //end applyParagraphProperties

void Paragraph::applyCharacterProperties(const wvWare::Word97::CHP* chp, KoGenStyle* style, const wvWare::Style* parentStyle, bool suppressFontSize, bool combineCharacters)
{
    //if we have a named style, set its CHP as the refChp
    const wvWare::Word97::CHP* refChp;
    if (parentStyle) {
        refChp = &parentStyle->chp();
    } else {
        refChp = 0;
    }

    //ico = color of text, but this has been replaced by cv
    if (!refChp || refChp->cv != chp->cv) {
        style->addProperty(QString("fo:color"), '#' + QString::number(chp->cv | 0xff000000, 16).right(6).toUpper(), KoGenStyle::TextType);
    }

    //hps = font size in half points
    if (!suppressFontSize && (!refChp || refChp->hps != chp->hps)) {
        style->addPropertyPt(QString("fo:font-size"), (int)(chp->hps / 2), KoGenStyle::TextType);
    }

    //fBold = bold text if 1
    if (!refChp || refChp->fBold != chp->fBold)
        style->addProperty(QString("fo:font-weight"), chp->fBold ? QString("bold") : QString("normal"), KoGenStyle::TextType);

    //fItalic = italic text if 1
    if (!refChp || refChp->fItalic != chp->fItalic)
        style->addProperty(QString("fo:font-style"), chp->fItalic ? QString("italic") : QString("normal"), KoGenStyle::TextType);

    //kul: underline code
    if (!refChp || refChp->kul != chp->kul) {
        switch (chp->kul) {
        case 0: //none
            style->addProperty(QString("style:text-underline-style"), QString("none") , KoGenStyle::TextType);
            break;
        case 1: // single
            style->addProperty("style:text-underline-style", "solid" , KoGenStyle::TextType);
            break;
        case 2: // by word
            style->addProperty("style:text-underline-style", "solid" , KoGenStyle::TextType);
            style->addProperty("style:text-underline-mode", "skip-white-space" , KoGenStyle::TextType);
            break;
        case 3: // double
            style->addProperty("style:text-underline-style", "solid" , KoGenStyle::TextType);
            style->addProperty("style:text-underline-type", "double", KoGenStyle::TextType);
            break;
        case 4: // dotted
            style->addProperty("style:text-underline-style", "dotted", KoGenStyle::TextType);
            break;
        case 5: // hidden - This makes no sense as an underline property!
            //I guess we could change this to have an underline the same color
            //as the background?
            style->addProperty("style:text-underline-type", "none", KoGenStyle::TextType);
            break;
        case 6: // thick
            style->addProperty("style:text-underline-style", "solid", KoGenStyle::TextType);
            style->addProperty("style:text-underline-weight", "thick", KoGenStyle::TextType);
            break;
        case 7: //dash
            style->addProperty("style:text-underline-style", "dash", KoGenStyle::TextType);
            break;
        case 8: //dot (not used, says the docu)
            break;
        case 9: //dot dash
            style->addProperty("style:text-underline-style", "dot-dash", KoGenStyle::TextType);
            break;
        case 10: //dot dot dash
            style->addProperty("style:text-underline-style", "dot-dot-dash", KoGenStyle::TextType);
            break;
        case 11: //wave
            style->addProperty("style:text-underline-style", "wave", KoGenStyle::TextType);
            break;
        default:
            style->addProperty("style:text-underline-style", "none", KoGenStyle::TextType);
        };
    }
    //fstrike = use strikethrough if 1
    //fDStrike = use double strikethrough if 1
    if (!refChp || refChp->fStrike != chp->fStrike || refChp->fDStrike != chp->fDStrike) {
        if (chp->fStrike)
            style->addProperty("style:text-line-through-type", "single", KoGenStyle::TextType);
        else if (chp->fDStrike)
            style->addProperty("style:text-line-through-type", "double", KoGenStyle::TextType);
        else
            style->addProperty("style:text-line-through-type", "none", KoGenStyle::TextType);
    }

    //font attribute (uppercase, lowercase (not in MSWord), small caps)
    //fCaps = displayed with all caps when 1, no caps when 0
    //fSmallCaps = displayed with small caps when 1, no small caps when 0
    if (!refChp || refChp->fCaps != chp->fCaps || refChp->fSmallCaps != chp->fSmallCaps) {
        if (chp->fCaps)
            style->addProperty("fo:text-transform", "uppercase", KoGenStyle::TextType);
        if (chp->fSmallCaps)
            style->addProperty("fo:font-variant", "small-caps", KoGenStyle::TextType);
    }

    //iss = superscript/subscript indices
    if (!refChp || refChp->iss != chp->iss) {
        if (chp->iss == 1)   //superscript
            style->addProperty("style:text-position", "super", KoGenStyle::TextType);
        else if (chp->iss == 2)   //subscript
            style->addProperty("style:text-position", "sub", KoGenStyle::TextType);
    }

    //fHighlight = when 1, characters are highlighted with color specified by chp.icoHighlight
    //icoHighlight = highlight color (see chp.ico)
    if (!refChp || refChp->fHighlight != chp->fHighlight || refChp->icoHighlight != chp->icoHighlight) {
        if (chp->fHighlight) {
            QString color = Conversion::color(chp->icoHighlight, -1);
            style->addProperty("fo:background-color", color, KoGenStyle::TextType);
        } else {
            style->addProperty("fo:background-color", "transparent", KoGenStyle::TextType);
        }
    }

    if (!refChp || refChp->shd.cvBack != chp->shd.cvBack) {
        if (chp->shd.cvBack != 0xff000000)
            style->addProperty(QString("fo:background-color"), '#' + QString::number(chp->shd.cvBack | 0xff000000, 16).right(6).toUpper(), KoGenStyle::TextType);
        else
            style->addProperty("fo:background-color", "transparent", KoGenStyle::TextType);
    }

    //fShadow = text has shadow if 1
    //fImprint = text engraved if 1
    if (!refChp || refChp->fShadow != chp->fShadow || refChp->fImprint != chp->fImprint) {
        if (chp->fShadow)
            style->addProperty("style:text-shadow", "1pt", KoGenStyle::TextType);
        if (chp->fImprint)
            style->addProperty("style:font-relief", "engraved", KoGenStyle::TextType);
    }

    //fOutline = text is outline if 1
    if (!refChp || refChp->fOutline != chp->fOutline) {
        if (chp->fOutline)
            style->addProperty("style:text-outline", "true", KoGenStyle::TextType);
        else
            style->addProperty("style:text-outline", "false", KoGenStyle::TextType);
    }

    // if the characters are combined, add proper style
    if (combineCharacters) {
        style->addProperty("style:text-combine","letters");
    }

    //dxaSpace = letterspacing in twips
    if (!refChp || refChp->dxaSpace != chp->dxaSpace) {
        double value =  chp->dxaSpace / 20.0; // twips -> pt
        style->addPropertyPt("fo:letter-spacing", value, KoGenStyle::TextType);
    }
    //pctCharwidth = pct stretch doesn't seem to have an ODF ccounterpart but Qt could support it

    //fTNY = 1 when text is vertical
    if (!refChp || refChp->fTNY != chp->fTNY) {
        if (chp->fTNY) {
            style->addProperty("style:text-rotation-angle", 90);
            if (chp->fTNYCompress) {
                style->addProperty("style:text-rotation-scale", "fixed");
            } else {
                style->addProperty("style:text-rotation-scale", "line-height");
            }
        }
    }

    //wCharScale - MUST be greater than or equal to 1 and less than or equal to 600
    if (!refChp || refChp->wCharScale != chp->wCharScale) {
        if (chp->wCharScale) {
            style->addProperty("style:text-scale", chp->wCharScale);
        }
    }
}

void Paragraph::setCombinedCharacters(bool isCombined)
{
    m_combinedCharacters = isCombined;
}

Paragraph::DropCapStatus Paragraph::dropCapStatus() const
{
    return m_dropCapStatus;
}

void Paragraph::getDropCapData(QString *string, int *type, int *lines, qreal *distance) const
{
    // As far as I can see there is only ever a single character as drop cap.
    *string = m_textStrings[0];
    *type = m_dcs_fdct;
    *lines = m_dcs_lines;
    *distance = m_dropCapDistance;
}


void Paragraph::addDropCap(QString &string, int type, int lines, qreal distance)
{
    kDebug(30513) << "combining drop cap paragraph: " << string;
    if (m_dropCapStatus == IsDropCapPara) 
        kDebug(30513) << "This paragraph already has a dropcap set!";

    m_dropCapStatus = HasDropCapIntegrated;

    // Get the drop cap data.
    m_dcs_fdct        = type;
    m_dcs_lines       = lines;
    m_dropCapDistance = distance;

    // Add the actual text.
    // Here we assume that there will only be one text snippet for the drop cap.
#if 1

#if 1
    kDebug(30513) << "size: " << m_textStrings.size();
    m_textStrings[0].prepend(string);
#else
    m_textStrings.prepend(string);
    KoGenStyle* style = 0;
    m_textStyles.insert(m_textStyles.begin(), style);
#endif

#else
    std::vector<QString>            tempStrings;
    std::vector<const KoGenStyle*>  tempStyles;
    tempStrings.push_back(dropCapParagraph->m_textStrings[0]);
    KoGenStyle* style = 0;
    tempStyles.push_back(style);

    if (dropCapParagraph->m_textStrings.size() > 0) {
        for (uint i = 0; i < m_textStrings.size(); ++i) {
            tempStrings.push_back(m_textStrings.[i]);
            tempStyles.push_back(m_textStyles.[i]);
        }            
    }

    m_textStrings = tempStrings;
    m_textStyles  = tempStyles;
#endif
}


int Paragraph::strings() const
{
    return m_textStrings.size();
}

QString Paragraph::string(int index) const
{
    return m_textStrings[index];
}

