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
#include "OdsReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


// ================================================================
//             class OdsReaderBackend::Private


class OdsReaderBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in
                                // this class yet but it's still
                                // needed for forward binary compatibility.
};

OdsReaderBackend::Private::Private()
{
}

OdsReaderBackend::Private::~Private()
{
}


// ================================================================
//                 class OdsReaderBackend


OdsReaderBackend::OdsReaderBackend()
    : d(new OdsReaderBackend::Private)
{
}

OdsReaderBackend::~OdsReaderBackend()
{
    delete d;
}


// ----------------------------------------------------------------
//                 ODS document level functions


IMPLEMENT_BACKEND_FUNCTION(OdsReader, OfficeSpreadsheet)
