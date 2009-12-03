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

#include "XlsxXmlSharedStringsReader.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlSharedStringsReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

XlsxXmlSharedStringsReaderContext::XlsxXmlSharedStringsReaderContext(QVector<QString>& _strings)
    : strings(&_strings)
{
}

class XlsxXmlSharedStringsReader::Private {
public:
    Private()
    {
    }
    ~Private()
    {
    }
private:
};

XlsxXmlSharedStringsReader::XlsxXmlSharedStringsReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlReader(writers)
    , m_context(0)
    , d(new Private)
{
    init();
}

XlsxXmlSharedStringsReader::~XlsxXmlSharedStringsReader()
{
    delete d;
}

void XlsxXmlSharedStringsReader::init()
{
    m_defaultNamespace = "";
    m_index = 0;
}

KoFilter::ConversionStatus XlsxXmlSharedStringsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlSharedStringsReaderContext*>(context);
    Q_ASSERT(m_context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus XlsxXmlSharedStringsReader::readInternal()
{
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // sst
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("sst")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces( namespaceDeclarations() );
    for (int i=0; i<namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains( QXmlStreamNamespaceDeclaration( QString(), MSOOXML::Schemas::spreadsheetml ) )) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::spreadsheetml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(sst)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL sst
//! workbook handler (Shared String Table)
/*! ECMA-376, 18.4.9, p. 1912.
 Root element.
*/
KoFilter::ConversionStatus XlsxXmlSharedStringsReader::read_sst()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(count)
//! @todo use uniqueCount attr?
//    TRY_READ_ATTR_WITHOUT_NS(uniqueCount)
    bool ok;
    const uint countNumber = count.toUInt(&ok);
    if (!ok) {
        raiseUnexpectedAttributeValueError(count, "sst@count");
        return KoFilter::WrongFormat;
    }
    m_context->strings->resize(countNumber);
    m_index = 0;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(si)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL si
//! si handler (String Item)
/*! ECMA-376, 18.2.20, p. 1740.
 This element is the representation of an individual string in the Shared String table.

 Child elements:
 - phoneticPr (Phonetic Properties) §18.4.3
 - r (Rich Text Run) §18.4.4
 - rPh (Phonetic Run) §18.4.6
 - [done] t (Text) §18.4.12
 Parent elements:
 - [done] sst (§18.4.9)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlSharedStringsReader::read_si()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(t)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL t
//! t handler (Text)
/*! ECMA-376, 18.4.12, p. 1914.
 This element represents the text content shown as part of a string.

 No child elements.
 Parent elements:
 - is (§18.3.1.53)
 - r (§18.4.4)
 - rPh (§18.4.6)
 - [done] si (§18.4.8)
 - text (§18.7.7)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlSharedStringsReader::read_t()
{
    READ_PROLOGUE
    readNext();

    kDebug() << "#" << m_index << text().toString();
    if (m_index >= (uint)m_context->strings->size()) {
        raiseError(i18n("Declared number of shared strings too small (%1)", m_context->strings->size()));
        return KoFilter::WrongFormat;
    }

    (*m_context->strings)[m_index] = text().toString();
    m_index++;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
