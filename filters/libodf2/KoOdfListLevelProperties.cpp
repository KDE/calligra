/* This file is part of the KDE project
  *
  * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
  * Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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
#include "KoOdfListLevelProperties.h"

// Qt
#include <QString>

// odflib
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h>

#include "Odf2Debug.h"

// ----------------------------------------------------------------
//                         private class


class Q_DECL_HIDDEN KoOdfListLevelProperties::Private
{
public:
    Private()
        :hasLabelAlignment(false){};
    ~Private() {};

    bool hasLabelAlignment;
    AttributeSet propertiesAttributes;  // name, value
    AttributeSet labelAlignmentAttributes;  // name, value
};


// ----------------------------------------------------------------


KoOdfListLevelProperties::KoOdfListLevelProperties()
    : KoOdfStyleProperties()
    , d(new KoOdfListLevelProperties::Private())
{
}

KoOdfListLevelProperties::~KoOdfListLevelProperties()
{
    delete d;
}


void KoOdfListLevelProperties::clear()
{
    KoOdfStyleProperties::clear();
}


bool KoOdfListLevelProperties::readOdf(KoXmlStreamReader &reader)
{
    //FIXME: Handle child element
    bool retval = readAttributes(reader);

    KoXmlStreamAttributes attrs = reader.attributes();
    foreach (const KoXmlStreamAttribute &attr, attrs) {
        d->propertiesAttributes.insert(attr.qualifiedName().toString(), attr.value().toString());
    }
    debugOdf2 << "level properties attributes:" << d->propertiesAttributes;

    while (reader.readNextStartElement()) {
        QString child = reader.qualifiedName().toString();

        // style:list-level-label-alignment just is uesd in this place.
        // Save the list-level-label-alignment and list-level-properties in separate Attributeset for saveOdf.
        if (child == "style:list-level-label-alignment") {
            d->hasLabelAlignment = true;
            // FIXME: Should create a class for  this element or not.
            retval = readAttributes(reader);

            KoXmlStreamAttributes attrs = reader.attributes();
            foreach (const KoXmlStreamAttribute &attr, attrs) {
                d->labelAlignmentAttributes.insert(attr.qualifiedName().toString(), attr.value().toString());
            }
            debugOdf2 << "Label alignment attributes:" << d->labelAlignmentAttributes;
        }
    }
    reader.skipCurrentElement();
    return retval;
}

bool KoOdfListLevelProperties::saveOdf(const QString &propertySet, KoXmlWriter *writer)
{
    Q_UNUSED(propertySet);

    writer->startElement("style:list-level-properties");
    ::saveAttributes(d->propertiesAttributes, writer);
    if (d->hasLabelAlignment) {
        writer->startElement("style:list-level-label-alignment");
        ::saveAttributes(d->labelAlignmentAttributes, writer);
        writer->endElement();
    }
    writer->endElement(); // style:text-properties

    return true;
}
