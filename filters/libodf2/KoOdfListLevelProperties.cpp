/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
        : hasLabelAlignment(false){};
    ~Private() = default;

    bool hasLabelAlignment;
    AttributeSet propertiesAttributes; // name, value
    AttributeSet labelAlignmentAttributes; // name, value
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
    // FIXME: Handle child element
    bool retval = readAttributes(reader);

    const KoXmlStreamAttributes attrs = reader.attributes();
    for (const KoXmlStreamAttribute &attr : attrs) {
        d->propertiesAttributes.insert(attr.qualifiedName().toString(), attr.value().toString());
    }
    debugOdf2 << "level properties attributes:" << d->propertiesAttributes;

    while (reader.readNextStartElement()) {
        QString child = reader.qualifiedName().toString();

        // style:list-level-label-alignment just is used in this place.
        // Save the list-level-label-alignment and list-level-properties in separate Attributeset for saveOdf.
        if (child == "style:list-level-label-alignment") {
            d->hasLabelAlignment = true;
            // FIXME: Should create a class for  this element or not.
            retval = readAttributes(reader);

            const KoXmlStreamAttributes attrs = reader.attributes();
            for (const KoXmlStreamAttribute &attr : attrs) {
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
