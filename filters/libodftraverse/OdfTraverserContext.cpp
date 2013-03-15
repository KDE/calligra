/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin            <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


// Own
#include "OdfTraverserContext.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


// ----------------------------------------------------------------
//                 class OdfTraverserContext::Private


class OdfTraverserContext::Private
{
public:
    Private(KoStore *store);
    ~Private();

    KoStore *odfStore;

    // This data is used for conversion while traversing the content tree.
    // It's created from the store that is given to us at construction time.
    QHash<QString, QString>    metadata;
    QHash<QString, QString>    manifest;

    // This data is created during the traversal and can be used after
    // it is finished.
    QHash<QString, QSizeF>   images;
    QHash<QString, QString>  mediaFiles;
};


OdfTraverserContext::Private::Private(KoStore *store)
    : odfStore(store)
{
}

OdfTraverserContext::Private::~Private()
{
}


// ----------------------------------------------------------------
//                     class OdfTraverserContext


OdfTraverserContext::OdfTraverserContext(KoStore *store)
    : d(new OdfTraverserContext::Private(store))
{
}

OdfTraverserContext::~OdfTraverserContext()
{
}


KoFilter::ConversionStatus OdfTraverserContext::analyzeOdfFile()
{
    if (!d->odfStore) {
        return KoFilter::FileNotFound;
    }

    // ----------------------------------------------------------------
    // Parse input files

    OdfParser odfParser;
    KoFilter::ConversionStatus  status;

    // Parse meta.xml into m_metadata
    status = odfParser.parseMetadata(d->odfStore, d->metadata);
    if (status != KoFilter::OK) {
        return status;
    }

    // Parse manifest
    status = odfParser.parseManifest(d->odfStore, d->manifest);
    if (status != KoFilter::OK) {
        return status;
    }

    return KoFilter::OK;
}


KoStore *OdfTraverserContext::odfStore() const
{
    return d->odfStore;
}

QHash<QString, QString> OdfTraverserContext::metadata() const
{
    return d->metadata;
}

QHash<QString, QString> OdfTraverserContext::manifest() const
{
    return d->manifest;
}


QHash<QString, QSizeF> OdfTraverserContext::images() const
{
    return d->images;
}

QHash<QString, QString> OdfTraverserContext::mediaFiles() const
{
    return d->mediaFiles;
}
