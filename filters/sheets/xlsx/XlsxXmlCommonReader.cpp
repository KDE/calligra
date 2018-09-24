/*
 * This file is part of Office 2007 Filters for Calligra
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

#include "XlsxUtils.h"
#include "XlsxXmlCommonReader.h"

#include <math.h>

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
#include <KoCharacterStyle.h>

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlCommonReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

class XlsxXmlCommonReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
private:
};

XlsxXmlCommonReader::XlsxXmlCommonReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , d(new Private)
{
    init();
}

XlsxXmlCommonReader::~XlsxXmlCommonReader()
{
    delete d;
}

void XlsxXmlCommonReader::init()
{
    m_currentTextStyleProperties = 0;
}

QColor XlsxXmlCommonReader::tintedColor(const QColor& color, qreal tint)
{
    const int HLSMAX = 255; // Used for computing tint
    if (tint == 0.0 || !color.isValid()) {
        return color;
    }
    int h, l, s;
    color.getHsl(&h, &l, &s);
    if (tint < 0.0) {
        l = floor( l * (1.0 + tint) );
    }
    else {
        l = floor( l * (1.0 - tint) + (HLSMAX - HLSMAX * (1.0 - tint)) );
    }
    int r, g, b;
    color.getRgb(&r, &g, &b);
    return QColor(r, g, b, color.alpha());
}

#undef CURRENT_EL
#define CURRENT_EL t
//! t handler (Text)
/*! ECMA-376, 18.4.12, p. 1914.
 This element represents the text content shown as part of a string.

 No child elements.
 Parent elements:
 - is (§18.3.1.53)
 - [done] r (§18.4.4)
 - rPh (§18.4.6)
 - [done] si (§18.4.8)
 - text (§18.7.7)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_t()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        if (isCharacters()) {
            body->addTextSpan(text().toString());
        }
        BREAK_IF_END_OF(CURRENT_EL)
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL r
//! r handler (Rich Text Run)
/*! ECMA-376, 18.4.12, p. 1909.
 This element represents a run of rich text. A rich text run is a region of text that share a common set of
 properties, such as formatting properties. The properties are defined in the rPr element, and the text displayed
 to the user is defined in the Text (t) element.

 Parent elements:
 - is (§18.3.1.53)
 - [done] si (§18.4.8)
 - text (§18.7.7)

 Child elements:
 - [done] rPr (§18.4.7)
 - [done] t (§18.4.12)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_r()
{
    READ_PROLOGUE

    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    MSOOXML::Utils::XmlWriteBuffer rBuf;
    body = rBuf.setWriter(body);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(rPr)
            ELSE_TRY_READ_IF(t)
            ELSE_WRONG_FORMAT
        }
    }

    body = rBuf.originalWriter();

    body->startElement("text:span", false);
    if (!m_currentTextStyle.isEmpty() || !m_currentTextStyle.parentName().isEmpty()) {
        const QString currentTextStyleName(mainStyles->insert(m_currentTextStyle));
        body->addAttribute("text:style-name", currentTextStyleName);
    }

    (void)rBuf.releaseWriter();

    body->endElement(); //text:span

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPr
//! rPr handler (Run Properties)
/*! ECMA-376, 18.4.7, p. 1911.
 This element represents a set of properties to apply to the contents of this rich text run.

 Parent elements:
 - [done] r (§18.4.4)

 Child elements:
 - [done] b §18.8.2
 - charset §18.4.1
 - [done] color §18.3.1.15
 - condense §18.8.12
 - extend §18.8.17
 - family §18.8.18
 - [done] i §18.8.26
 - [done] outline §18.4.2
 - [done] rFont §18.4.5
 - [done] scheme §18.8.35
 - shadow §18.8.36
 - [done] strike §18.4.10
 - [done] sz §18.4.11
 - [done] u §18.4.13
 - [done] vertAlign §18.4.14

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_rPr()
{
    READ_PROLOGUE
    m_currentTextStyleProperties = new KoCharacterStyle;

    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(vertAlign)
            ELSE_TRY_READ_IF(sz)
            ELSE_TRY_READ_IF(rFont)
            ELSE_TRY_READ_IF(color)
            ELSE_TRY_READ_IF(u)
            ELSE_TRY_READ_IF(i)
            ELSE_TRY_READ_IF(b)
            ELSE_TRY_READ_IF(strike)
            ELSE_TRY_READ_IF(outline)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        m_currentTextStyleProperties->setForeground(QBrush(m_currentColor));
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL vertAlign
//! vertAlign handler (Vertical Alignment)
/*! ECMA-376, 18.4.7, p. 1914.
 This element adjusts the vertical position of the text relative to the text's default appearance for this run.
 It is used to get 'superscript' or 'subscript' texts, and shall reduce the font size
 (if a smaller size is available) accordingly.

 Parent elements:
 - [done] font (§18.8.22)
 - [done] rPr (§18.4.7)

 No child elements.

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_vertAlign()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    if (val == "subscript") {
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSubScript);
    }
    else if (val == "superscript") {
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sz
//! fontSize
/*
 Parent elements:
 - [done] font (§18.8.22);
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_sz()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        m_currentTextStyleProperties->setFontPointSize(val.toDouble());
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rFont
//! font
/*
 Parent elements:
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_rFont()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        m_currentTextStyle.addProperty("fo:font-family", val);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL i
//! i handler (Italic)
/*! ECMA-376, 18.8.26, p. 1969.
 Displays characters in italic font style.

 Parent elements:
 - [done] font (§18.8.22)
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_i()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    const bool italic = MSOOXML::Utils::convertBooleanAttr(val, true);
    m_currentTextStyleProperties->setFontItalic(italic);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL scheme
//! scheme handler (Scheme)
/*!

 Parent elements:
 - [done] font (§18.8.22)
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_scheme()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    QString font;

    if (val == "major") {
        font = m_themes->fontScheme.majorFonts.latinTypeface;
        m_currentTextStyle.addProperty("fo:font-family", font);
    } else if (val == "minor") {
        font = m_themes->fontScheme.minorFonts.latinTypeface;
        m_currentTextStyle.addProperty("fo:font-family", font);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL b
//! b handler (Bold)
/*! ECMA-376, 18.8.2, p. 1947.
 Displays characters in bold face font style.

 Parent elements:
 - [done] font (§18.8.22)
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_b()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    const bool bold = MSOOXML::Utils::convertBooleanAttr(val, true);
    m_currentTextStyleProperties->setFontWeight(bold ? QFont::Bold : QFont::Normal);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL outline
//! outline handler (Outline)
/*!

 Parent elements:
 - [done] font (§18.8.22)
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_outline()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    if (val == "1") {
        m_currentTextStyleProperties->setTextOutline(QPen(Qt::SolidLine));
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL strike
//! strike handler (Strike Through)
/*! ECMA-376, 18.4.10, p. 1913.
 This element draws a strikethrough line through the horizontal middle of the text.

 Parent elements:
 - [done] font (§18.8.22)
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_strike()
{
    READ_PROLOGUE

    m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
    m_currentTextStyleProperties->setStrikeOutType(KoCharacterStyle::SingleLine);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL u
//! u handler (Underline)
/*! ECMA-376, 18.4.13, p. 1914.
 This element represents the underline formatting style.

 Parent elements:
 - [done] font (§18.8.22)
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_u()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(val)
    if (!val.isEmpty()) {
        MSOOXML::Utils::setupUnderLineStyle(val, m_currentTextStyleProperties);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL color
//! color
/*
 Parent elements:
 - [done] font (§18.8.22);
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_color()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(indexed)
    TRY_READ_ATTR_WITHOUT_NS(rgb)
    TRY_READ_ATTR_WITHOUT_NS(theme)
    TRY_READ_ATTR_WITHOUT_NS(tint)

    if (!indexed.isEmpty()) {
        int index = indexed.toInt();
        if (index >= 0 && index < 64) {
            m_currentColor = QString("#%1").arg(m_colorIndices.at(index));
        }
    }
    if (!rgb.isEmpty()) {
        m_currentColor = QString("#" + rgb.right(rgb.length()-2));
    }
    if (!theme.isEmpty()) {
        // Xlsx seems to switch these indices
        if (theme == "0" ) {
            theme = "1";
        }
        else if (theme == "1" ) {
            theme = "0";
        }
        else if (theme == "2") {
            theme = "3";
        }
        else if (theme == "3") {
            theme = "2";
        }
        MSOOXML::DrawingMLColorSchemeItemBase *colorItemBase = m_themes->colorScheme.value(theme);
        if (colorItemBase) {
            m_currentColor = colorItemBase->value();
        }
    }
    if (!tint.isEmpty()) {
        m_currentColor = tintedColor(m_currentColor, tint.toDouble());
    }

    readNext();
    READ_EPILOGUE
}
