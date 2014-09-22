/* This file is part of the KDE project

   Copyright (C) 2013-2014 Inge Wallin       <inge@lysator.liu.se>

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
#include "OdfDrawReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


// ================================================================
//             class OdfDrawReaderBackend::Private


class OdfDrawReaderBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in
                                // this class yet but it's still
                                // needed for forward binary compatibility.
};

OdfDrawReaderBackend::Private::Private()
{
}

OdfDrawReaderBackend::Private::~Private()
{
}


// ================================================================
//                 class OdfDrawReaderBackend


OdfDrawReaderBackend::OdfDrawReaderBackend()
    : d(new OdfDrawReaderBackend::Private)
{
}

OdfDrawReaderBackend::~OdfDrawReaderBackend()
{
    delete d;
}


// ----------------------------------------------------------------
// Draw functions: circles, rectangles, etc


void OdfDrawReaderBackend::elementDr3dScene(KoXmlStreamReader &reader,
					    OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

#if 0
void OdfDrawReaderBackend::elementOfficeAnnotation(KoXmlStreamReader &reader,
                                                   OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementOfficeAnnotationEnd(KoXmlStreamReader &reader,
                                                      OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDcCreator(KoXmlStreamReader &reader,
                                            OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDcDate(KoXmlStreamReader &reader,
                                         OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawP(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawList(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementTableTable(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementTableTableColumn(KoXmlStreamReader &reader,
                                                   OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementTableTableRow(KoXmlStreamReader &reader,
                                                OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementTableTableHeaderRows(KoXmlStreamReader &reader,
                                                       OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementTableTableCell(KoXmlStreamReader &reader,
                                                 OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementTableCoveredTableCell(KoXmlStreamReader &reader,
                                                        OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, draw content itself, etc.


void OdfDrawReaderBackend::elementDrawA(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawLineBreak(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawSpan(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawS(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


void OdfDrawReaderBackend::elementDrawSoftPageBreak(KoXmlStreamReader &reader,
                                                    OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


void OdfDrawReaderBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

// ----------------------------------------------------------------
// List level functions: lit-header and list-item.

void OdfDrawReaderBackend::elementDrawListHeader(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawListItem(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}
#endif
