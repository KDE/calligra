/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef MSOOXMLCOMMONREADER_IMPL_H
#define MSOOXMLCOMMONREADER_IMPL_H

void MSOOXML_CURRENT_CLASS::initInternal()
{
    m_insideHdr = false;
    m_insideFtr = false;
}

#undef CURRENT_EL
#define CURRENT_EL t
//! t handler (Text)
/*! ECMA-376, 17.3.3.31, p.389.
 Parent elements:
 - r (§22.1.2.87) - Shared ML
 - [done] r (§17.3.2.25)
 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_t()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isCharacters()) {
            body->addTextSpan(text().toString());
        }
//! @todo add ELSE_WRONG_FORMAT
        BREAK_IF_END_OF(CURRENT_EL);
    }
//kDebug() << "{1}";
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL r
//! r handler (Text Run)
/*! ECMA-376, 17.3.2.25, p.320.

 Parent elements:
 - bdo (§17.3.2.3)
 - customXml (§17.5.1.3)
 - del (§17.13.5.14)
 - dir (§17.3.2.8)
 - fldSimple (§17.16.19)
 - hyperlink (§17.16.22)
 - ins (§17.13.5.18)
 - moveFrom (§17.13.5.22)
 - moveTo (§17.13.5.25)
 - [done] p (§17.3.1.22)
 - rt (§17.3.3.24)
 - rubyBase (§17.3.3.27)
 - sdtContent (§17.5.2.36)
 - smartTag (§17.5.1.9)
 Child elements:
 - annotationRef (Comment Information Block) §17.13.4.1
 - br (Break) §17.3.3.1
 - commentReference (Comment Content Reference Mark) §17.13.4.5
 - contentPart (Content Part) §17.3.3.2
 - continuationSeparator (Continuation Separator Mark) §17.11.1
 - cr (Carriage Return) §17.3.3.4
 - dayLong (Date Block - Long Day Format) §17.3.3.5
 - dayShort (Date Block - Short Day Format) §17.3.3.6
 - delInstrText (Deleted Field Code) §17.16.13
 - delText (Deleted Text) §17.3.3.7
 - [done] drawing (DrawingML Object) §17.3.3.9
 - endnoteRef (Endnote Reference Mark) §17.11.6
 - endnoteReference (Endnote Reference) §17.11.7
 - fldChar (Complex Field Character) §17.16.18
 - footnoteRef (Footnote Reference Mark) §17.11.13
 - footnoteReference (Footnote Reference) §17.11.14
 - instrText (Field Code) §17.16.23
 - lastRenderedPageBreak (Position of Last Calculated Page Break) §17.3.3.13
 - monthLong (Date Block - Long Month Format) §17.3.3.15
 - monthShort (Date Block - Short Month Format) §17.3.3.16
 - noBreakHyphen (Non Breaking Hyphen Character) §17.3.3.18
 - object (Embedded Object) §17.3.3.19
 - pgNum (Page Number Block) §17.3.3.22
 - ptab (Absolute Position Tab Character) §17.3.3.23
 - [done] rPr (Run Properties) §17.3.2.28
 - ruby (Phonetic Guide) §17.3.3.25
 - separator (Footnote/Endnote Separator Mark) §17.11.23
 - softHyphen (Optional Hyphen Character) §17.3.3.29
 - sym (Symbol Character) §17.3.3.30
 - [done] t (Text) §17.3.3.31
 - tab (Tab Character) §17.3.3.32
 - yearLong (Date Block - Long Year Format) §17.3.3.33
 - yearShort (Date Block - Short Year Format) §17.3.3.34
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_r()
{
    READ_PROLOGUE
    while (!atEnd()) {
//kDebug() <<"[0]";
        readNext();
        kDebug() << *this;
//kDebug() <<"[1]";
        if (isStartElement()) {
            TRY_READ_IF(rPr)
            ELSE_TRY_READ_IF(t)
            ELSE_TRY_READ_IF(drawing)
//            else { SKIP_EVERYTHING }
//! @todo add ELSE_WRONG_FORMAT
//kDebug() <<"[1.5]";
        }
//kDebug() <<"[2]";
        BREAK_IF_END_OF(CURRENT_EL);
//kDebug() <<"[2.5]";
    }
//kDebug() <<"[3]";
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lang
//! lang handler (Languages for Run Content)
/*! ECMA-376, 17.3.2.20, p.314.
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lang()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
//! @todo add ELSE_WRONG_FORMAT
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPr
//! [1] rPr handler (Run Properties for the Paragraph Mark) WML ECMA-376, 17.3.1.29, p.263,
//!         This element specifies a set of run properties which shall be applied to the contents
//!         of the parent run after all style formatting has been applied to the text.

//! [2] rPr handler (Run Properties) WML ECMA-376, 17.3.2.28, p.331,
//!         This element specifies a set of run properties which shall be applied to the contents
//!         of the parent run after all style formatting has been applied to the text.

//! [3] rPr handler (Text Run Properties) DrawingML ECMA-376, 21.1.2.3.9, p.3624.
//!     This element contains all run level text properties for the text runs within a containing paragraph.
/*!
 Parent elements:
 - [1] pPr (§17.3.1.26)
 - [2] ctrlPr (§22.1.2.23)
 - [2] r (§22.1.2.87) - Shared ML
 - [2] r (§17.3.2.25)
 - [3] br (§21.1.2.2.1)
 - [3] fld (§21.1.2.2.4)
 - [3] r (§21.1.2.3.8)
 Child elements:
 - [done] b (Bold) §17.3.2.1
 - bCs (Complex Script Bold) §17.3.2.2
 - bdr (Text Border) §17.3.2.4
 - caps (Display All Characters As Capital Letters) §17.3.2.5
 - [done] color (Run Content Color) §17.3.2.6
 - cs (Use Complex Script Formatting on Run) §17.3.2.7
 - del (Deleted Paragraph) §17.13.5.15
 - [done] dstrike (Double Strikethrough) §17.3.2.9
 - eastAsianLayout (East Asian Typography Settings) §17.3.2.10
 - effect (Animated Text Effect) §17.3.2.11
 - em (Emphasis Mark) §17.3.2.12
 - emboss (Embossing) §17.3.2.13
 - fitText (Manual Run Width) §17.3.2.14
 - [done] highlight (Text Highlighting) §17.3.2.15
 - [done] i (Italics) §17.3.2.16
 - iCs (Complex Script Italics) §17.3.2.17
 - imprint (Imprinting) §17.3.2.18
 - ins (Inserted Paragraph) §17.13.5.20
 - kern (Font Kerning) §17.3.2.19
 - [done] lang (Languages for Run Content) §17.3.2.20
 - moveFrom (Move Source Paragraph) §17.13.5.21
 - moveTo (Move Destination Paragraph) §17.13.5.26
 - noProof (Do Not Check Spelling or Grammar) §17.3.2.21
 - oMath (Office Open XML Math) §17.3.2.22
 - outline (Display Character Outline) §17.3.2.23
 - position (Vertically Raised or Lowered Text) §17.3.2.24
 - rFonts (Run Fonts) §17.3.2.26
 - rPrChange (Revision Information for Run Properties on the Paragraph Mark) §17.13.5.30
 - rStyle (Referenced Character Style) §17.3.2.29
 - rtl (Right To Left Text) §17.3.2.30
 - shadow (Shadow) §17.3.2.31
 - shd (Run Shading) §17.3.2.32
 - smallCaps (Small Caps) §17.3.2.33
 - snapToGrid (Use Document Grid Settings For Inter-Character Spacing) §17.3.2.34
 - spacing (Character Spacing Adjustment) §17.3.2.35
 - specVanish (Paragraph Mark Is Always Hidden) §17.3.2.36
 - [done] strike (Single Strikethrough) §17.3.2.37
 - [done] sz (Non-Complex Script Font Size) §17.3.2.38
 - szCs (Complex Script Font Size) §17.3.2.39
 - [done] u (Underline) §17.3.2.40
 - vanish (Hidden Text) §17.3.2.41
 - vertAlign (Subscript/Superscript Text) §17.3.2.42
 - w (Expanded/Compressed Text) §17.3.2.43
 - webHidden (Web Hidden Text) §17.3.2.44
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_rPr()
{
    ReadMethod caller = m_calls.top();
    READ_PROLOGUE

    const QXmlStreamAttributes attrs( attributes() );

    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = new KoCharacterStyle();
    m_currentTextStyle = KoGenStyle(KoGenStyle::StyleTextAuto, "text");

    MSOOXML::Utils::XmlWriteBuffer textSpanBuf;
    if (CALLER_IS(r)) {
//kDebug() << "text:span...";
        body = textSpanBuf.setWriter(body);
    }
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(b)
            ELSE_TRY_READ_IF(i)
            ELSE_TRY_READ_IF(u)
            ELSE_TRY_READ_IF(sz)
            ELSE_TRY_READ_IF(strike)
            ELSE_TRY_READ_IF(dstrike)
            ELSE_TRY_READ_IF(color)
            ELSE_TRY_READ_IF(highlight)
            ELSE_TRY_READ_IF(lang)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (CALLER_IS(r)) {
//kDebug() << "CALLER_IS(r)";
        // DrawingML: b, i, strike, u attributes:
        if (attrs.hasAttribute("b")) {
            m_currentTextStyleProperties->setFontWeight(readBooleanAttr("b") ? QFont::Bold : QFont::Normal);
        }
        if (attrs.hasAttribute("i")) {
            m_currentTextStyleProperties->setFontItalic(readBooleanAttr("i"));
//kDebug() << "ITALIC:" << m_currentTextStyleProperties->fontItalic();
        }
        // from 20.1.10.79 ST_TextStrikeType (Text Strike Type)
        TRY_READ_ATTR_WITHOUT_NS(strike)
        if (strike == QLatin1String("sngStrike")) {
            m_currentTextStyleProperties->setStrikeOutType(KoCharacterStyle::SingleLine);
            m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
        }
        else if (strike == QLatin1String("dblStrike")) {
            m_currentTextStyleProperties->setStrikeOutType(KoCharacterStyle::DoubleLine);
            m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
        }
        else {
            // empty or "noStrike"
        }
        TRY_READ_ATTR_WITHOUT_NS(u)
        if (!u.isEmpty()) {
            MSOOXML::Utils::setupUnderLineStyle(u, m_currentTextStyleProperties);
        }
        // elements
        m_currentTextStyleProperties->saveOdf(m_currentTextStyle);

        READ_EPILOGUE_WITHOUT_RETURN
        // read 't' in one go and insert the contents into text:span
        readNext();
        // Only create text:span if the next el. is 't'. Do not this the next el. is 'drawing', etc.
        if (QUALIFIED_NAME_IS(t)) {
            const QString currentTextStyleName( mainStyles->lookup(m_currentTextStyle) );
            body->startElement("text:span", false);
            body->addAttribute("text:style-name", currentTextStyleName);
            TRY_READ(t)
            body->endElement(); //text:span
        }
//kDebug() << "currentTextStyleName:" << currentTextStyleName;
        else {
            undoReadNext();
        }
        //already checked expectElEnd(MSOOXML_CURRENT_NS ":t");
//kDebug() << "/text:span";
        body = textSpanBuf.releaseWriter();
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    if (CALLER_IS(r)) {
        return KoFilter::OK;
    }

//kDebug() << "!CALLER_IS(r)";
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL b
//! b handler
//! CASE #1112
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_b()
{
    READ_PROLOGUE
    m_currentTextStyleProperties->setFontWeight(READ_BOOLEAN_ATTR ? QFont::Bold : QFont::Normal);
//kDebug() << JOIN(MSOOXML_CURRENT_NS ":", CURRENT_EL) << m_currentTextStyleProperties->fontWeight();
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL i
//! i handler
//! CASE #1112
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_i()
{
    READ_PROLOGUE
    m_currentTextStyleProperties->setFontItalic(READ_BOOLEAN_ATTR);
//kDebug() << JOIN(MSOOXML_CURRENT_NS ":", CURRENT_EL) << m_currentTextStyleProperties->fontItalic();
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL u
//! u handler
//! CASE #1149
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_u()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR(val)
//! @todo more styles
    MSOOXML::Utils::setupUnderLineStyle(val, m_currentTextStyleProperties);

    TRY_READ_ATTR(color)
    QColor c(MSOOXML::Utils::ST_HexColorRGB_to_QColor(color));
    if (c.isValid()) {
        m_currentTextStyleProperties->setUnderlineColor(c);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sz
//! sz handler (Non-Complex Script Font Size) ECMA-376, 17.3.2.38, p.342
/*!    This element specifies the font size which shall be applied to all
       non complex script characters in the contents of this run when displayed.
*/
/*!
 Parent elements:
 - [done] rPr (§17.3.1.29)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - rPr (§17.7.9.1)
 - rPr (§17.7.5.4)
 - [done] rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)
 No child elements.
*/
//! @todo support all elements
//! CASE #1162
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_sz()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR(val)
// CASE #1164
    bool ok;
    const qreal pointSize = qreal( val.toUInt(&ok) ) / 2.0; /* half-points */
    if (ok) {
        m_currentTextStyleProperties->setFontPointSize(pointSize);
    }

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }

    READ_EPILOGUE
}

void MSOOXML_CURRENT_CLASS::readStrikeValue(KoCharacterStyle::LineType type)
{
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR(val)
    if (   val == MsooXmlReader::constOn
        || val == MsooXmlReader::constTrue
        || val == MsooXmlReader::const1
        || val.isEmpty())
    {
        m_currentTextStyleProperties->setStrikeOutType(type);
        m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
//! @todo m_currentTextStyleProperties->strikeOutWidth() ??
//! @todo m_currentTextStyleProperties->setStrikeOutColor() ??
//! @todo m_currentTextStyleProperties->setStrikeOutMode() ??
//! @todo m_currentTextStyleProperties->setStrikeOutText() ??
    }
}

#undef CURRENT_EL
#define CURRENT_EL strike
//! strike handler
//! CASE #1050
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_strike()
{
    READ_PROLOGUE
    readStrikeValue(KoCharacterStyle::SingleLine);
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL dstrike
//! dstrike handler
//! CASE #1051
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_dstrike()
{
    READ_PROLOGUE
    readStrikeValue(KoCharacterStyle::DoubleLine);
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL color
//! color handler
//! CASE #1158
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_color()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    READ_ATTR(val)
//! @todo more styles
    if (val == MsooXmlReader::constAuto) {
//! @todo set use-window-font-color="true" (currently no way to do this using KoCharacterStyle)
    }
    else {
        QColor color(MSOOXML::Utils::ST_HexColorRGB_to_QColor(val));
        if (color.isValid()) {
            m_currentTextStyleProperties->setForeground(QBrush(color));
        }
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL highlight
//! highlight handler (Text Highlighting)
/*! ECMA-376, 17.3.2.15, p.310
*/
//! CASE #1158
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_highlight()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    READ_ATTR(val)
    m_currentTextStyleProperties->setBackground( MSOOXML::Utils::ST_HighlightColor_to_QColor(val) );
    readNext();
    READ_EPILOGUE
}

//! CASE #410
void MSOOXML_CURRENT_CLASS::setParentParagraphStyleName(const QXmlStreamAttributes& attrs)
{
    TRY_READ_ATTR(pStyle)
    if (pStyle.isEmpty()) {
//! CASE #412
//! @todo
    }
    else {
//! CASE #411
        if (isDefaultTocStyle(pStyle)) {
            pStyle = QLatin1String("Contents") + pStyle.mid(3);
        }
    }

    if (pStyle.isEmpty())
        return;
    kDebug() << "parent paragraph style name set to:" << pStyle;
    m_currentParagraphStyle.setParentName(pStyle);
}

#undef CURRENT_EL
#define CURRENT_EL pPr
//! pPr handler (Paragraph Properties)
/*! ECMA-376, 17.3.1.26, p. 259.

 Parent elements:
 - [done] p (§17.3.1.22)
 Child elements:
 - adjustRightInd (Automatically Adjust Right Indent When Using Document Grid) §17.3.1.1
 - autoSpaceDE (Automatically Adjust Spacing of Latin and East Asian Text) §17.3.1.2
 - autoSpaceDN (Automatically Adjust Spacing of East Asian Text and Numbers) §17.3.1.3
 - bidi (Right to Left Paragraph Layout) §17.3.1.6
 - cnfStyle (Paragraph Conditional Formatting) §17.3.1.8
 - contextualSpacing (Ignore Spacing Above and Below When Using Identical Styles) §17.3.1.9
 - divId (Associated HTML div ID) §17.3.1.10
 - framePr (Text Frame Properties) §17.3.1.11
 - ind (Paragraph Indentation) §17.3.1.12
 - jc (Paragraph Alignment) §17.3.1.13
 - keepLines (Keep All Lines On One Page) §17.3.1.14
 - keepNext (Keep Paragraph With Next Paragraph) §17.3.1.15
 - kinsoku (Use East Asian Typography Rules for First and Last Character per Line) §17.3.1.16
 - mirrorIndents (Use Left/Right Indents as Inside/Outside Indents) §17.3.1.18
 - numPr (Numbering Definition Instance Reference) §17.3.1.19
 - outlineLvl (Associated Outline Level) §17.3.1.20
 - overflowPunct (Allow Punctuation to Extend Past Text Extents) §17.3.1.21
 - pageBreakBefore (Start Paragraph on Next Page) §17.3.1.23
 - pBdr (Paragraph Borders) §17.3.1.24
 - pPrChange (Revision Information for Paragraph Properties) §17.13.5.29
 - pStyle (Referenced Paragraph Style) §17.3.1.27
 - [done] rPr (Run Properties for the Paragraph Mark) §17.3.1.29
 - sectPr (Section Properties) §17.6.18
 - shd (Paragraph Shading) §17.3.1.31
 - snapToGrid (Use Document Grid Settings for Inter-Line Paragraph Spacing) §17.3.1.32
 - spacing (Spacing Between Lines and Above/Below Paragraph) §17.3.1.33
 - suppressAutoHyphens (Suppress Hyphenation for Paragraph) §17.3.1.34
 - suppressLineNumbers (Suppress Line Numbers for Paragraph) §17.3.1.35
 - suppressOverlap (Prevent Text Frames From Overlapping) §17.3.1.36
 - tabs (Set of Custom Tab Stops) §17.3.1.38
 - textAlignment (Vertical Character Alignment on Line) §17.3.1.39
 - textboxTightWrap (Allow Surrounding Paragraphs to Tight Wrap to Text Box Contents) §17.3.1.40
 - textDirection (Paragraph Text Flow Direction) §17.3.1.41
 - topLinePunct (Compress Punctuation at Start of a Line) §17.3.1.43
 - widowControl (Allow First/Last Line to Display on a Separate Page) §17.3.1.44
 - wordWrap (Allow Line Breaking At Character Level) §17.3.1.45
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_pPr()
{
    READ_PROLOGUE
//    if (!m_currentParagraphStyleCreated) {
        // add automatic style for the paragraph
//    }
    const QXmlStreamAttributes attrs( attributes() );
    setParentParagraphStyleName(attrs);

    TRY_READ_ATTR_WITHOUT_NS(lvl)
    m_pPr_lvl = lvl.toUInt(); // 0 (the default) on failure, so ok.

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(rPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

/* moved to buffer in read_p()
#ifndef SETUP_PARA_STYLE_IN_READ_P // for DOCX
    setupParagraphStyle();
#endif
*/
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL p
//! [1] p handler (Paragraph) ECMA-376, WPML 17.3.1.22, p. 251,
//!       This element specifies a paragraph of content in the document.

//! [2] p handler (Text Paragraphs) ECMA-376, DrawingML 21.1.2.2.6, p. 3587.
//!       This element specifies the presence of a paragraph of text within the containing text body.
/*!
 Parent elements:
 - [done] body (§17.2.2)
 - comment (§17.13.4.2)
 - customXml (§17.5.1.6)
 - docPartBody (§17.12.6)
 - endnote (§17.11.2)
 - footnote (§17.11.10)
 - ftr (§17.10.3)
 - hdr (§17.10.4)
 - sdtContent (§17.5.2.34)
 - tc (§17.4.66)
 - [done] p (§17.3.1.22)
 Child elements:
 - bdo (Bidirectional Override) §17.3.2.3
 - bookmarkEnd (Bookmark End) §17.13.6.1
 - bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - commentRangeStart (Comment Anchor Range Start) §17.13.4.4
 - customXml (Inline-Level Custom XML Element) §17.5.1.3
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - del (Deleted Run Content) §17.13.5.14
 - dir (Bidirectional Embedding Level) §17.3.2.8
 - fldSimple (Simple Field) §17.16.19
 - hyperlink (Hyperlink) §17.16.22
 - ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - oMath (Office Math) §22.1.2.77
 - oMathPara (Office Math Paragraph) §22.1.2.78
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - [done] pPr (Paragraph Properties) §17.3.1.26
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - [done] r (Text Run) §17.3.2.25
 - sdt (Inline-Level Structured Document Tag) §17.5.2.31
 - smartTag (Inline-Level Smart Tag) §17.5.1.9
 - subDoc (Anchor for Subdocument Location) §17.17.1.1
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_p()
{
    READ_PROLOGUE
    const read_p_args args = m_read_p_args;
    m_read_p_args = 0;
    m_paragraphStyleNameWritten = false;

    MSOOXML::Utils::XmlWriteBuffer textPBuf;

    if (args & read_p_Skip) {
        kDebug() << "SKIP!";
    }
    else {
        body = textPBuf.setWriter(body);
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::StyleAuto, "paragraph");
//        m_currentParagraphStyleNumber++;
//moved        body->addAttribute("text:style-name", currentParagraphStyleName());
//moved down
//      #ifdef SETUP_PARA_STYLE_IN_READ_P // for PPTX
//moved         setupParagraphStyle();
//moved #endif
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(p)) {
// CASE #301: avoid nested paragaraphs
                kDebug() << "Nested" << qualifiedName() << "detected: skipping the inner element";
                TRY_READ_WITH_ARGS( p, read_p_Skip; )
            }
// CASE #400.1
            ELSE_TRY_READ_IF(pPr)
// CASE #400.2
//! @todo add more conditions testing the parent
            ELSE_TRY_READ_IF(r)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (args & read_p_Skip) {
        //nothing
    }
    else {
        body = textPBuf.originalWriter();
        body->startElement("text:p", false);
//#ifndef SETUP_PARA_STYLE_IN_READ_P // for DOCX
        setupParagraphStyle();
//#endif
/*        if (!m_paragraphStyleNameWritten) {
            // no style, set default
            body->addAttribute("text:style-name", "Standard");
        }*/
        (void)textPBuf.releaseWriter();
        body->endElement(); //text:p
kDebug() << "/text:p";
    }
    READ_EPILOGUE
}

//! lstStyle handler (Text List Styles) ECMA-376, DrawingML 21.1.2.4.12, p. 3651.
//!          This element specifies the list of styles associated with this body of text.
/*!
 Parent elements:
 - lnDef (§20.1.4.1.20)
 - rich (§21.2.2.156)
 - spDef (§20.1.4.1.27)
 - t (§21.4.3.8)
 - txBody (§21.3.2.26)
 - txBody (§20.1.2.2.40)
 - txBody (§20.5.2.34)
 - [done] txBody (§19.3.1.51)
 - txDef (§20.1.4.1.28)
 - txPr (§21.2.2.216)

 Child elements:
 - defPPr (Default Paragraph Style) §21.1.2.2.2
 - extLst (Extension List) §20.1.2.2.15
 - lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
 - lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
 - lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
 - lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
 - lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
 - lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
 - lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
 - lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
 - lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all elements
#undef CURRENT_EL
#define CURRENT_EL lstStyle
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lstStyle()
{
    READ_PROLOGUE
    m_lstStyleFound = true;

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL drawing
//! drawing handler (DrawingML Object)
/*! ECMA-376, 17.3.3.9, p.362.

 This element specifies that a DrawingML object is located at this position
 in the run’s contents. The layout properties of this DrawingML object
 are specified using the WordprocessingML Drawing syntax (§20.4, p. 3466).

 Parent elements:
 - background (§17.2.1)
 - numPicBullet (§17.9.21)
 - object (§17.3.3.19)
 - r (§22.1.2.87) - Shared ML
 - [done] r (§17.3.2.25)

 Child elements:
 - [done] anchor (Anchor for Floating DrawingML Object) §20.4.2.3
 - [done] inline (Inline DrawingML Object) §20.4.2.8
*/
//! CASE #1300
//! CASE #1301
//! CASE #1380
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_drawing()
{
    READ_PROLOGUE

#ifdef DOCXXMLDOCREADER_H
        m_currentDrawStyle = KoGenStyle(KoGenStyle::StyleGraphicAuto, "graphic");
        m_currentDrawStyle.addAttribute("style:parent-style-name", QLatin1String("Graphics"));
#endif

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF_NS(wp, anchor)
            ELSE_TRY_READ_IF_NS(wp, inline)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "wp" // wordprocessingDrawing

void MSOOXML_CURRENT_CLASS::distToODF(const char * odfEl, const QString emuValue)
{
    if (emuValue.isEmpty() || emuValue == "0") // skip 0cm which is the default
        return;
    QString s = MSOOXML::Utils::EMU_to_ODF_CM(emuValue);
    if (!s.isEmpty()) {
        m_currentDrawStyle.addProperty(QLatin1String(odfEl), s, KoGenStyle::GraphicType);
    }
}

void MSOOXML_CURRENT_CLASS::saveStyleWrap(const char * style)
{
    m_currentDrawStyle.addProperty(QLatin1String("style:wrap"), style, KoGenStyle::GraphicType);
}

#undef CURRENT_EL
#define CURRENT_EL anchor
//! anchor handler (Anchor for Floating DrawingML Object)
/*! ECMA-376, 20.4.2.3, p.3469.

 This element specifies that the DrawingML object located at this position
 in the document is a floating object.
 Within a WordprocessingML document, drawing objects can exist in two states:
 - Inline - The drawing object is in line with the text, and affects the line
   height and layout of its line (like a character glyph of similar size).
 - Floating - The drawing object is anchored within the text, but can be
   absolutely positioned in the document relative to the page.

 When this element encapsulates the DrawingML object's information, 
 then all child elements shall dictate the positioning of this object
 as a floating object on the page.

 Parent elements:
 - [done] drawing (§17.3.3.9)

 Child elements:
 - cNvGraphicFramePr (Common DrawingML Non-Visual Properties) §20.4.2.4
 - [done] docPr (Drawing Object Non-Visual Properties) §20.4.2.5
 - effectExtent (Object Extents Including Effects) §20.4.2.6
 - extent (Drawing Object Size) §20.4.2.7
 - [done] graphic (Graphic Object) §20.1.2.2.16
 - [done] positionH (Horizontal Positioning) §20.4.2.10
 - [done] positionV (Vertical Positioning) §20.4.2.11
 - simplePos (Simple Positioning Coordinates) §20.4.2.13
 - [done] wrapNone (No Text Wrapping) §20.4.2.15
 - [done] wrapSquare (Square Wrapping) §20.4.2.17
 - [done] wrapThrough (Through Wrapping) §20.4.2.18
 - [done] wrapTight (Tight Wrapping) §20.4.2.19
 - [done] wrapTopAndBottom (Top and Bottom Wrapping) §20.4.2.20

 Attributes:
 - allowOverlap (Allow Objects to Overlap)
 - [done] behindDoc (Display Behind Document Text)
 - [done] distB (Distance From Text on Bottom Edge) (see also: inline)
 - [done] distL (Distance From Text on Left Edge) (see also: inline)
 - [done] distR (Distance From Text on Right Edge) (see also: inline)
 - [done] distT (Distance From Text on Top Edge) (see also: inline)
 - hidden (Hidden)
 - layoutInCell (Layout In Table Cell)
 - locked (Lock Anchor)
 - relativeHeight (Relative Z-Ordering Position)
 - simplePos (Page Positioning)
*/
//! @todo support all elements
//! CASE #1340
//! CASE #1410
//! CASE #1420
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_anchor()
{
    READ_PROLOGUE
    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
    m_docPrName.clear();
    m_docPrDescr.clear();

    const QXmlStreamAttributes attrs( attributes() );
//! @todo parse 20.4.3.4 ST_RelFromH (Horizontal Relative Positioning), p. 3511
    READ_ATTR_WITHOUT_NS(distT)
    distToODF("fo:margin-top", distT);
    READ_ATTR_WITHOUT_NS(distB)
    distToODF("fo:margin-bottom", distB);
    READ_ATTR_WITHOUT_NS(distL)
    distToODF("fo:margin-left", distL);
    READ_ATTR_WITHOUT_NS(distR)
    distToODF("fo:margin-right", distR);

    const bool behindDoc = readBooleanAttr("behindDoc");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF_NS(a, graphic)
            ELSE_TRY_READ_IF(positionH)
            ELSE_TRY_READ_IF(positionV)
            ELSE_TRY_READ_IF(docPr)
            ELSE_TRY_READ_IF(wrapSquare)
            ELSE_TRY_READ_IF(wrapTight)
            ELSE_TRY_READ_IF(wrapThrough)
            else if (QUALIFIED_NAME_IS(wrapNone)) {
                // wrapNone (No Text Wrapping), ECMA-376, 20.4.2.15
                // This element specifies that the parent DrawingML object shall
                // not cause any text wrapping within the contents of the host
                // WordprocessingML document based on its display location.
                // CASE #1410
                readNext();
                if (!expectElEnd(QUALIFIED_NAME(wrapNone)))
                    return KoFilter::WrongFormat;
                saveStyleWrap("run-through");
                m_currentDrawStyle.addProperty(QLatin1String("style:run-through"),
                    (behindDoc || m_insideHdr || m_insideFtr) ? "background" : "foreground",
                    KoGenStyle::GraphicType);
            }
            else if (QUALIFIED_NAME_IS(wrapTopAndBottom)) {
                // 20.4.2.20 wrapTopAndBottom (Top and Bottom Wrapping)
                // This element specifies that text shall wrap around the top
                // and bottom of this object, but not its left or right edges.
                // CASE #1410
                readNext();
                if (!expectElEnd(QUALIFIED_NAME(wrapTopAndBottom)))
                    return KoFilter::WrongFormat;
                saveStyleWrap("none");
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
    READ_EPILOGUE
}

//! @todo Currently all read_wrap*() uses the same read_wrap(), no idea if they can behave differently
//! CASE #1425
void MSOOXML_CURRENT_CLASS::readWrap()
{
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(wrapText)
    if (wrapText == "bothSides")
        saveStyleWrap("parallel");
    else if (wrapText == "largest")
        saveStyleWrap("dynamic");
    else
        saveStyleWrap(wrapText.toLatin1());
//! @todo Is saveStyleWrap(wrapText) OK?
}

#undef CURRENT_EL
#define CURRENT_EL wrapSquare
//! wrapSquare handler (Square Wrapping)
/*! ECMA-376, 20.4.2.17, p.3497.
 This element specifies that text shall wrap around a virtual rectangle bounding
 this object. The bounds of the wrapping rectangle shall be dictated by the extents
 including the addition of the effectExtent element as a child of this element
 (if present) or the effectExtent present on the parent element.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - effectExtent (Object Extents Including Effects)

 Attributes:
 - distB (Distance From Text on Bottom Edge)
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - distT (Distance From Text (Top))
 - [done] wrapText (Text Wrapping Location)
*/
//! CASE #1410
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_wrapSquare()
{
    READ_PROLOGUE
    readWrap();

    while (!atEnd()) {
        readNext();
//        if (isStartElement()) {
//! @todo effectExtent
//        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wrapTight
//! wrapTight handler (Tight Wrapping)
/*! ECMA-376, 20.4.2.17, p.3497.
 This element specifies that text shall wrap around the wrapping polygon
 bounding this object as defined by the child wrapPolygon element.
 When this element specifies a wrapping polygon, it shall not allow text
 to wrap within the object's maximum left and right extents.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - wrapPolygon (Wrapping Polygon)

 Attributes:
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - [done] wrapText (Text Wrapping Location)
*/
//! CASE #1410
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_wrapTight()
{
    READ_PROLOGUE
    readWrap();

    while (!atEnd()) {
        readNext();
//        if (isStartElement()) {
//! @todo effectExtent
//        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wrapThrough
//! wrapThrough handler (Through Wrapping)
/*! ECMA-376, 20.4.2.18, p.3500.
 This element specifies that text shall wrap around the wrapping polygon
 bounding this object as defined by the child wrapPolygon element.
 When this element specifies a wrapping polygon, it shall allow text
 to wrap within the object's maximum left and right extents.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - wrapPolygon (Wrapping Polygon)

 Attributes:
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - [done] wrapText (Text Wrapping Location)
*/
//! CASE #1410
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_wrapThrough()
{
    READ_PROLOGUE
    readWrap();

    while (!atEnd()) {
        readNext();
//        if (isStartElement()) {
//! @todo effectExtent
//        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL positionH
//! positionH handler (Horizontal Positioning)
/*! ECMA-376, 20.4.2.10, p.3490.
 This element specifies the horizontal positioning of a floating
 DrawingML object within a WordprocessingML document.
 This positioning is specified in two parts:
 
 - Positioning Base - The relativeFrom attribute on this element
   specifies the part of the document from which the positioning
   shall be calculated.
 - Positioning - The child element of this element (align
   or posOffset) specifies how the object is positioned relative
   to that base.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - [done] align (Relative Horizontal Alignment) §20.4.2.1
 - [done] posOffset (Absolute Position Offset) §20.4.2.12

 Attributes:
 - [done] relativeFrom (Horizontal Position Relative Base)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_positionH()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
//! @todo parse 20.4.3.4 ST_RelFromH (Horizontal Relative Positioning), p. 3511
    READ_ATTR_WITHOUT_NS_INTO(relativeFrom, m_relativeFromH)

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(align)
            ELSE_TRY_READ_IF(posOffset)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL positionV
//! positionV handler (Vertical Positioning)
/*! ECMA-376, 20.4.2.11, p.3491.
 This element specifies the vertical positioning of a floating
 DrawingML object within a WordprocessingML document.
 This positioning is specified in two parts:
 
 - Positioning Base - The relativeFrom attribute on this element
   specifies the part of the document from which the positioning
   shall be calculated.
 - Positioning - The child element of this element (align
   or posOffset) specifies how the object is positioned relative
   to that base.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - [done] align (Relative Vertical Alignment) §20.4.2.2
 - [done] posOffset (Absolute Position Offset) §20.4.2.12

 Attributes:
 - [done] relativeFrom (Horizontal Position Relative Base)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_positionV()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
//! @todo parse 20.4.3.5 ST_RelFromV (Vertical Relative Positioning), p. 3512
    READ_ATTR_WITHOUT_NS_INTO(relativeFrom, m_relativeFromV)

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(align)
            ELSE_TRY_READ_IF(posOffset)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL align
//! align handler (Relative Horizontal Alignment, Relative Vertical Alignment)
/*! ECMA-376, 20.4.2.1, 20.4.2.2, p.3468, 3469.
 This element specifies how a DrawingML object shall be horizontally/vertically
 aligned relative to the horizontal alignment base defined
 by the parent element. Once an alignment base is defined,
 this element shall determine how the DrawingML object shall
 be aligned relative to that location.

 Parent elements:
 - [done] positionH (§20.4.2.10)
 - [done] positionV (§20.4.2.11)

 No child elements.
*/
//! CASE #1340
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_align()
{
    ReadMethod caller = m_calls.top();
    READ_PROLOGUE
    if (CALLER_IS(positionH)) {
//! 20.4.3.1 ST_AlignH (Relative Horizontal Alignment Positions), p. 3508.
/*center
inside
left
outside
right*/
        m_alignH = text().toString();
    }
    else if (CALLER_IS(positionV)) {
//! 20.4.3.2 ST_AlignV (Vertical Alignment Definition), p. 3509.
/*bottom
center
inside
outside
top*/
        m_alignV = text().toString();
    }

    SKIP_EVERYTHING
/*    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }*/
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL posOffset
//! posOffset handler (Absolute Position Offset)
/*! ECMA-376, 20.4.2.12, p.3492.
 This element specifies an absolute measurement for the positioning
 of a floating DrawingML object within a WordprocessingML document.
 This measurement shall be calculated relative to the top left edge
 of the positioning base specified by the parent element's
 relativeFrom attribute.

 Parent elements:
 - [done] positionH (§20.4.2.10)
 - [done] positionV (§20.4.2.11)

 No child elements.
*/
//! CASE #1360
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_posOffset()
{
    ReadMethod caller = m_calls.top();
    READ_PROLOGUE

    readNext();
    if (isCharacters()) {
        if (CALLER_IS(positionH)) {
            STRING_TO_INT(text().toString(), m_posOffsetH)
            m_hasPosOffsetH = true;
        }
        else if (CALLER_IS(positionV)) {
            STRING_TO_INT(text().toString(), m_posOffsetV)
            m_hasPosOffsetV = true;
        }
        ELSE_WRONG_FORMAT
    }
    ELSE_WRONG_FORMAT

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL inline
//! inline handler (Inline DrawingML Object)
/*! ECMA-376, 20.4.2.8, p.3485.

 This element specifies that the DrawingML object located at this position
 in the document is a floating object.
 Within a WordprocessingML document, drawing objects can exist in two states:
 - Inline - The drawing object is in line with the text, and affects the line
   height and layout of its line (like a character glyph of similar size).
 - Floating - The drawing object is anchored within the text, but can be
   absolutely positioned in the document relative to the page.

 When this element encapsulates the DrawingML object's information, 
 then all child elements shall dictate the positioning of this object
 as a floating object on the page.

 Parent elements:
 - [done] drawing (§17.3.3.9)

 Child elements:
 - cNvGraphicFramePr (Common DrawingML Non-Visual Properties) §20.4.2.4
 - [done] docPr (Drawing Object Non-Visual Properties) §20.4.2.5
 - effectExtent (Object Extents Including Effects) §20.4.2.6
 - extent (Drawing Object Size) §20.4.2.7
 - [done] graphic (Graphic Object) §20.1.2.2.16

 Attributes:
 - distB (Distance From Text on Bottom Edge)
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - distT (Distance From Text on Top Edge)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_inline()
{
    READ_PROLOGUE
    m_docPrName.clear();
    m_docPrDescr.clear();

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF_NS(a, graphic)
            TRY_READ_IF(docPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL docPr
//! docPr handler (Drawing Object Non-Visual Properties)
/*! ECMA-376, 20.4.2.5, p.3478.

 This element specifies non-visual object properties for the parent DrawingML object.
 These properties are specified as child elements of this element.

 Parent elements:
 - [done]anchor (§20.4.2.3)
 - inline (§20.4.2.8)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - hlinkClick (Click Hyperlink) §21.1.2.3.5
 - hlinkHover (Hyperlink for Hover) §20.1.2.2.23

 Attributes:
 - descr (Alternative Text for Object)
 - hidden (Hidden)
 - id (Unique Identifier)
 - name (Name)
*/
//! CASE #1340
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_docPr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_docPrName)
    TRY_READ_ATTR_WITHOUT_NS_INTO(descr, m_docPrDescr)
//! @todo support docPr/@hidden (maybe to style:text-properties/@text:display)

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wrapSquare
//! wrapSquare handler (Drawing Object Non-Visual Properties)
/*! ECMA-376, 20.4.2.5, p.3478.

 This element specifies non-visual object properties for the parent DrawingML object.
 These properties are specified as child elements of this element.

 Parent elements:
 - [done]anchor (§20.4.2.3)
 - inline (§20.4.2.8)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - hlinkClick (Click Hyperlink) §21.1.2.3.5
 - hlinkHover (Hyperlink for Hover) §20.1.2.2.23

 Attributes:
 - descr (Alternative Text for Object)
 - hidden (Hidden)
 - id (Unique Identifier)
 - name (Name)
*/
//! CASE #1410

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a" // DrawingML

#undef CURRENT_EL
#define CURRENT_EL graphic
//! graphic handler (Graphic Object)
/*! ECMA-376, 20.1.2.2.16, p.3037.

 This element specifies the existence of a single graphic object.
 Document authors should refer to this element when they wish to persist
 a graphical object of some kind. The specification for this graphical
 object is provided entirely by the document author and referenced within
 the graphicData child element.

 Parent elements:
 - [done] anchor (§20.4.2.3)
 - graphicFrame (§21.3.2.12)
 - graphicFrame (§20.1.2.2.18)
 - graphicFrame (§20.5.2.16)
 - graphicFrame (§19.3.1.21)
 - [done] inline (§20.4.2.8)

 Child elements:
 - [done] graphicData (Graphic Object Data) §20.1.2.2.17
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_graphic()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(graphicData)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphicData
//! graphicData handler (Graphic Object Data)
/*! ECMA-376, 20.1.2.2.17, p.3038.

 This element specifies the reference to a graphic object within the document.
 This graphic object is provided entirely by the document authors who choose
 to persist this data within the document.

 Parent elements:
 - [done] graphic (§20.1.2.2.16)

 Child elements:
 - Any element in any namespace

 Attributes:
 - uri (Uniform Resource Identifier)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_graphicData()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF_NS(pic, pic)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS blipFill_NS

#undef CURRENT_EL
#define CURRENT_EL blipFill
//! blipFill handler (Picture Fill)
//! ECMA-376, PresentationML, 19.3.1.4, p. 2818; DrawingML, 20.1.8.14, p. 3195
//! @todo use it in DrawingML, 20.2.2.1, p. 3456
/*! This element specifies the type of picture fill that the picture object has.
 Because a picture has a picture fill already by default, it is possible to have
 two fills specified for a picture object.

 BLIPs refer to Binary Large Image or Pictures. Blip Fills are made up of several components: a Blip
 Reference, a Source Rectangle, and a Fill Mode.
 See also M.4.8.4.3 Blip Fills, ECMA-376, p. 5411.

 Parent elements:
    - bg (§21.4.3.1)
    - bgFillStyleLst (§20.1.4.1.7)
    - bgPr (§19.3.1.2)
    - defRPr (§21.1.2.3.2)
    - endParaRPr (§21.1.2.2.3)
    - fill (§20.1.8.28)
    - fill (§20.1.4.2.9)
    - fillOverlay (§20.1.8.29)
    - fillStyleLst (§20.1.4.1.13)
    - grpSpPr (§21.3.2.14)
    - grpSpPr (§20.1.2.2.22)
    - grpSpPr (§20.5.2.18)
    - grpSpPr (§19.3.1.23)
    - [done] pic (§20.1.2.2.30) - DrawingML
    - [done] pic (§19.3.1.37) - PresentationML
    - rPr (§21.1.2.3.9)
    - spPr (§21.2.2.197)
    - spPr (§21.3.2.23)
    - spPr (§21.4.3.7)
    - spPr (§20.1.2.2.35)
    - spPr (§20.2.2.6)
    - spPr (§20.5.2.30)
    - spPr (§19.3.1.44)
    - tblPr (§21.1.3.15)
    - tcPr (§21.1.3.17)
    - uFill (§21.1.2.3.12)

 Child elements:
    - [done] blip (Blip) §20.1.8.13
    - srcRect (Source Rectangle) §20.1.8.55
    - stretch (Stretch) §20.1.8.56
    - tile (Tile) §20.1.8.58

 Attributes:
    - dpi (DPI Setting)
    - rotWithShape (Rotate With Shape)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_blipFill()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:blip")) {
                TRY_READ(blip)
            }
            else if (qualifiedName() == QLatin1String("a:stretch")) {
                TRY_READ(stretch)
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#undef CURRENT_EL
#define CURRENT_EL blip
//! blip handler (Blip)
//! ECMA-376, 20.1.8.13, p. 3194
/*! This element specifies the existence of an image (binary large image or picture)
    and contains a reference to the image data.

 Parent elements:
    - blipFill (§21.3.2.2) - DrawingML, p. 3919
    - [done] blipFill (§20.1.8.14) - DrawingML, p. 3195
    - blipFill (§20.2.2.1) - DrawingML, p. 3456
    - blipFill (§20.5.2.2) - DrawingML, p. 3518
    - [done] blipFill (§19.3.1.4) - PresentationML, p. 2818
    - buBlip (§21.1.2.4.2)

 Child elements:
    - alphaBiLevel (Alpha Bi-Level Effect) §20.1.8.1
    - alphaCeiling (Alpha Ceiling Effect) §20.1.8.2
    - alphaFloor (Alpha Floor Effect) §20.1.8.3
    - alphaInv (Alpha Inverse Effect) §20.1.8.4
    - alphaMod (Alpha Modulate Effect) §20.1.8.5
    - alphaModFix (Alpha Modulate Fixed Effect) §20.1.8.6
    - alphaRepl (Alpha Replace Effect) §20.1.8.8
    - biLevel (Bi-Level (Black/White) Effect) §20.1.8.11
    - blur (Blur Effect) §20.1.8.15
    - clrChange (Color Change Effect) §20.1.8.16
    - clrRepl (Solid Color Replacement) §20.1.8.18
    - duotone (Duotone Effect) §20.1.8.23
    - extLst (Extension List) §20.1.2.2.15
    - fillOverlay (Fill Overlay Effect) §20.1.8.29
    - grayscl (Gray Scale Effect) §20.1.8.34
    - hsl (Hue Saturation Luminance Effect) §20.1.8.39
    - lum (Luminance Effect) §20.1.8.42
    - tint (Tint Effect) §20.1.8.60

 Attributes:
    - cstate (Compression State)
    - [done] embed (Embedded Picture Reference), 22.8.2.1 ST_RelationshipId (Explicit Relationship ID), p. 4324
    - link (Linked Picture Reference)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_blip()
{
    READ_PROLOGUE

    m_xlinkHref.clear();
    const QXmlStreamAttributes attrs( attributes() );
//! @todo more attrs
    TRY_READ_ATTR_WITH_NS(r, embed)
    kDebug() << "embed:" << r_embed;
    if (!r_embed.isEmpty()) {
        const QString sourceName( m_context->relationships->target(m_context->path, m_context->file, r_embed) );
        kDebug() << "sourceName:" << sourceName;
        if (sourceName.isEmpty()) {
            return KoFilter::FileNotFound;
        }
        const QString destinationName(
            QLatin1String("Pictures/") + sourceName.mid( sourceName.lastIndexOf('/') + 1 ) );
        if (m_copiedFiles.contains(sourceName)) {
            kDebug() << sourceName << "already copied - skipping";
        }
        else {
//! @todo should we check name uniqueness here in case the sourceName can be located in various directories?
            const KoFilter::ConversionStatus status = m_context->import->copyFile(sourceName, destinationName);
            if (status != KoFilter::OK) {
                return status;
            }
            m_copiedFiles.insert(sourceName);
        }
        m_xlinkHref = destinationName;
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL stretch
//! stretch handler (Stretch)
//! ECMA-376, 20.1.8.56, p. 3233
/*! This element specifies that a BLIP should be stretched
 to fill the target rectangle. The other option is a tile where
 a BLIP is tiled to fill the available area.

 Parent elements:
    - blipFill (§21.3.2.2) - DrawingML, p. 3919
    - [done] blipFill (§20.1.8.14) - DrawingML, p. 3195
    - blipFill (§20.2.2.1) - DrawingML, p. 3456
    - blipFill (§20.5.2.2) - DrawingML, p. 3518
    - [done] blipFill (§19.3.1.4) - PresentationML, p. 2818

 Child elements:
    - [done] fillRect (Fill Rectangle) §20.1.8.30
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_stretch()
{
    READ_PROLOGUE

    m_fillImageRenderingStyle.clear();

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(fillRect)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fillRect
//! fillRect handler (Fill Rectangle)
//! ECMA-376, 20.1.8.30, p. 3212
/*! This element specifies a fill rectangle. When stretching of an image
    is specified, a source rectangle, srcRect, is scaled to fit the specified fill rectangle.

 Parent elements:
    - [done] stretch (§20.1.8.56)

 No child elements.

 Attributes:
    - b (Bottom Offset)
    - l (Left Offset)
    - r (Right Offset)
    - t (Top Offset)

 Complex type: CT_RelativeRect, p. 4545
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_fillRect()
{
    READ_PROLOGUE

//    const QXmlStreamAttributes attrs( attributes() );
//! @todo use ST_Percentage_withMsooxmlFix_to_double for attributes b, l, r, t
/*    TRY_READ_ATTR_WITHOUT_NS(r, b)
    TRY_READ_ATTR_WITHOUT_NS(r, l)
    TRY_READ_ATTR_WITHOUT_NS(r, r)
    TRY_READ_ATTR_WITHOUT_NS(r, t)*/
//MSOOXML_EXPORT double ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok);

    m_fillImageRenderingStyle = QLatin1String("stretch");
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

//! @todo KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_srcRect()
/*
 No child elements.

 Attributes:
    - b (Bottom Offset)
    - l (Left Offset)
    - r (Right Offset)
    - t (Top Offset)

 Complex type: CT_RelativeRect, p. 4545

 const QXmlStreamAttributes attrs( attributes() );
 use double ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok)....
*/


#if 0 //todo
#undef CURRENT_EL
#define CURRENT_EL background
//! background handler (Document Background)
/*! ECMA-376, 17.2.1, p. 199.
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_background()
{
}
#endif

#endif
