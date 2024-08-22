/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "XlsxXmlTableReader.h"
#include "XlsxUtils.h"

#define MSOOXML_CURRENT_CLASS XlsxXmlTableReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>

XlsxXmlTableReaderContext::XlsxXmlTableReaderContext()
    : headerStyleIndex(-1)
    , dataStyleIndex(-1)
    , totalsRowIndex(-1)
    , headerRowCount(1)
    , totalsRowCount(1)
{
}

XlsxXmlTableReader::XlsxXmlTableReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_context(nullptr)
{
}

XlsxXmlTableReader::~XlsxXmlTableReader() = default;

KoFilter::ConversionStatus XlsxXmlTableReader::read(MSOOXML::MsooXmlReaderContext *context)
{
    m_context = dynamic_cast<XlsxXmlTableReaderContext *>(context);
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    qCDebug(lcXlsxImport) << *this << namespaceUri();

    if (!expectEl("table")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        qCDebug(lcXlsxImport) << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
    //! @todo find out whether the namespace returned by namespaceUri()
    //!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(QString(), MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::spreadsheetml)));
        return KoFilter::WrongFormat;
    }
    //! @todo expect other namespaces too...

    TRY_READ(table)

    qCDebug(lcXlsxImport) << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL table
/*
 Parent elements:
 - [done] root element

 Child elements:
 - autoFilter (AutoFilter Settings) §18.3.1.2
 - extLst (Future Feature Data Storage Area) §18.2.10
 - sortState (Sort State) §18.3.1.92
 - tableColumns (Table Columns) §18.5.1.4
 - tableStyleInfo (Table Style) §18.5.1.5

*/
KoFilter::ConversionStatus XlsxXmlTableReader::read_table()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(ref)
    TRY_READ_ATTR_WITHOUT_NS(headerRowDxfId)
    TRY_READ_ATTR_WITHOUT_NS(dataDxfId)
    TRY_READ_ATTR_WITHOUT_NS(totalsRowDxfId)
    TRY_READ_ATTR_WITHOUT_NS(totalsRowCount)
    TRY_READ_ATTR_WITHOUT_NS(headerRowCount)

    m_context->referenceArea = ref;
    m_context->headerStyleIndex = headerRowDxfId.toInt();
    m_context->dataStyleIndex = dataDxfId.toInt();
    m_context->totalsRowIndex = totalsRowDxfId.toInt();

    if (!totalsRowCount.isEmpty()) {
        m_context->totalsRowCount = totalsRowCount.toInt();
    }
    if (!headerRowCount.isEmpty()) {
        m_context->headerRowCount = headerRowCount.toInt();
    }

    // TODO: Read all, it should be possible to reference the table with formulas from
    // worksheets maybe through the context

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) { }
    }
    READ_EPILOGUE
}
