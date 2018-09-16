/* This file is part of the Calligra project

   Copyright (C) 2009 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2010-2012 Matus Uzak <matus.uzak@gmail.com>

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

#include <math.h>

#include "conversion.h"
#include "msdoc.h"
#include "MsDocDebug.h"

//define the static attribute
QStack<QString> Paragraph::m_bgColors;

//definition of local functions
const char* getStrokeValue(const uint brcType);
const char* getTextUnderlineMode(const uint kul);
const char* getTextUnderlineStyle(const uint kul);
const char* getTextUnderlineType(const uint kul);
const char* getTextUnderlineWidth(const uint kul);


Paragraph::Paragraph(KoGenStyles* mainStyles, const QString& bgColor, bool inStylesDotXml, bool isHeading,
                     bool inHeaderFooter, int outlineLevel)
        : m_paragraphProperties(0),
        m_paragraphProperties2(0),
        m_characterProperties(0),
        m_odfParagraphStyle(0),
        m_odfParagraphStyle2(0),
        m_mainStyles(0),
        m_paragraphStyle(0),
        m_paragraphStyle2(0),
        m_inStylesDotXml(inStylesDotXml),
        m_isHeading(isHeading),
        m_inHeaderFooter(inHeaderFooter),
        m_outlineLevel(0),
        m_dropCapStatus(NoDropCap),
        m_containsPageNumberField(false),
        m_combinedCharacters(false)
{
    debugMsDoc;
    m_mainStyles = mainStyles;
    m_odfParagraphStyle = new KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");

    if (inStylesDotXml) {
        debugMsDoc << "this paragraph is in styles.xml";
        m_odfParagraphStyle->setAutoStyleInStylesDotXml(true);
        m_inStylesDotXml = true;
    }

    if (isHeading)     {
        debugMsDoc << "this paragraph is a heading";
        m_outlineLevel = (outlineLevel > 0 ? outlineLevel : 1);
    } else {
        m_outlineLevel = -1;
    }

    //init the background-color stack to page background-color
    if (m_bgColors.size() > 0) {
        warnMsDoc << "BUG: m_bgColors stack NOT empty, clearing!";
        m_bgColors.clear();
    }

    if (!bgColor.isEmpty()) {
        pushBgColor(bgColor);
    } else {
        warnMsDoc << "Warning: page background-color information missing!";
    }
}

Paragraph::~Paragraph()
{
    delete m_odfParagraphStyle;
    m_odfParagraphStyle = 0;

    m_bgColors.clear();
}

void Paragraph::setParagraphStyle(const wvWare::Style* paragraphStyle)
{
    m_paragraphStyle = paragraphStyle;
    m_odfParagraphStyle->addAttribute("style:parent-style-name",
                                      Conversion::styleName2QString(m_paragraphStyle->name()));
}

void Paragraph::setParagraphProperties(wvWare::SharedPtr<const wvWare::ParagraphProperties> props)
{
    m_paragraphProperties = props;

    const wvWare::Word97::PAP* refPap = 0;
    if (m_paragraphStyle) {
        refPap = &m_paragraphStyle->paragraphProperties().pap();
    }
    const wvWare::Word97::PAP& pap = props->pap();

    QString color;

    //process the background-color information in order to calculate a
    //proper fo:color for the text in case of cvAuto.
    if (!refPap ||
        (refPap->shd.cvBack != pap.shd.cvBack) ||
        (refPap->shd.isShdAuto() != pap.shd.isShdAuto()) ||
        (refPap->shd.isShdNil() != pap.shd.isShdNil()))
    {
        color = Conversion::shdToColorStr(pap.shd, currentBgColor(), QString());
    }
    //Check the background-color of the named paragraph style.
    else {
        const KoGenStyle *pStyle = m_mainStyles->style(Conversion::styleName2QString(m_paragraphStyle->name()), m_paragraphStyle->type() == sgcPara ? "paragraph" : "text");
        if (pStyle) {
            color = pStyle->property("fo:background-color", KoGenStyle::ParagraphType);
            if (color.isEmpty() || color == "transparent") {
                color = pStyle->property("fo:background-color", KoGenStyle::TextType);
            }
            if (color == "transparent") {
                color.clear();
            }
        }
    }

    if (!color.isEmpty()) {
        pushBgColor(color);
    }
}

void Paragraph::popBgColor(void)
{
    if (m_bgColors.isEmpty()) {
        warnMsDoc << "Warning: m_bgColors stack already empty!";
    } else {
        m_bgColors.pop();
    }
}

void Paragraph::updateBgColor(const QString& val)
{
    if (!m_bgColors.isEmpty()) {
        m_bgColors.pop();
    }
    m_bgColors.push(val);
}

void Paragraph::addRunOfText(QString text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                             const QString &fontName, const wvWare::StyleSheet& styles,
                             bool addCompleteElement)
{
    // Check for column break in this text string
    int colBreak = text.indexOf(QChar(0xE));

    //I think this break always comes at the beginning of the text string
    if (colBreak == 0) {
        // Add needed attribute to paragraph style.
        //
        // NOTE: This logic breaks down if this isn't the first string in the
        //       paragraph, or there are other strings with another colBreak
        //       later in the same paragraph.
        m_odfParagraphStyle->addProperty("fo:break-before", "column", KoGenStyle::ParagraphType);
        // Remove character that signaled a column break
        text.remove(QChar(0xE));
    }

    // if it's inner paragraph, push back true this is an
    // m_textStyles.push_back(nullptr) complement if we still need the style
    // applied
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
    if (!msTextStyle && styles.size()) {
        msTextStyle = styles.styleByID(stiNormalChar);
        debugMsDoc << "Invalid reference to text style, reusing NormalChar";
    }
    Q_ASSERT(msTextStyle);

    QString msTextStyleName = Conversion::styleName2QString(msTextStyle->name());
    debugMsDoc << "text based on characterstyle " << msTextStyleName;

    KoGenStyle *textStyle = 0;

    bool suppresFontSize = false;
    if (m_textStyles.size() == 0 && m_paragraphProperties->pap().dcs.lines > 1) {
        suppresFontSize = true;
    }

    // Apply any extra properties to the auto style.  Those extra properties
    // are the diff beteween the referenceChp and the summed chp.  ReferenceCHP
    // can be either from the paragraph style or the character style.
    if (msTextStyle->sti() != stiNormalChar) {
        // this is not the default character style
        textStyle = new KoGenStyle(KoGenStyle::TextAutoStyle, "text");
        if (m_inStylesDotXml) {
            textStyle->setAutoStyleInStylesDotXml(true);
        }
        textStyle->setParentName(msTextStyleName);
        const wvWare::Word97::CHP* refChp = &msTextStyle->chp();

        //if we have a new font, process that
        if (!refChp || refChp->ftcAscii != chp->ftcAscii) {
            if (!fontName.isEmpty()) {
                textStyle->addProperty(QString("style:font-name"), fontName, KoGenStyle::TextType);
            }
            applyCharacterProperties(chp, textStyle, msTextStyle, suppresFontSize, m_combinedCharacters);
        }
    } else {
        // Default Paragraph Font, which is handled differently
        // Meaning we should really compare against the CHP of the paragraph
        textStyle = new KoGenStyle(KoGenStyle::TextAutoStyle, "text");
        if (m_inStylesDotXml) {
            textStyle->setAutoStyleInStylesDotXml(true);
        }
        //if we have a new font, process that
        const wvWare::Word97::CHP* refChp = &m_paragraphStyle->chp();
        if (!refChp || refChp->ftcAscii != chp->ftcAscii) {
            if (!fontName.isEmpty()) {
                textStyle->addProperty(QString("style:font-name"), fontName, KoGenStyle::TextType);
            }
        }
        applyCharacterProperties(chp, textStyle, m_paragraphStyle, suppresFontSize, m_combinedCharacters);
    }

    //add text style to list
    m_textStyles.push_back(textStyle);
}

QString Paragraph::writeToFile(KoXmlWriter* writer, bool openTextBox, QChar* tabLeader)
{
    debugMsDoc;

    //TODO: The paragraph-properties might have to be set before
    //text-properties to have proper automatic colors.

    //[MS-DOC] PAP -> [ODF] paragraph-properties
    applyParagraphProperties(*m_paragraphProperties, m_odfParagraphStyle, m_paragraphStyle,
                             m_inHeaderFooter && m_containsPageNumberField, this, tabLeader);

    // text-properties are required for empty paragraphs or paragraphs
    // containing only floating/inline objects.  If there is any text chunk,
    // then the text-properties are included in the style of type TextStyle.
    bool textPropsRequired = false;
    for (int i = 0; i < m_textStyles.size(); i++) {
        if (!m_textStyles[i]) {
            textPropsRequired = true;
            break;
        }
    }
    // [MS-DOC] CHP -> [ODF] text-properties
    if (m_textStrings.isEmpty() || textPropsRequired) {
        if (m_characterProperties) {
            applyCharacterProperties(m_characterProperties, m_odfParagraphStyle, m_paragraphStyle);
        } else {
            debugMsDoc << "Missing CHPs for an empty paragraph!";
        }
    }

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
        debugMsDoc << "returning with drop cap paragraph";
        if (m_textStrings.size()) {
            if (m_textStyles[0] != 0) {
                m_dropCapStyleName= m_textStyles[0]->parentName();
            }
        }
        return QString();
    }

    // If there is a dropcap defined, then write it to the style.
    if (m_dropCapStatus == HasDropCapIntegrated) {
        debugMsDoc << "Creating drop cap style";

        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        KoXmlWriter tmpWriter(&buf, 3);
        tmpWriter.startElement("style:drop-cap");
        tmpWriter.addAttribute("style:lines", m_dcs_lines);
        tmpWriter.addAttributePt("style:distance", m_dropCapDistance);
        // We have only support for fdct=1, i.e. regular dropcaps.
        // There is no support for fdct=2, i.e. dropcaps in the margin (ODF doesn't support this).
        tmpWriter.addAttribute("style:length", m_dcs_fdct > 0 ? 1 : 0);
        if (!m_dropCapStyleName.isEmpty()) {
            tmpWriter.addAttribute("style:style-name", m_dropCapStyleName.toUtf8());
        }
        tmpWriter.endElement(); //style:drop-cap
        buf.close();

        QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        m_odfParagraphStyle->addChildElement("style:drop-cap", contents);

        m_dropCapStatus = NoDropCap;
    }

    QString textStyleName;
    //add paragraph style to the collection and its name to the content
    debugMsDoc << "adding paragraphStyle";
    //add the attribute for our style in <text:p>
    textStyleName = "P";
    textStyleName = m_mainStyles->insert(*m_odfParagraphStyle, textStyleName);

    //check if the paragraph is inside of an absolutely positioned frame
    if (openTextBox) {
        KoGenStyle gs(KoGenStyle::GraphicAutoStyle, "graphic");
        const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
        QString drawStyleName;

        writer->startElement("text:p", false);
        writer->addAttribute("text:style-name", textStyleName.toUtf8());

        const wvWare::Word97::PAP& pap = m_paragraphProperties->pap();
        int dxaAbs = 0;
        int dyaAbs = 0;

        // horizontal position of the anchor
        QString pos = Conversion::getHorizontalPos(pap.dxaAbs);
        gs.addProperty("style:horizontal-pos", pos, gt);
        if (pos == "from-left") {
            dxaAbs = pap.dxaAbs;
        }
        // vertical position of the anchor
        pos = Conversion::getVerticalPos(pap.dyaAbs);
        gs.addProperty("style:vertical-pos", pos, gt);
        if (pos == "from-top") {
            dyaAbs = pap.dyaAbs;
        }
        // relative vertical position of the anchor
        QString anchor = Conversion::getVerticalRel(pap.pcVert);
        if (!anchor.isEmpty()) {
            gs.addProperty("style:vertical-rel", anchor, gt);
        }
        // relative horizontal position of the anchor
        anchor = Conversion::getHorizontalRel(pap.pcHorz);
        if (!anchor.isEmpty()) {
            gs.addProperty("style:horizontal-rel", anchor, gt);
        }

        if (pap.dxaWidth == 0) {
            gs.addProperty("draw:auto-grow-width", "true");
        }
        if (pap.dyaHeight == 0) {
            gs.addProperty("draw:auto-grow-height", "true");
        }

        // as fas as can be determined
        gs.addProperty("style:flow-with-text", "false");

        // In case a header/footer is processed, save the style into styles.xml
        if (m_inStylesDotXml) {
            gs.setAutoStyleInStylesDotXml(true);
        }

        //TODO: improve frame borders support
        if ( pap.brcLeft.brcType || pap.brcTop.brcType ||
             pap.brcRight.brcType || pap.brcBottom.brcType )
        {
            debugMsDoc << "Frame bordes not fully supported!";
        }
        gs.addProperty("draw:stroke", getStrokeValue(pap.brcLeft.brcType), gt);

        drawStyleName = "fr";
        drawStyleName = m_mainStyles->insert(gs, drawStyleName);
        writer->startElement("draw:frame");
        writer->addAttribute("draw:style-name", drawStyleName.toUtf8());
        writer->addAttribute("text:anchor-type", "paragraph");

        if (pap.dxaWidth != 0) {
            writer->addAttributePt("svg:width", (double)pap.dxaWidth / 20);
        }
        if (pap.dyaHeight != 0) {
            writer->addAttributePt("svg:height", (double)pap.dyaHeight / 20);
        }
        writer->addAttributePt("svg:x", (double)dxaAbs / 20);
        writer->addAttributePt("svg:y", (double)dyaAbs / 20);
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

    //TODO: insert <text:tab> elements at specified locations

    //if there's any paragraph content
    if (!m_textStrings.isEmpty()) {
        //Loop through each text strings and styles (equal # of both) and write
        //them to the file.
        debugMsDoc << "writing text spans now";
        QString oldStyleName;
        bool startedSpan = false;
        for (int i = 0; i < m_textStrings.size(); i++) {
            if (m_textStyles[i] == 0) {
                if (startedSpan) {
                    writer->endElement(); //text:span
                    startedSpan = false;
                    oldStyleName = " ";
                }
                //if style is null, we have an inner paragraph and add the
                //complete paragraph element to writer need to get const char*
                //from the QString
                debugMsDoc << "complete element: " <<
                                 m_textStrings[i].toLocal8Bit().constData();
                writer->addCompleteElement(m_textStrings[i].toUtf8().constData());
            } else {
                //put style into m_mainStyles & get its name
                textStyleName = 'T';
                textStyleName = m_mainStyles->insert(*m_textStyles[i], textStyleName);
                //debugMsDoc << m_textStyles[i]->type();

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
                //Write text string to writer.  Now I just need to write the
                //text:span to the header tag.
                debugMsDoc << "Writing \"" << m_textStrings[i] << "\"";
                if (m_addCompleteElement[i] == false) {
                    writer->addTextSpan(m_textStrings[i]);
                }
                //special case we need style applied and complete element added
                else {
                    writer->addCompleteElement(m_textStrings[i].toUtf8().constData());
                }
                //cleanup
                delete m_textStyles[i];
                m_textStyles[i] = 0;
            }
        }
        // If we have an unfinished text:span, finish it now.
        if (startedSpan) {
            writer->endElement(); //text:span
            startedSpan = false;
        }
    } //if (!m_textStrings.isEmpty())

    //close the <text:p> or <text:h> tag we opened
    writer->endElement();

    return textStyleName;
}

// open/closeInnerParagraph: enables us to process a paragraph inside
// another paragraph, as in the case of footnotes.  openInnerParagraph
// should be called from texthandler.paragraphStart, and
// closeInnerParagraph should be called from paragraphEnd, but only
// after calling writeToFile to write the content to another xml
// writer (eg. m_footnoteWriter).
void Paragraph::openInnerParagraph()
{
    debugMsDoc;

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
    debugMsDoc;

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

void Paragraph::applyParagraphProperties(const wvWare::ParagraphProperties& properties,
                                         KoGenStyle* style, const wvWare::Style* parentStyle,
                                         bool setDefaultAlign, Paragraph *paragraph,
                                         QChar* tabLeader, const QString& bgColor)
{
    debugMsDoc;

    const wvWare::Word97::PAP* refPap;
    if (parentStyle) {
        refPap = &parentStyle->paragraphProperties().pap();
    } else {
        refPap = 0;
    }

    if (!bgColor.isNull()) {
        updateBgColor(bgColor);
    }

    //pap is our paragraph properties object
    const wvWare::Word97::PAP& pap = properties.pap();

    const KoGenStyle::PropertyType pt = KoGenStyle::ParagraphType;

    //paragraph alignment
    //jc = justification code
    if (!refPap || refPap->jc != pap.jc) {
        if (pap.jc == 1)   //1 = center justify
            style->addProperty("fo:text-align", "center", pt);
        else if (pap.jc == 2)  //2 = right justify
            style->addProperty("fo:text-align", "end", pt);
        else if (pap.jc == 3)  //3 = left & right justify
            style->addProperty("fo:text-align", "justify", pt);
        else if (pap.jc == 4)  //4 = distributed .. fake it as justify
            style->addProperty("fo:text-align", "justify", pt);
        else //0 = left justify
            style->addProperty("fo:text-align", "start", pt);
    } else if (setDefaultAlign) {
        // Set default align for page number field in header or footer
        debugMsDoc << "setting default align for page number field in header or footer";
        style->addProperty("fo:text-align", "center", pt);
    }

    if (!refPap || refPap->fBiDi != pap.fBiDi) {
        if (pap.fBiDi == 1)   //1 = right to left
            style->addProperty("style:writing-mode", "rl-tb", pt);
        else //0 = normal
            style->addProperty("style:writing-mode", "lr-tb", pt);
    }

    // If there is no parent style OR the parent and child background
    // color don't match.
    if ( !refPap ||
         (refPap->shd.cvBack != pap.shd.cvBack) ||
         (refPap->shd.isShdAuto() != pap.shd.isShdAuto()) ||
         (refPap->shd.isShdNil() != pap.shd.isShdNil()) )
    {
        QString color = Conversion::shdToColorStr(pap.shd, currentBgColor(), QString());
        if (!color.isNull()) {
            updateBgColor(color);
        } else {
            color = "transparent";
        }
        style->addProperty("fo:background-color", color, pt);
    }

    //dxaLeft1 = first-line indent from left margin (signed, relative to dxaLeft)
    //dxaLeft = indent from left margin (signed)
    //dxaRight = indent from right margin (signed)
    if (!refPap || refPap->dxaLeft != pap.dxaLeft) {
        // apply twip -> pt conversion, only if not in a list
        if (pap.ilfo == 0) {
            style->addPropertyPt("fo:margin-left", (double)pap.dxaLeft / 20.0, pt);
        }
    }
    if (!refPap || refPap->dxaRight != pap.dxaRight) {
        // apply twip -> pt conversion
        style->addPropertyPt("fo:margin-right", (double)pap.dxaRight / 20.0, pt);
    }
    if (!refPap || refPap->dxaLeft1 != pap.dxaLeft1) {
        // apply twip -> pt conversion, only if not in a list
        if (pap.ilfo == 0) {
            style->addPropertyPt("fo:text-indent", (double)pap.dxaLeft1 / 20.0, pt);
        }
    }

    //dyaBefore = vertical spacing before paragraph (unsigned)
    //dyaAfter = vertical spacing after paragraph (unsigned)
    if (!refPap || refPap->dyaBefore != pap.dyaBefore) {
        double marginTop = (double)pap.dyaBefore / 20.0;
        if (pap.dyaBeforeAuto) {
           //TODO: Figure out the proper logic for automatic margins.
            marginTop = 14;
        }
        style->addPropertyPt("fo:margin-top", marginTop, pt);
    }
    if (!refPap || refPap->dyaAfter != pap.dyaAfter) {
        double marginBottom = (double)pap.dyaAfter / 20.0;
        if (pap.dyaAfterAuto) {
           //TODO: Figure out the proper logic for automatic margins.
            marginBottom = 14;
        }
        style->addPropertyPt("fo:margin-bottom", marginBottom, pt);
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
            QString proportionalLineSpacing(QString::number(ceil(pap.lspd.dyaLine / 2.4f)));
            style->addProperty("fo:line-height", proportionalLineSpacing.append("%"), pt);
        } else if (pap.lspd.fMultLinespace == 0) {
            // Magnitude of lspd.dyaLine specifies the amount of space
            // that will be provided for lines in the paragraph in twips.
            //
            // See sprmPDyaLine in generator_wword8.htm
            qreal value = qAbs((qreal)pap.lspd.dyaLine / (qreal)20.0); // twip -> pt
            // lspd.dyaLine > 0 means "at least", < 0 means "exactly"
            if (pap.lspd.dyaLine > 0)
                style->addPropertyPt("style:line-height-at-least", value, pt);
            else if (pap.lspd.dyaLine < 0 && pap.dcs.fdct==0)
                style->addPropertyPt("fo:line-height", value, pt);
        } else
            warnMsDoc << "Unhandled LSPD::fMultLinespace value: "
            << pap.lspd.fMultLinespace;
    }

    //fKeep = keep entire paragraph on one page if possible
    //fKeepFollow = keep paragraph on same page with next paragraph if possible
    //fPageBreakBefore = start this paragraph on new page
    if (!refPap || refPap->fKeep != pap.fKeep) {
        if (pap.fKeep)
            style->addProperty("fo:keep-together", "always", pt);
        else
            style->addProperty("fo:keep-together", "auto", pt);
    }
    if (!refPap || refPap->fKeepFollow != pap.fKeepFollow) {
        if (pap.fKeepFollow)
            style->addProperty("fo:keep-with-next", "always", pt);
        else
            style->addProperty("fo:keep-with-next", "auto", pt);
    }
    if (!refPap || refPap->fPageBreakBefore != pap.fPageBreakBefore) {
        if (pap.fPageBreakBefore)
            style->addProperty("fo:break-before", "page", pt);
        else
            style->addProperty("fo:break-before", "auto", pt);
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
        style->addProperty("fo:border-top", Conversion::setBorderAttributes(pap.brcTop), pt);
    }
    if (!refPap || refPap->brcBottom.brcType != pap.brcBottom.brcType) {
        style->addProperty("fo:border-bottom", Conversion::setBorderAttributes(pap.brcBottom), pt);
    }
    if (!refPap || refPap->brcLeft.brcType != pap.brcLeft.brcType) {
        style->addProperty("fo:border-left", Conversion::setBorderAttributes(pap.brcLeft), pt);
    }
    if (!refPap || refPap->brcRight.brcType != pap.brcRight.brcType) {
        style->addProperty("fo:border-right", Conversion::setBorderAttributes(pap.brcRight), pt);
    }

    // Padding
    if (!refPap || refPap->brcTop.dptSpace != pap.brcTop.dptSpace) {
        style->addPropertyPt("fo:padding-top", pap.brcTop.dptSpace, pt);
    }
    if (!refPap || refPap->brcBottom.dptSpace != pap.brcBottom.dptSpace) {
        style->addPropertyPt("fo:padding-bottom", pap.brcBottom.dptSpace, pt);
    }
    if (!refPap || refPap->brcLeft.dptSpace != pap.brcLeft.dptSpace) {
        style->addPropertyPt("fo:padding-left", pap.brcLeft.dptSpace, pt);
    }
    if (!refPap || refPap->brcRight.dptSpace != pap.brcRight.dptSpace) {
        style->addPropertyPt("fo:padding-right", pap.brcRight.dptSpace, pt);
    }

    // Drop Cap Style (DCS)
    if (!refPap || refPap->dcs.fdct != pap.dcs.fdct || refPap->dcs.lines != pap.dcs.lines) {
        if (paragraph) {
            debugMsDoc << "Processing drop cap";
            if (paragraph->m_textStrings.size() > 0)
                debugMsDoc << "String = """ << paragraph->m_textStrings[0] << """";
            else
                debugMsDoc << "No drop cap string";

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
    //TODO: Compare with the parent style to avoid duplicity.

    // Tabulators, only if not in a list.  itbdMac = number of tabs stops
    // defined for paragraph.  Must be in <0,64>.
    if (pap.itbdMac) {

        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        KoXmlWriter tmpWriter(&buf, 3);//root, office:automatic-styles, style:style
        tmpWriter.startElement("style:tab-stops");
        for (int i = 0 ; i < pap.itbdMac ; ++i) {
            tmpWriter.startElement("style:tab-stop");

            //rgdxaTab = array of { positions of itbdMac tab stops ; itbdMac
            //tab descriptors } itbdMax == 64.
            const wvWare::Word97::TabDescriptor &td = pap.rgdxaTab[i];
            //td.dxaTab = position in twips
            //QString pos( QString::number( (double)td.dxaTab / 20.0 ) );
            tmpWriter.addAttributePt("style:position", (double)td.dxaTab / 20.0);

            //td.tbd.jc = justification code
            switch (td.tbd.jc) {
            case jcCenter:
                tmpWriter.addAttribute("style:type", "center");
                break;
            case jcRight:
                tmpWriter.addAttribute("style:type", "right");
                break;
            case jcDecimal:
                tmpWriter.addAttribute("style:type", "char");
                tmpWriter.addAttribute("style:char", ".");
                break;
            case jcBar:
                //bar -> just creates a vertical bar at that point that's always visible
                warnMsDoc << "Unhandled tab justification code: " << td.tbd.jc;
                break;
            default:
                //ODF: The default value for this attribute is left.
                break;
            }
            //td.tbd.tlc = tab leader code, default no leader (can be ignored)
            QChar leader;
            switch (td.tbd.tlc) {
            case tlcDot:
            case tlcMiddleDot:
                leader = QChar('.');
                break;
            case tlcHyphen:
                leader = QChar('-');
                break;
            case tlcUnderscore:
            case tlcHeavy:
                leader = QChar('_');
                break;
            case tlcNone:
            default:
                //ODF: The default value for this attribute is “ “ (U+0020, SPACE).
                break;
            }
            //The value MUST be ignored if jc is equal jcBar.
            if (td.tbd.jc != jcBar && leader.unicode() > 0) {
                tmpWriter.addAttribute("style:leader-text", leader);
            }
            tmpWriter.endElement();//style:tab-stop

            if (tabLeader) {
                *tabLeader = leader;
            }
        }
        tmpWriter.endElement();//style:tab-stops
        buf.close();
        QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        //now write the tab info to the paragraph style
        style->addChildElement("style:tab-stops", contents);
    }
} //end applyParagraphProperties

void Paragraph::applyCharacterProperties(const wvWare::Word97::CHP* chp, KoGenStyle* style, const wvWare::Style* parentStyle, bool suppressFontSize, bool combineCharacters, const QString& bgColor)
{
    //TODO: Also compare against the CHPs of the paragraph style.  At the
    //moment comparing against CHPs of the referred built-in character style.

    const KoGenStyle::PropertyType tt = KoGenStyle::TextType;

    //if we have a named style, set its CHP as the refChp
    const wvWare::Word97::CHP* refChp;
    if (parentStyle) {
        refChp = &parentStyle->chp();
    } else {
        refChp = 0;
    }

    //initialize the colors
    if (!bgColor.isNull()) {
        updateBgColor(bgColor);
    }

    const quint8 bgColorsSize_bkp = m_bgColors.size();

    //fHighlight = when 1, characters are highlighted with color specified by
    //chp.icoHighlight icoHighlight = highlight color (see chp.ico)
    if (!refChp ||
        (refChp->fHighlight != chp->fHighlight) ||
        (refChp->icoHighlight != chp->icoHighlight))
    {
        QString color("transparent");
        if (chp->fHighlight) {
            color = Conversion::color(chp->icoHighlight, -1);
            pushBgColor(color);
        }
        style->addProperty("fo:background-color", color, tt);
    }

    if (!refChp ||
        (refChp->shd.cvBack != chp->shd.cvBack) ||
        (refChp->shd.isShdAuto() != chp->shd.isShdAuto()) ||
        (refChp->shd.isShdNil() != chp->shd.isShdNil()))
    {
        QString color = Conversion::shdToColorStr(chp->shd, currentBgColor(), QString());
        if (!color.isNull()) {
            pushBgColor(color);
        } else {
            color = "transparent";
        }
        style->addProperty("fo:background-color", color, tt);
    }

    //TODO: Check fo:background-color of the named character style

    //ico = color of text, but this has been replaced by cv
    if (!refChp ||
        (refChp->cv != chp->cv) ||
        (chp->cv == wvWare::Word97::cvAuto))
    {
        QString color;
        if (chp->cv == wvWare::Word97::cvAuto) {
            //use the color context to set the proper font color
            color = Conversion::computeAutoColor(chp->shd, currentBgColor(), QString());

            // NOTE: Have to specify fo:color explicitly because only
            // basic support of style:use-window-font-color is
            // implemented in calligra.  No support for text in shapes
            // with solid fill.
            //
            // style->addProperty("style:use-window-font-color", "true", tt);
        } else {
            color = QString('#' + QString::number(chp->cv | 0xff000000, 16).right(6).toUpper());
        }
        style->addProperty("fo:color", color, tt);
        // m_fontColor = color;
    }

    //hps = font size in half points
    if (!suppressFontSize && (!refChp || refChp->hps != chp->hps)) {
        style->addPropertyPt(QString("fo:font-size"), ((qreal) chp->hps / 2), tt);
    }

    //fBold = bold text if 1
    if (!refChp || (refChp->fBold != chp->fBold)) {
        style->addProperty(QString("fo:font-weight"), chp->fBold ? QString("bold") : QString("normal"), tt);
    }

    //fItalic = italic text if 1
    if (!refChp || refChp->fItalic != chp->fItalic)
        style->addProperty(QString("fo:font-style"), chp->fItalic ? QString("italic") : QString("normal"), tt);

    // ********************
    // style of underline
    // ********************
    if (!refChp || refChp->kul != chp->kul) {
        // style:text-underline-color
        QString color("font-color");
        if (chp->cvUl != wvWare::Word97::cvAuto) {
            color = QString('#' + QString::number(chp->cvUl | 0xff000000, 16).right(6).toUpper());
        }
        style->addProperty("style:text-underline-color", color, tt);
        // style:text-underline-mode
        style->addProperty("style:text-underline-mode", getTextUnderlineMode(chp->kul), tt);
        // style:text-underline-style
        QString ustyle(getTextUnderlineStyle(chp->kul));
        if (!ustyle.isEmpty()) {
            style->addProperty("style:text-underline-style", ustyle, tt);
        }
        // style:text-underline-type
        style->addProperty("style:text-underline-type", getTextUnderlineType(chp->kul), tt);
        // style:text-underline-width
        style->addProperty("style:text-underline-width", getTextUnderlineWidth(chp->kul), tt);
    }
    //fstrike = use strikethrough if 1
    //fDStrike = use double strikethrough if 1
    if (!refChp || refChp->fStrike != chp->fStrike || refChp->fDStrike != chp->fDStrike) {
        if (chp->fStrike)
            style->addProperty("style:text-line-through-type", "single", tt);
        else if (chp->fDStrike)
            style->addProperty("style:text-line-through-type", "double", tt);
        else
            style->addProperty("style:text-line-through-type", "none", tt);
    }

    //font attribute (uppercase, lowercase (not in MSWord), small caps)
    //fCaps = displayed with all caps when 1, no caps when 0
    //fSmallCaps = displayed with small caps when 1, no small caps when 0
    if (!refChp || refChp->fCaps != chp->fCaps || refChp->fSmallCaps != chp->fSmallCaps) {
        if (chp->fCaps)
            style->addProperty("fo:text-transform", "uppercase", tt);
        if (chp->fSmallCaps)
            style->addProperty("fo:font-variant", "small-caps", tt);
    }

    //iss = superscript/subscript indices
    if (!refChp || refChp->iss != chp->iss) {
        if (chp->iss == 1)   //superscript
            style->addProperty("style:text-position", "super", tt);
        else if (chp->iss == 2)   //subscript
            style->addProperty("style:text-position", "sub", tt);
        else   //no superscript or subscript
            style->addProperty("style:text-position", "0% 100%", tt);
    }

    //fShadow = text has shadow if 1
    //fImprint = text engraved if 1
    if (!refChp || refChp->fShadow != chp->fShadow || refChp->fImprint != chp->fImprint) {
        if (chp->fShadow)
            style->addProperty("fo:text-shadow", "1pt", tt);
        if (chp->fImprint)
            style->addProperty("style:font-relief", "engraved", tt);
    }

    //fOutline = text is outline if 1
    if (!refChp || refChp->fOutline != chp->fOutline) {
        if (chp->fOutline)
            style->addProperty("style:text-outline", "true", tt);
        else
            style->addProperty("style:text-outline", "false", tt);
    }

    // if the characters are combined, add proper style
    if (combineCharacters) {
        style->addProperty("style:text-combine", "letters", tt);
    }

    //dxaSpace = letterspacing in twips
    if (!refChp || refChp->dxaSpace != chp->dxaSpace) {
        double value =  chp->dxaSpace / 20.0; // twips -> pt
        style->addPropertyPt("fo:letter-spacing", value, tt);
    }
    //pctCharwidth = pct stretch doesn't seem to have an ODF ccounterpart but Qt could support it

    //fTNY = 1 when text is vertical
    if (!refChp || refChp->fTNY != chp->fTNY) {
        if (chp->fTNY) {
            style->addProperty("style:text-rotation-angle", 90, tt);
            if (chp->fTNYCompress) {
                style->addProperty("style:text-rotation-scale", "fixed", tt);
            } else {
                style->addProperty("style:text-rotation-scale", "line-height", tt);
            }
        }
    }

    //wCharScale - MUST be greater than or equal to 1 and less than or equal to 600
    if (!refChp || refChp->wCharScale != chp->wCharScale) {
        if (chp->wCharScale) {
            style->addProperty("style:text-scale", QString::number(chp->wCharScale) + "%", tt);
        }
    }

    //remove the background-colors collected for this text chunk
    while (m_bgColors.size() > bgColorsSize_bkp) {
        popBgColor();
    }
}

Paragraph::DropCapStatus Paragraph::dropCapStatus() const
{
    return m_dropCapStatus;
}

void Paragraph::getDropCapData(QString *string, int *type, int *lines, qreal *distance, QString *style) const
{
    // As far as I can see there is only ever a single character as drop cap.
    *string = m_textStrings.isEmpty() ? QString() : m_textStrings[0];
    *type = m_dcs_fdct;
    *lines = m_dcs_lines;
    *distance = m_dropCapDistance;
    *style = m_dropCapStyleName;
}


void Paragraph::addDropCap(QString &string, int type, int lines, qreal distance, const QString &style)
{
    debugMsDoc << "combining drop cap paragraph: " << string;
    if (m_dropCapStatus == IsDropCapPara)
        debugMsDoc << "This paragraph already has a dropcap set!";

    m_dropCapStatus = HasDropCapIntegrated;

    // Get the drop cap data.
    m_dcs_fdct        = type;
    m_dcs_lines       = lines;
    m_dropCapDistance = distance;
    m_dropCapStyleName = style;

    // Add the actual text.
    // Here we assume that there will only be one text snippet for the drop cap.
#if 1

#if 1
    debugMsDoc << "size: " << m_textStrings.size();
    if (m_textStrings.isEmpty()) {
        m_textStrings.append(string);
        KoGenStyle* style = 0;
        m_textStyles.insert(m_textStyles.begin(), style);
    } else {
        m_textStrings[0].prepend(string);
    }
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

QString Paragraph::createTextStyle(wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const wvWare::StyleSheet& styles)
{
    if (!chp) {
        return QString();
    }
    const wvWare::Style* msTextStyle = styles.styleByIndex(chp->istd);
    if (!msTextStyle && styles.size()) {
        msTextStyle = styles.styleByID(stiNormalChar);
        debugMsDoc << "Invalid reference to text style, reusing NormalChar";
    }
    Q_ASSERT(msTextStyle);

    QString msTextStyleName = Conversion::styleName2QString(msTextStyle->name());
    debugMsDoc << "text based on characterstyle " << msTextStyleName;

    bool suppresFontSize = false;
    if (m_paragraphProperties->pap().dcs.lines > 1) {
        suppresFontSize = true;
    }
    KoGenStyle textStyle(KoGenStyle::TextAutoStyle, "text");
    if (m_inStylesDotXml) {
        textStyle.setAutoStyleInStylesDotXml(true);
    }

    applyCharacterProperties(chp, &textStyle, msTextStyle, suppresFontSize, m_combinedCharacters);

    QString textStyleName('T');
    textStyleName = m_mainStyles->insert(textStyle, textStyleName);
    return textStyleName;
}

const char* getStrokeValue(const uint brcType)
{
    //TODO: create corresponding dash styles
    switch (brcType) {
    case 0x01: //A single line.
    case 0x03: //A double line.
    case 0x05: //A thin single solid line.
    case 0x14: //A single wavy line.
    case 0x15: //A double wavy line.
    case 0x18: //threeDEmboss
    case 0x19: //threeDEngrave
    case 0x1A: //outset
    case 0x1B: //inset
    return "solid";
    default:
        return "none";
    }
}

const char* getTextUnderlineMode(const uint kul)
{
    switch (kul) {
    case kulWords:
        return "skip-white-space";
    default:
        return "continuous";
    }
}

const char* getTextUnderlineStyle(const uint kul)
{
    // The values are none, solid, dotted, dash, long-dash, dot-dash,
    // dot-dot-dash or wave.  The defined value for the
    // style:text-underline-style attribute is none: text has no underlining.
    switch (kul) {
    case kulSingle:
    case kulWords:
    case kulDouble:
        return "solid";
    case kulDotted:
    case kulDottedHeavy:
        return "dotted";
    case kulThick:
    return "solid";
    case kulDash:
    case kulDashHeavy:
        return "dash";
    case kulDashLong:
    case kulDashLongHeavy:
        return "long-dash";
    case kulDotDash:
    case kulDotDashHeavy:
        return "dot-dash";
    case kulDotDotDash:
    case kulDotDotDashHeavy:
        return "dot-dot-dash";
    case kulWavy:
    case kulWavyDouble:
    case kulWavyHeavy:
        return "wave";
    case 5: // hidden - This makes no sense as an underline property!
    case 8:
        //NOTE: Styles of underline not specified in [MS-DOC] - v20101219
        debugMsDoc << "Unknown style of underline detected!";
        return "";
    case kulNone:
    default:
        return "";
    };
}

const char* getTextUnderlineType(const uint kul)
{
    //The values are none, single or double.
    switch (kul) {
    case kulNone:
        return "none";
    case kulDouble:
    case kulWavyDouble:
        return "double";
    default:
        return "single";
    }
}

const char* getTextUnderlineWidth(const uint kul)
{
    // The values are auto, normal, bold, thin, medium, thick, a value of type
    // positiveInteger, a value of type percent or a value of type
    // positiveLength.
    switch (kul) {
    case kulThick:
        return "thick";
    case kulDottedHeavy:
    case kulDashHeavy:
    case kulDashLongHeavy:
    case kulDotDashHeavy:
    case kulDotDotDashHeavy:
    case kulWavyHeavy:
        return "bold";
    default:
        return "auto";
    }
}
