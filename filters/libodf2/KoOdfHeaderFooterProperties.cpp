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
    Private() {};
    ~Private() {};

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
