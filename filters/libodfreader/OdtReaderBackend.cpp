/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdtReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"

// ================================================================
//             class OdtReaderBackend::Private

class Q_DECL_HIDDEN OdtReaderBackend::Private
{
public:
    Private();
    ~Private();

    bool dummy; // We don't have any actual content in
                // this class yet but it's still
                // needed for forward binary compatibility.
};

OdtReaderBackend::Private::Private() = default;

OdtReaderBackend::Private::~Private() = default;

// ================================================================
//                 class OdtReaderBackend

OdtReaderBackend::OdtReaderBackend()
    : d(new OdtReaderBackend::Private)
{
}

OdtReaderBackend::~OdtReaderBackend()
{
    delete d;
}

// ----------------------------------------------------------------
//                 ODT document level functions

IMPLEMENT_BACKEND_FUNCTION(OdtReader, OfficeText)
