/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

    bool dummy; // We don't have any actual content in
                // this class yet but it's still
                // needed for forward binary compatibility.
};

OdsReaderBackend::Private::Private() = default;

OdsReaderBackend::Private::~Private() = default;

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
