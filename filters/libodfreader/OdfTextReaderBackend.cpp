/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdfTextReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"

// ================================================================
//             class OdfTextReaderBackend::Private

class Q_DECL_HIDDEN OdfTextReaderBackend::Private
{
public:
    Private();
    ~Private();

    bool dummy; // We don't have any actual content in
                // this class yet but it's still
                // needed for forward binary compatibility.
};

OdfTextReaderBackend::Private::Private() = default;

OdfTextReaderBackend::Private::~Private() = default;

// ================================================================
//                 class OdfTextReaderBackend

OdfTextReaderBackend::OdfTextReaderBackend()
    : d(new OdfTextReaderBackend::Private)
{
}

OdfTextReaderBackend::~OdfTextReaderBackend()
{
    delete d;
}

// ----------------------------------------------------------------
// Text level functions: paragraphs, headings, sections, frames, objects, etc

IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, OfficeAnnotation)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, OfficeAnnotationEnd)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, DcCreator)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, DcDate)

IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextH)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextP)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextList)

IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTable)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableColumnGroup)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableColumn)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableRowGroup)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableRow)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableRows)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableColumns)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableHeaderColumns)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableHeaderRows)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableTableCell)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TableCoveredTableCell)

// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, text content itself, etc.

IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextA)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextLineBreak)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextSpan)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextS)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextSoftPageBreak)

// ----------------------------------------------------------------
// List level functions: lit-header and list-item.

IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextListHeader)
IMPLEMENT_BACKEND_FUNCTION(OdfTextReader, TextListItem)

// ----------------------------------------------------------------
// Some special functions

void OdfTextReaderBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::textVariable(const QString &name, const QString &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}
