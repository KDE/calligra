/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

    bool dummy; // We don't have any actual content in
                // this class yet but it's still
                // needed for forward binary compatibility.
};

OdfReaderBackend::Private::Private() = default;

OdfReaderBackend::Private::~Private() = default;

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
