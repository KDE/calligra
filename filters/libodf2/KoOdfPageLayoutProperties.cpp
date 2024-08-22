/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2014 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KoOdfPageLayoutProperties.h"

// Qt
#include <QString>

// odflib
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h>

#include "Odf2Debug.h"

// ----------------------------------------------------------------
//                         private class

class Q_DECL_HIDDEN KoOdfPageLayoutProperties::Private
{
public:
    Private() = default;
    ~Private() = default;

    // NYI: Background Image
    // NYI: Columns
    // NYI: Footnote sep
};

// ----------------------------------------------------------------

KoOdfPageLayoutProperties::KoOdfPageLayoutProperties()
    : KoOdfStyleProperties()
    , d(new KoOdfPageLayoutProperties::Private())
{
}

KoOdfPageLayoutProperties::~KoOdfPageLayoutProperties()
{
    delete d;
}

void KoOdfPageLayoutProperties::clear()
{
    KoOdfStyleProperties::clear();
}

bool KoOdfPageLayoutProperties::readOdf(KoXmlStreamReader &reader)
{
    bool retval = readAttributes(reader);
    if (!retval) {
        return false;
    }

    // Load child elements.  For page-layout-properties, these are:
    //  - style:background-image
    //  - style:columns
    //  - text:footnote-sep
    while (reader.readNextStartElement()) {
        QString child = reader.qualifiedName().toString();

        if (child == "style:background-image") {
            // FIXME: NYI
        } else if (child == "style:columns") {
            // FIXME: NYI
        } else if (child == "text:footnote-sep") {
            // FIXME: NYI
        }

        // Skip rest of each element including children that are not read yet (shouldn't be any).
        reader.skipCurrentElement();
    }

    return retval;
}

bool KoOdfPageLayoutProperties::saveOdf(const QString &propertySet, KoXmlWriter *writer)
{
    Q_UNUSED(propertySet);

    writer->startElement("style:page-layout-properties");
    saveAttributes(writer);

    // Save child elements of style:page-layout-properties
    // FIXME NYI: style:background-image
    // FIXME NYI: style:columns
    // FIXME NYI: text:footnote-sep

    writer->endElement(); // style:page-layout-properties

    return true;
}
