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

#include "XlsxXmlCommonReader.h"
#include "XlsxXmlStylesReader.h"

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
    readNext();

    m_text = text().toString();

    while (!atEnd()) {
        kDebug() << *this;
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
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

    QString readResult;

    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        TRY_READ_IF(rPr)
        else if (QUALIFIED_NAME_IS(t)) {
            TRY_READ(t)
//! @todo
            kDebug() << "readResult += m_text" << readResult << m_text;
            readResult += m_text;
        }
//! @todo support rPr
//! @todo            ELSE_WRONG_FORMAT
    }
    m_text = readResult;

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
 - b §18.8.2
 - charset §18.4.1
 - color §18.3.1.15
 - condense §18.8.12
 - extend §18.8.17
 - family §18.8.18
 - i §18.8.26
 - outline §18.4.2
 - [done] rFont §18.4.5
 - scheme §18.8.35
 - shadow §18.8.36
 - strike §18.4.10
 - [done] sz §18.4.11
 - u §18.4.13
 - [done] vertAlign §18.4.14

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_rPr()
{
    READ_PROLOGUE
    m_currentTextStyleProperties = new KoCharacterStyle;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(vertAlign)
            ELSE_TRY_READ_IF(sz)
            ELSE_TRY_READ_IF(rFont)
            ELSE_TRY_READ_IF(color)
//! @todo add ELSE_WRONG_FORMAT
        }
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
 - [elsewhere] font (§18.8.22)
 - [done] rPr (§18.4.7)

 No child elements.

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_vertAlign()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
    ST_VerticalAlignRun vertAlign(val);
    vertAlign.setupCharacterStyle(m_currentTextStyleProperties);
//! @todo more QTextCharFormat::Align* styles?

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sz
//! fontSize
/*
 Parent elements:
 - [elsewhere] font (§18.8.22);
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_sz()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

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
 - [elsewhere] font (§18.8.22);
 - [done] rPr (§18.4.7)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_rFont()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    if (!val.isEmpty()) {
        m_currentTextStyle.addProperty("fo:font-family", val);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL color
//! color
/*
 Parent elements:
 - [elsewhere] font (§18.8.22);
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

    QColor currentColor;

    if (!indexed.isEmpty()) {
        int index = indexed.toInt();
        if (index >= 0 && index < 64) {
            currentColor = QString("#%1").arg(m_colorIndices.at(index));
        }
    }
    if (!rgb.isEmpty()) {

    }
    if (!theme.isEmpty()) {

    }
    if (!tint.isEmpty()) {

    }

    if (currentColor.isValid()) {
        m_currentTextStyle.addProperty("fo:color", currentColor.name());
    }

    readNext();
    READ_EPILOGUE
}
