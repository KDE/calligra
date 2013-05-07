/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
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
#include "KoOdfGraphicProperties.h"

// Qt
#include <QString>

// KDE
#include <kdebug.h>

// odflib
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h>


// ----------------------------------------------------------------
//                         private class


class KoOdfGraphicProperties::Private
{
public:
    Private() {};
    ~Private() {};

    // Background Image
    // Columns
    // List Style
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
    // The element graphic-properties has no children.
    bool retval = readAttributes(reader);

    // Load child elements.  For graphic-properties, these are:
    //  - style:background-image
    //  - style:columns
    //  - text:list-style
    while (reader.readNextStartElement()) {
        QString child = reader.qualifiedName().toString();

        if (child == "style:background-image") {
            reader.skipCurrentElement();
        }
        else if (child == "style:columns") {
            reader.skipCurrentElement();
        }
        else if (child == "text:list-style") {
            reader.skipCurrentElement();
        }
    }

    return retval;
}

bool KoOdfGraphicProperties::saveOdf(const QString &propertySet, KoXmlWriter *writer)
{
    Q_UNUSED(propertySet);

    writer->startElement("style:graphic-properties");
    saveAttributes(writer);
    writer->endElement(); // style:graphic-properties

    return true;
}
