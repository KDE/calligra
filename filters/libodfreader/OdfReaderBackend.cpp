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
#include "OdfReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


// ================================================================
//             class OdfReaderBackend::Private


class OdfReaderBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in
                                // this class yet but it's still
                                // needed for forward binary compatibility.
};

OdfReaderBackend::Private::Private()
{
}

OdfReaderBackend::Private::~Private()
{
}


// ================================================================
//                 class OdfReaderBackend


OdfReaderBackend::OdfReaderBackend()
    : d(new OdfReaderBackend::Private)
{
}

OdfReaderBackend::~OdfReaderBackend()
{
    delete d;
}


// ----------------------------------------------------------------
//                 ODF document level functions


IMPLEMENT_BACKEND_FUNCTION(OdfReader, OfficeDocumentcontent);
IMPLEMENT_BACKEND_FUNCTION(OdfReader, OfficeBody);
