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
//                         Dr3d functions


void OdfDrawReaderBackend::elementDr3dScene(KoXmlStreamReader &reader,
					    OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}



// ----------------------------------------------------------------
//                         Draw functions


void OdfDrawReaderBackend::elementDrawA(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawCircle(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


// ----------------------------------------------------------------
//                             Frames


void OdfDrawReaderBackend::elementDrawFrame(KoXmlStreamReader &reader,
					    OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawObject(KoXmlStreamReader &reader,
					     OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfDrawReaderBackend::elementDrawObjectOle(KoXmlStreamReader &reader,
						OdfReaderContext *context)
{
    Q_UNUSED(reader);
    Q_UNUSED(context);
}
