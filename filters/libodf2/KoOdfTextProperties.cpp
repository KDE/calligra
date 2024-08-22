/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    Private() = default;
    ~Private() = default;

    int dummy; // This class is currently empty
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
