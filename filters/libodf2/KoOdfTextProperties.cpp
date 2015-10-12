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
#include "KoOdfTextProperties.h"

// Qt
#include <QString>

// odflib
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h>

#include "Odf2Debug.h"

// ----------------------------------------------------------------
//                         private class


class Q_DECL_HIDDEN KoOdfTextProperties::Private
{
public:
    Private() {};
    ~Private() {};

    int dummy;                          // This class is currently empty
};


// ----------------------------------------------------------------


KoOdfTextProperties::KoOdfTextProperties()
    : KoOdfStyleProperties()
    , d(new KoOdfTextProperties::Private())
{
}

KoOdfTextProperties::~KoOdfTextProperties()
{
    delete d;
}


void KoOdfTextProperties::clear()
{
    KoOdfStyleProperties::clear();
}


bool KoOdfTextProperties::readOdf(KoXmlStreamReader &reader)
{
    // The element text-properties has no children.
    bool retval = readAttributes(reader);
    reader.skipCurrentElement();

    return retval;
}

bool KoOdfTextProperties::saveOdf(const QString &propertySet, KoXmlWriter *writer)
{
    Q_UNUSED(propertySet);

    writer->startElement("style:text-properties");
    saveAttributes(writer);
    writer->endElement(); // style:text-properties

    return true;
}
