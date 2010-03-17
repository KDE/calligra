/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
    m_posOffsetH = 0;
    m_posOffsetV = 0;
    m_currentTextStylePredefined = false;
    m_currentTextStyleProperties = 0;
    m_fillImageRenderingStyleStretch = false;
}

void MSOOXML_CURRENT_CLASS::doneInternal()
{
    delete m_currentTextStyleProperties;
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
#define CURRENT_EL lang
//! w:lang handler (Languages for Run Content)
/*! ECMA-376, 17.3.2.20, p.314.
 Parent elements:
 - [done] rPr (§17.3.1.29) (within pPr §17.3.1.26)
 - rPr (§17.3.1.30) (within rPrChange §17.13.5.30)
 - rPr (§17.3.2.27) (within del §17.13.5.13, ins §17.13.5.16, rPrChange §17.13.5.31)
 - [done] rPr (§17.3.2.28) (witin ctrlPr §22.1.2.23, r §22.1.2.87, r §17.3.2.25)
 - rPr (§17.5.2.27) (within sdtPr §17.5.2.38)
 - rPr (§17.5.2.28) (within sdtEndPr §17.5.2.37)
 - [done] rPr (§17.7.9.1) (within style §17.7.4.17 - styles)
 - [done] rPr (§17.7.5.4) (within rPrDefault §17.7.5.5 - styles)
 - rPr (§17.7.6.2) (within tblStylePr §17.7.6.6 - styles)
 - rPr (§17.9.25) (within lvl §17.9.6, lvl §17.9.7 - numbering)
 No child elements.

 CASE #850 -> CASE #858 -> CASE #861 -> CASE #1100

 @todo support all elements
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lang()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
// CASE #1100
    TRY_READ_ATTR(bidi)
    QString language, country;
    if (!bidi.isEmpty()) {
        if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(bidi, language, country)) {
            m_currentTextStyle.addProperty("style:language-complex", language, KoGenStyle::TextType);
            m_currentTextStyle.addProperty("style:country-complex", country, KoGenStyle::TextType);
        } else {
            kWarning() << "invalid value of \"bidi\" attribute:" << bidi << " - skipping";
        }
    }
    TRY_READ_ATTR(val)
    if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(val, language, country)) {
        m_currentTextStyle.addProperty("fo:language", language, KoGenStyle::TextType);
        m_currentTextStyle.addProperty("fo:country", country, KoGenStyle::TextType);
    } else {
        kWarning() << "invalid value of \"val\" attribute:" << val << " - skipping";
    }

    TRY_READ_ATTR(eastAsia)
    if (!eastAsia.isEmpty()) {
        if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(eastAsia, language, country)) {
            m_currentTextStyle.addProperty("style:language-asian", language, KoGenStyle::TextType);
            m_currentTextStyle.addProperty("style:country-asian", country, KoGenStyle::TextType);
        } else {
            kWarning() << "invalid value of \"eastAsia\" attribute:" << eastAsia << " - skipping";
        }
    }
    kDebug() << "bidi:" << bidi << "val:" << val << "eastAsia:" << eastAsia;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPr
#if __GNUC__
#warning split read_rPr to DML/WML
#endif
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
 - [done] rFonts (Run Fonts) §17.3.2.26
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
 - [done] vertAlign (Subscript/Superscript Text) §17.3.2.42
 - w (Expanded/Compressed Text) §17.3.2.43
 - webHidden (Web Hidden Text) §17.3.2.44
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_rPr()
{
#ifdef DOCXXMLDOCREADER_CPP
    ReadMethod caller = m_calls.top();
#endif
    READ_PROLOGUE

    const bool setupTextStyle =
#ifdef DOCXXMLDOCREADER_CPP
        CALLER_IS(r);
#else
        true;
#endif
    kDebug() << "setupTextStyle:" << setupTextStyle;

    const QXmlStreamAttributes attrs(attributes());

    Q_ASSERT(m_currentTextStyleProperties == 0);
//    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = new KoCharacterStyle();

    if (!m_currentTextStylePredefined) {
        m_currentTextStyle = KoGenStyle(KoGenStyle::StyleTextAuto, "text");
    }

    MSOOXML::Utils::XmlWriteBuffer textSpanBuf;
    if (setupTextStyle) {
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
            ELSE_TRY_READ_IF(shd)
            ELSE_TRY_READ_IF(vertAlign)
            ELSE_TRY_READ_IF(rFonts)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (setupTextStyle) {
//kDebug() << "CALLER_IS(r)";
        // DrawingML: b, i, strike, u attributes:
        if (attrs.hasAttribute("b")) {
            m_currentTextStyleProperties->setFontWeight(MSOOXML::Utils::convertBooleanAttr(attrs.value("b").toString()) ? QFont::Bold : QFont::Normal);
        }
        if (attrs.hasAttribute("i")) {
            m_currentTextStyleProperties->setFontItalic(MSOOXML::Utils::convertBooleanAttr(attrs.value("i").toString()));
//kDebug() << "ITALIC:" << m_currentTextStyleProperties->fontItalic();
        }
        if (attrs.hasAttribute("sz")) {
            bool ok = false;
            const qreal pointSize = qreal(attrs.value("sz").toString().toUInt(&ok)) / 100.0;
            if (ok) {
                m_currentTextStyleProperties->setFontPointSize(pointSize);
            }
        }
        // from 20.1.10.79 ST_TextStrikeType (Text Strike Type)
        TRY_READ_ATTR_WITHOUT_NS(strike)
        if (strike == QLatin1String("sngStrike")) {
            m_currentTextStyleProperties->setStrikeOutType(KoCharacterStyle::SingleLine);
            m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
        } else if (strike == QLatin1String("dblStrike")) {
            m_currentTextStyleProperties->setStrikeOutType(KoCharacterStyle::DoubleLine);
            m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
        } else {
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
            const QString currentTextStyleName(mainStyles->lookup(m_currentTextStyle));
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

    if (setupTextStyle) {
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
qDebug() << "What is this?";
    m_currentTextStyleProperties->setFontWeight(READ_BOOLEAN_ATTR ? QFont::Bold : QFont::Normal);
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
    const QXmlStreamAttributes attrs(attributes());
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
 - rPr (§17.7.5.4) (within style)
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
    const char *ns = 0;
#ifdef MSOOXML_CURRENT_NS
    ns = MSOOXML_CURRENT_NS;
#endif
    RETURN_IF_ERROR(MSOOXML::MsooXmlReader::read_sz(ns, m_currentTextStyleProperties))
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL vertAlign
//! vertAlign handler (Subscript/Superscript Text) ECMA-376, 17.3.2.42, p.349
/*!    This element specifies the alignment which shall be applied to
       the contents of this run when displayed.
*/
/*!
 Parent elements:
 - [done] rPr
 No child elements.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_vertAlign()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
    if (QString::compare(val, "superscript", Qt::CaseInsensitive) == 0)
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    else if (QString::compare(val, "subscript", Qt::CaseInsensitive) == 0)
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSubScript);
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rFonts
//! w:lang handler (Run Fonts)
/*! ECMA-376, 17.3.2.26, p.323.
 Parent elements:
 - rPr (§17.3.1.29) (within p)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - rPr (§17.7.9.1)
 - rPr (§17.7.5.4) (within style)
 - rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)
 No child elements.

 CASE #850 -> CASE #858 -> CASE #861 -> CASE #1150

 @todo support all elements
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_rFonts()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
// CASE #1153
    TRY_READ_ATTR(ascii)
    if (!ascii.isEmpty()) {
        m_currentTextStyle.addProperty("style:font-name", ascii, KoGenStyle::TextType);
    }
// CASE #1152
    if (ascii.isEmpty()) {
        TRY_READ_ATTR(asciiTheme)
        if (!asciiTheme.isEmpty()) {
            //! @todo
        }
    }
// CASE #1155
    TRY_READ_ATTR(cs)
    if (!cs.isEmpty()) {
        m_currentTextStyle.addProperty("style:font-name-complex", cs, KoGenStyle::TextType);
    }
// CASE #1154
    if (cs.isEmpty()) {
        TRY_READ_ATTR(cstheme)
        if (!cstheme.isEmpty()) {
            //! @todo
        }
    }
// CASE #1157
    TRY_READ_ATTR(eastAsia)
    if (!eastAsia.isEmpty()) {
        m_currentTextStyle.addProperty("style:font-name-asian", eastAsia, KoGenStyle::TextType);
    }
// CASE #1156
    if (eastAsia.isEmpty()) {
        TRY_READ_ATTR(eastAsiaTheme)
        if (!eastAsiaTheme.isEmpty()) {
            //! @todo
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pStyle
//! pStyle handler (Referenced Paragraph Style)
/*! ECMA-376, WML, 17.3.1.27, p.260.

 This element specifies the style ID of the paragraph style which shall be used to format the contents
 of this paragraph. This formatting is applied at the following location in the style hierarchy:
 - Document defaults
 - Table styles
 - Numbering styles
 - Paragraph styles (this element)
 - Character styles
 - Direct Formatting

 This means that all properties specified in the style element (§17.7.4.17) with a styleId which
 corresponds to the value in this element's val attribute are applied to the paragraph at the appropriate
 level in the hierarchy.

 Parent elements:
 - pPr (§17.3.1.26)
 - pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
 - [done] pPr (§17.9.23)
 - [done] pPr (§17.7.8.2)

 No child elements.

 @todo support all elements
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_pStyle()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_INTO(val, m_currentStyleName)
    SKIP_EVERYTHING
    READ_EPILOGUE
}

void MSOOXML_CURRENT_CLASS::readStrikeValue(KoCharacterStyle::LineType type)
{
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
    if (val == MsooXmlReader::constOn
            || val == MsooXmlReader::constTrue
            || val == MsooXmlReader::const1
            || val.isEmpty()) {
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
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
//! @todo more styles
    if (val == MsooXmlReader::constAuto) {
//! @todo set use-window-font-color="true" (currently no way to do this using KoCharacterStyle)
    } else {
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
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    m_currentTextStyleProperties->setBackground(MSOOXML::Utils::ST_HighlightColor_to_QColor(val));
    readNext();
    READ_EPILOGUE
}
#undef CURRENT_EL
#define CURRENT_EL jc
//! Paragraph Alignment
/*! ECMA-376, 17.3.1.13, p.239
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_jc()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    // Does ODF support high/low/medium kashida ?
    val = val.toLower();
    if ((val == "both") || (val == "distribute"))
        m_currentParagraphStyle.addProperty("fo:text-align", "justify");
    else if ((val == "start") || (val == "left") || (val == "right") || (val == "center"))
        m_currentParagraphStyle.addProperty("fo:text-align", val);
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spacing
//! spacing handler (Spacing Between Lines and Above/Below Paragraph)
/*! ECMA-376, 17.3.1.33, p.269.

 This element specifies the inter-line and inter-paragraph spacing which shall be applied
 to the contents of this paragraph when it is displayed by a consumer.

 Parent elements:
 - pPr (§17.3.1.26)
 - pPr (§17.3.1.25)
 - pPr (§17.7.5.2)
 - pPr (§17.7.6.1)
 - pPr (§17.9.23)
 - pPr (§17.7.8.2)

 No child elements.

 @todo support all elements
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spacing()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(after)
    const QString marginBottom(MSOOXML::Utils::ST_TwipsMeasure_to_ODF(after));
    if (!marginBottom.isEmpty()) {
        m_currentParagraphStyle.addAttribute("fo:margin-bottom", marginBottom);
    }

    TRY_READ_ATTR(before)
    const QString marginTop(MSOOXML::Utils::ST_TwipsMeasure_to_ODF(before));
    if (!marginTop.isEmpty()) {
        m_currentParagraphStyle.addAttribute("fo:margin-top", marginTop);
    }

    TRY_READ_ATTR(line)
    TRY_READ_ATTR(lineRule)

    SKIP_EVERYTHING
    READ_EPILOGUE
}

//! CASE #410
void MSOOXML_CURRENT_CLASS::setParentParagraphStyleName(const QXmlStreamAttributes& attrs)
{
    TRY_READ_ATTR(pStyle)
    if (pStyle.isEmpty()) {
//! CASE #412
//! @todo
    } else {
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
/*!
 1. pPr (Paragraph Properties) 17.3.1.26, p.259.
 Parent elements:
 - [done] p (§17.3.1.22) - common reader

 2. pPr (Paragraph Properties) 17.7.5.2, p.725.
 Parent elements:
 - [done] pPrDefault - DocxXmlStylesReader

 3. pPr (Table Style Conditional Formatting Paragraph Properties)  17.7.6.1, p.733.
 Parent elements:
 - tblStylePr (§17.7.6.6)

 4. pPr (Style Paragraph Properties) 17.7.8.2, p.752.
 Parent elements:
 - [done] style (§17.7.4.17) - DocxXmlStylesReader

 5. pPr (Numbering Level Associated Paragraph Properties) 17.9.23, p.818.
 Parent elements:
 - lvl (§17.9.6)
 - lvl (§17.9.7)

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
 - [done] jc (Paragraph Alignment) §17.3.1.13
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
 - [done] pStyle (Referenced Paragraph Style) §17.3.1.27
 - [done] rPr (Run Properties for the Paragraph Mark) §17.3.1.29
 - sectPr (Section Properties) §17.6.18
 - [done] shd (Paragraph Shading) §17.3.1.31
 - snapToGrid (Use Document Grid Settings for Inter-Line Paragraph Spacing) §17.3.1.32
 - [done] spacing (Spacing Between Lines and Above/Below Paragraph) §17.3.1.33
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
//! CASE #850 -> CASE #853
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_pPr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    setParentParagraphStyleName(attrs);

    TRY_READ_ATTR_WITHOUT_NS(lvl)
    m_pPr_lvl = lvl.toUInt(); // 0 (the default) on failure, so ok.

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(rPr)
            ELSE_TRY_READ_IF(shd)
            ELSE_TRY_READ_IF(jc)
            ELSE_TRY_READ_IF(spacing)
            ELSE_TRY_READ_IF(pStyle)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL shd
//! Shading handler (object's shading attributes)
/*! ECMA-376, 17.3.5, p.399
 Parent Elements:
 - [done] pPr (Paragraph Properties) §17.3.1.26
 Attributes:
 - color (Shading Pattern Color)
 - fill (Shading Background Color)
 - themeColor (Shading Pattern Theme Color)
 - themeFill (Shading Background Theme Color)
 - themeFillShade (Shading Background Theme Color Shade)
 - themeFillTint (Shading Background Theme Color Tint)
 - themeShade (Shading Pattern Theme Color Shade)
 - themeTint (Shading Pattern Theme Color Tint)
 - [done] val (Shading Pattern)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_shd()
{
    ReadMethod caller = m_calls.top();
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    TRY_READ_ATTR(color)
//kDebug() << m_callsNames;
    if (!color.isEmpty() && color != MsooXmlReader::constAuto) {
        QColor clr(MSOOXML::Utils::ST_HexColorRGB_to_QColor(color));
        if (CALLER_IS(rPr) && clr.isValid() && (val.compare("solid", Qt::CaseInsensitive) == 0)) {
            m_currentTextStyleProperties->setBackground(clr);
        }
    }

    TRY_READ_ATTR(fill)
    QString fillColor = fill.toLower();
    if (!fillColor.isEmpty() && fillColor != MsooXmlReader::constAuto) {
        fillColor.prepend("#");
        if (CALLER_IS(pPr)) {
            m_currentParagraphStyle.addProperty("fo:background-color", fillColor);
        }
    }
    readNext();
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
    }

    READ_EPILOGUE
}

#endif
