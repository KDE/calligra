/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KoOdfGraphicProperties.h"

// Qt
#include <QString>

// odflib
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h>

#include "Odf2Debug.h"

// ----------------------------------------------------------------
//                         private class

class Q_DECL_HIDDEN KoOdfGraphicProperties::Private
{
public:
    Private() = default;
    ~Private() = default;

    // NYI: Background Image
    // NYI: Columns
    // NYI: List Style
};

// ----------------------------------------------------------------

KoOdfGraphicProperties::KoOdfGraphicProperties()
    : KoOdfStyleProperties()
    , d(new KoOdfGraphicProperties::Private())
{
}

KoOdfGraphicProperties::~KoOdfGraphicProperties()
{
    delete d;
}

void KoOdfGraphicProperties::clear()
{
    KoOdfStyleProperties::clear();
}

bool KoOdfGraphicProperties::readOdf(KoXmlStreamReader &reader)
{
    bool retval = readAttributes(reader);
    if (!retval) {
        return false;
    }

    // Load child elements.  For graphic-properties, these are:
    //  - style:background-image
    //  - style:columns
    //  - text:list-style
    while (reader.readNextStartElement()) {
        QString child = reader.qualifiedName().toString();

        if (child == "style:background-image") {
            // FIXME: NYI
        } else if (child == "style:columns") {
            // FIXME: NYI
        } else if (child == "text:list-style") {
            // FIXME: NYI
        }

        // Skip rest of each element including children that are not read yet (shouldn't be any).
        reader.skipCurrentElement();
    }

    return retval;
}

bool KoOdfGraphicProperties::saveOdf(const QString &propertySet, KoXmlWriter *writer)
{
    Q_UNUSED(propertySet);

    writer->startElement("style:graphic-properties");
    saveAttributes(writer);

    // Save child elements of style:graphic-properties
    // FIXME NYI: style:background-image
    // FIXME NYI: style:columns
    // FIXME NYI: text:list-style

    writer->endElement(); // style:graphic-properties

    return true;
}
