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

// Own
#include "XlsxUtils.h"
#include "XlsxXmlDocumentReader.h"

#include <KoBorder.h>  // needed by DrawingMLMethodso

#include "XlsxXmlWorksheetReader.h"
#include "XlsxXmlCommentsReader.h"
#include "XlsxImport.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <KoXmlWriter.h>
#include <KoFontFace.h>
#include <VmlDrawingReader.h>

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlDocumentReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

XlsxXmlDocumentReaderContext::XlsxXmlDocumentReaderContext(
    XlsxImport& _import,
    MSOOXML::DrawingMLTheme* _themes,
    const QVector<QString>& _sharedStrings,
    const XlsxComments& _comments,
    const XlsxStyles& _styles,
    MSOOXML::MsooXmlRelationships& _relationships,
    const QString &_file,
    const QString &_path
    )
        : MSOOXML::MsooXmlReaderContext(&_relationships)
        , import(&_import)
        , themes(_themes)
        , sharedStrings(&_sharedStrings)
        , comments(&_comments)
        , styles(&_styles)
        , file(_file)
        , path(_path)
{
}

class XlsxXmlDocumentReader::Private
{
public:
    Private()
            : worksheetNumber(0) {
    }
    ~Private() {
    }
    uint worksheetNumber;
private:
};

XlsxXmlDocumentReader::XlsxXmlDocumentReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

XlsxXmlDocumentReader::~XlsxXmlDocumentReader()
{
    delete d;
}

void XlsxXmlDocumentReader::init()
{
    m_defaultNamespace = "";
}

KoFilter::ConversionStatus XlsxXmlDocumentReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlDocumentReaderContext*>(context);
    Q_ASSERT(m_context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus XlsxXmlDocumentReader::readInternal()
{
    qCDebug(lcXlsxImport) << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // workbook
    readNext();
    qCDebug(lcXlsxImport) << *this << namespaceUri();

    if (!expectEl("workbook")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }
    /*
        const QXmlStreamAttributes attrs( attributes() );
        for (int i=0; i<attrs.count(); i++) {
            qCDebug(lcXlsxImport) << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
        }*/

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

    TRY_READ(workbook)

//! @todo hardcoded font face list; look at fonts used by theme
    mainStyles->insertFontFace(KoFontFace("Calibri"));
    mainStyles->insertFontFace(KoFontFace("Arial"));
    mainStyles->insertFontFace(KoFontFace("Tahoma"));

    qCDebug(lcXlsxImport) << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL workbook
//! workbook handler (Workbook)
/*! ECMA-376, 18.2.27, p. 1746.
 Root element.

 Child elements:
 - bookViews (Workbook Views) §18.2.1
 - calcPr (Calculation Properties) §18.2.2
 - customWorkbookViews (Custom Workbook Views) §18.2.4
 - definedNames (Defined Names) §18.2.6
 - externalReferences (External References) §18.2.9
 - extLst (Future Feature Data Storage Area) §18.2.10
 - fileRecoveryPr (File Recovery Properties) §18.2.11
 - fileSharing (File Sharing) §18.2.12
 - fileVersion (File Version) §18.2.13
 - functionGroups (Function Groups) §18.2.15
 - oleSize (Embedded Object Size) §18.2.16
 - pivotCaches (PivotCaches) §18.2.18
 - [done] sheets (Sheets) §18.2.20
 - smartTagPr (Smart Tag Properties) §18.2.21
 - smartTagTypes (Smart Tag Types) §18.2.23
 - webPublishing (Web Publishing Properties) §18.2.24
 - webPublishObjects (Web Publish Objects) §18.2.26
 - workbookPr (Workbook Properties) §18.2.28
 - workbookProtection (Workbook Protection) §18.2.29

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlDocumentReader::read_workbook()
{
    READ_PROLOGUE

    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        qCDebug(lcXlsxImport) << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(sheets)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sheets
//! sheets handler (Sheets)
/*! ECMA-376, 18.2.20, p. 1740.
 This element represents the collection of sheets in the workbook.

 Parent elements:
 - [done] workbook (§18.2.27)

 Child elements:
 - [done] sheet (Sheet Information) §18.2.19
*/
KoFilter::ConversionStatus XlsxXmlDocumentReader::read_sheets()
{
    READ_PROLOGUE

    unsigned numberOfWorkSheets = m_context->relationships->targetCountWithWord("worksheets") +
        m_context->relationships->targetCountWithWord("dialogsheets") +
        m_context->relationships->targetCountWithWord("chartsheets");
    unsigned worksheet = 1;

    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "sheet") {
                TRY_READ(sheet)
                ++worksheet;
                m_context->import->reportProgress(45 + (55/numberOfWorkSheets) * worksheet);
            }
            ELSE_WRONG_FORMAT
        }
    }

    if (!m_context->autoFilters.isEmpty()) {
        body->startElement("table:database-ranges");
        int index = 0;
        while (index < m_context->autoFilters.size()) {
            body->startElement("table:database-range");
            body->addAttribute("table:target-range-address", m_context->autoFilters.at(index).area);
            body->addAttribute("table:display-filter-buttons", "true");
            body->addAttribute("table:name", QString("excel-database-%1").arg(index));
            QString type = m_context->autoFilters.at(index).type;
            int filterConditionSize = m_context->autoFilters.at(index).filterConditions.size();
            if (filterConditionSize > 0) {
                if (type == "and") {
                    body->startElement("table:filter-and");
                }
                else if (type == "or") {
                    body->startElement("table:filter-or");
                }
                else {
                    body->startElement("table:filter");
                }
                int conditionIndex = 0;
                while (conditionIndex < filterConditionSize) {
                    body->startElement("table:filter-condition");
                    body->addAttribute("table:field-number", m_context->autoFilters.at(index).filterConditions.at(conditionIndex).field);
                    body->addAttribute("table:value", m_context->autoFilters.at(index).filterConditions.at(conditionIndex).value);
                    body->addAttribute("table:operator", m_context->autoFilters.at(index).filterConditions.at(conditionIndex).opField);
                    body->endElement(); // table:filter-condition
                    ++conditionIndex;
                }
                body->endElement(); // table:filter | table:filter-or | table:filter-and
            }
            body->endElement(); // table:database-range
            ++index;
        }

        body->endElement(); // table:database-ranges
    }


    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sheet
//! sheet handler (Sheet Information)
/*! ECMA-376, 18.2.19, p. 1740.
 This element defines a sheet in this workbook. Sheet data is stored in a separate part.

 Parent elements:
 - [done] sheets (§18.2.20)

 No child elements.
*/
KoFilter::ConversionStatus XlsxXmlDocumentReader::read_sheet()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITH_NS(r, id)
    READ_ATTR_WITHOUT_NS(sheetId)
    READ_ATTR_WITHOUT_NS(name)
    TRY_READ_ATTR_WITHOUT_NS(state)
    qCDebug(lcXlsxImport) << "r:id:" << r_id << "sheetId:" << sheetId << "name:" << name << "state:" << state;

    unsigned numberOfWorkSheets = m_context->relationships->targetCountWithWord("worksheets") +
        m_context->relationships->targetCountWithWord("dialogsheets") +
        m_context->relationships->targetCountWithWord("chartsheets");
    d->worksheetNumber++; // counted from 1
    QString path, file;
    QString filepath = m_context->relationships->target(m_context->path, m_context->file, r_id);
    MSOOXML::Utils::splitPathAndFile(filepath, &path, &file);
    qCDebug(lcXlsxImport) << "path:" << path << "file:" << file;

    // Loading potential ole replacements
    VmlDrawingReader vmlreader(this);
    QString vmlTarget = m_context->relationships->targetForType(path, file,
        "http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing");

    if (!vmlTarget.isEmpty()) {
        QString errorMessage, vmlPath, vmlFile;

        MSOOXML::Utils::splitPathAndFile(vmlTarget, &vmlPath, &vmlFile);

        VmlDrawingReaderContext vmlContext(*m_context->import,
            vmlPath, vmlFile, *m_context->relationships);

        const KoFilter::ConversionStatus status =
            m_context->import->loadAndParseDocument(&vmlreader, vmlTarget, errorMessage, &vmlContext);
        if (status != KoFilter::OK) {
            vmlreader.raiseError(errorMessage);
        }
    }

    XlsxXmlWorksheetReader worksheetReader(this);
    XlsxXmlWorksheetReaderContext context(d->worksheetNumber, numberOfWorkSheets, name, state, path, file,
                                          m_context->themes, *m_context->sharedStrings,
                                          *m_context->comments,
                                          *m_context->styles,
                                          *m_context->relationships, m_context->import,
                                          vmlreader.content(),
                                          vmlreader.frames(),
                                          m_context->autoFilters);
    // Due to some information being available only in the later part of the document, we have to read twice
    // In the first round we get the later information and in 2nd round we read the rest and use the information
    context.firstRoundOfReading = true;
    KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(&worksheetReader, filepath, &context);
    if (status != KoFilter::OK) {
        raiseError(worksheetReader.errorString());
        return status;
    }
    context.firstRoundOfReading = false;
    status = m_context->import->loadAndParseDocument(&worksheetReader, filepath, &context);
    if (status != KoFilter::OK) {
        raiseError(worksheetReader.errorString());
        return status;
    }

    readNext();
    READ_EPILOGUE
}
