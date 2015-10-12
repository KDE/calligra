/* This file is part of the KDE project
 *
 * Copyright (C) 2014 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
    Private() {};
    ~Private() {};

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
        }
        else if (child == "style:columns") {
            // FIXME: NYI
        }
        else if (child == "text:footnote-sep") {
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
