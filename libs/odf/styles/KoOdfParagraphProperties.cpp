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
#include "KoOdfParagraphProperties.h"

// Qt
#include <QString>

// KDE
#include <kdebug.h>

// odflib
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h>


// ----------------------------------------------------------------
//                         private class


class KoOdfParagraphProperties::Private
{
public:
    Private();
    ~Private();

    // Background Image
    KoOdfStyleDropCap         *dropCap;
    QList<KoOdfStyleTabStop*>  tabStops;
};

KoOdfParagraphProperties::Private::Private()
    : dropCap(0)
{
}

KoOdfParagraphProperties::Private::~Private()
{
    if (dropCap) {
        delete dropCap;
    }
    qDeleteAll(tabStops);
}


// ----------------------------------------------------------------


KoOdfParagraphProperties::KoOdfParagraphProperties()
    : KoOdfStyleProperties()
    , d(new KoOdfParagraphProperties::Private())
{
}

KoOdfParagraphProperties::~KoOdfParagraphProperties()
{
    delete d;
}


void KoOdfParagraphProperties::clear()
{
    KoOdfStyleProperties::clear();
}


bool KoOdfParagraphProperties::readOdf(KoXmlStreamReader &reader)
{
    // The element paragraph-properties has no children.
    bool retval = readAttributes(reader);

    // Load child elements.  For paragraph-properties, these are:
    //  - style:background-image
    //  - style:drop-cap
    //  - style:tab-stops
    while (reader.readNextStartElement()) {
        QString child = reader.qualifiedName().toString();

        if (child == "style:background-image") {
            // FIXME: Implement background image
        }
        else if (child == "style:drop-cap") {
            if (d->dropCap) {
                d->dropCap->attributes.clear();
            }
            else {
                d->dropCap = new KoOdfStyleDropCap();
            }
            copyAttributes(reader, d->dropCap->attributes);
        }
        else if (child == "style:tab-stops") {
            while (reader.readNextStartElement()) {
                if (reader.qualifiedName() == "style:tab-stop") {
                    KoOdfStyleTabStop  *tabStop = new KoOdfStyleTabStop;

                    copyAttributes(reader, tabStop->attributes);
                    d->tabStops.append(tabStop);
                }
            }
        }

        // Skip rest of each element including children that are not read yet (shouldn't be any).
        reader.skipCurrentElement();
    }

    return retval;
}

bool KoOdfParagraphProperties::saveOdf(const QString &propertySet, KoXmlWriter *writer)
{
    Q_UNUSED(propertySet);

    writer->startElement("style:paragraph-properties");
    saveAttributes(writer);

    // Save child elements of style:paragraph-properties
    // FIXME: Save background image here
    if (d->dropCap) {
        writer->startElement("style:drop-cap");
        ::saveAttributes(d->dropCap->attributes, writer);
        writer->endElement(); // style:drop-cap
    }
    if (!d->tabStops.isEmpty()) {
        writer->startElement("style:tab-stops");
        foreach (KoOdfStyleTabStop *tabStop, d->tabStops) {
            writer->startElement("style:tab-stop");
            ::saveAttributes(tabStop->attributes, writer);
            writer->endElement(); // style:tab-stop
        }
        writer->endElement(); // style:tab-stops
    }

    writer->endElement(); // style:paragraph-properties

    return true;
}
