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

#include "XlsxXmlDocumentReader.h"
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
    const XlsxSharedStringVector& _sharedStrings,
    const XlsxComments& _comments,
    const XlsxStyles& _styles,
    MSOOXML::MsooXmlRelationships& _relationships
    )
        : MSOOXML::MsooXmlReaderContext(&_relationships)
        , import(&_import)
        , themes(_themes)
        , sharedStrings(&_sharedStrings)
        , comments(&_comments)
        , styles(&_styles)
        , numberOfOleObjects(0)
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
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // workbook
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("workbook")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }
    /*
        const QXmlStreamAttributes attrs( attributes() );
        for (int i=0; i<attrs.count(); i++) {
            kDebug() << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
        }*/

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(QString(), MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::spreadsheetml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(workbook)

//! @todo hardcoded font face list; look at fonts used by theme
    mainStyles->insertFontFace(KoFontFace("Calibri"));
    mainStyles->insertFontFace(KoFontFace("Arial"));
    mainStyles->insertFontFace(KoFontFace("Tahoma"));

    kDebug() << "===========finished============";
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
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(sheets)
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

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(sheet)
            ELSE_WRONG_FORMAT
        }
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
    kDebug() << "r:id:" << r_id << "sheetId:" << sheetId << "name:" << name << "state:" << state;
//! @todo    TRY_READ_ATTR_WITHOUT_NS(state)

//! @todo implement MsooXmlRelationships with internal MsooXmlRelationshipsReader
//!       (for now we hardcode relationships, e.g. we use sheet1, sheet2...)
    d->worksheetNumber++; // counted from 1
    QString path = QString("xl/worksheets");
    QString file = QString("sheet%1.xml").arg(d->worksheetNumber);
    QString filepath = path + "/" + file;
    kDebug() << "path:" << path << "file:" << file;

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
    XlsxXmlWorksheetReaderContext context(d->worksheetNumber, name, state, path, file,
                                          m_context->themes, *m_context->sharedStrings,
                                          *m_context->comments,
                                          *m_context->styles,
                                          *m_context->relationships, m_context->import,
                                          m_context->numberOfOleObjects,
                                          vmlreader.content());
    const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(
                &worksheetReader, filepath, &context);
    if (result != KoFilter::OK) {
        raiseError(worksheetReader.errorString());
        return result;
    }

    readNext();
    READ_EPILOGUE
}
