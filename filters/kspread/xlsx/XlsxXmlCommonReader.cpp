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

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

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
        readNext();
        kDebug() << *this;
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

 No child elements.
 Parent elements:
 - is (§18.3.1.53)
 - [done] si (§18.4.8)
 - text (§18.7.7)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlCommonReader::read_r()
{
    READ_PROLOGUE

    QString readResult;

    while (!atEnd()) {
        if (QUALIFIED_NAME_IS(t)) {
            TRY_READ(t)
//! @todo
kDebug() << "readResult += m_text" << readResult << m_text;
            readResult += m_text;
        }
//! @todo support rPr
//! @todo            ELSE_WRONG_FORMAT
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    m_text = readResult;

    READ_EPILOGUE
}
