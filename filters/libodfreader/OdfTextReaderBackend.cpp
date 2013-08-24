/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin            <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


// Own
#include "OdfTextReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


// ================================================================
//             class OdfTextReaderBackend::Private


class OdfTextReaderBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in
                                // this class yet but it's still
                                // needed for forward binary compatibility.
};

OdfTextReaderBackend::Private::Private()
{
}

OdfTextReaderBackend::Private::~Private()
{
}


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

void OdfTextReaderBackend::elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


void OdfTextReaderBackend::elementTableTable(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::elementTableTableColumn(KoXmlStreamReader &reader,
                                                   OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::elementTableTableRow(KoXmlStreamReader &reader,
                                                OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::elementTableTableHeaderRows(KoXmlStreamReader &reader,
                                                       OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::elementTableTableCell(KoXmlStreamReader &reader,
                                                 OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, text content itself, etc.


void OdfTextReaderBackend::elementTextA(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderBackend::elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


void OdfTextReaderBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}
