/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2014 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KoOdfHeaderFooterProperties.h"

// Qt
#include <QString>

// odflib
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h>

#include "Odf2Debug.h"

// ----------------------------------------------------------------
//                         private class

class Q_DECL_HIDDEN KoOdfHeaderFooterProperties::Private
{
public:
    Private() = default;
    ~Private() = default;

    // NYI: Background Image
};

// ----------------------------------------------------------------

KoOdfHeaderFooterProperties::KoOdfHeaderFooterProperties()
    : KoOdfStyleProperties()
    , d(new KoOdfHeaderFooterProperties::Private())
{
}

KoOdfHeaderFooterProperties::~KoOdfHeaderFooterProperties()
{
    delete d;
}

void KoOdfHeaderFooterProperties::clear()
{
    KoOdfStyleProperties::clear();
}

bool KoOdfHeaderFooterProperties::readOdf(KoXmlStreamReader &reader)
{
    bool retval = readAttributes(reader);
    if (!retval) {
        return false;
    }

    // Load child elements.  For header-footer-properties, these are:
    //  - style:background-image
    while (reader.readNextStartElement()) {
        QString child = reader.qualifiedName().toString();

        if (child == "style:background-image") {
            // FIXME: NYI
        }

        // Skip rest of each element including children that are not read yet (shouldn't be any).
        reader.skipCurrentElement();
    }

    return retval;
}

bool KoOdfHeaderFooterProperties::saveOdf(const QString &propertySet, KoXmlWriter *writer)
{
    Q_UNUSED(propertySet);

    writer->startElement("style:header-footer-properties");
    saveAttributes(writer);

    // Save child elements of style:header-footer-properties
    // FIXME NYI: style:background-image

    writer->endElement(); // style:header-footer-properties

    return true;
}
