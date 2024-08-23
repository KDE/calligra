/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdfReaderContext.h"

// Calligra
#include <KoOdfStyleManager.h>
#include <KoXmlReader.h>

#include <QSizeF>

// Odftraverse library
#include "OdfParser.h"

// ----------------------------------------------------------------
//                 class OdfReaderContext::Private

class Q_DECL_HIDDEN OdfReaderContext::Private
{
public:
    Private(KoStore *store);
    ~Private();

    KoStore *odfStore;

    // This data is used for conversion while traversing the content tree.
    // It's created from the store that is given to us at construction time.
    QHash<QString, QString> metadata;
    QHash<QString, QString> manifest;
    KoOdfStyleManager *styleManager;

    // This data changes while the parsing proceeds.
    bool isInsideParagraph; // True while we are parsing paragraph contents.

    // This data is created during the reading and can be used after
    // it is finished.
    QHash<QString, QSizeF> images;
    QHash<QString, QString> mediaFiles;
};

OdfReaderContext::Private::Private(KoStore *store)
    : odfStore(store)
    , styleManager(new KoOdfStyleManager())
    , isInsideParagraph(false)
{
}

OdfReaderContext::Private::~Private()
{
    delete styleManager;
}

// ----------------------------------------------------------------
//                     class OdfReaderContext

OdfReaderContext::OdfReaderContext(KoStore *store)
    : d(new OdfReaderContext::Private(store))
{
}

OdfReaderContext::~OdfReaderContext()
{
    delete d;
}

KoFilter::ConversionStatus OdfReaderContext::analyzeOdfFile()
{
    if (!d->odfStore) {
        return KoFilter::FileNotFound;
    }

    // ----------------------------------------------------------------
    // Parse input files

    OdfParser odfParser;
    KoFilter::ConversionStatus status;

    // Parse meta.xml into m_metadata
    status = odfParser.parseMetadata(*d->odfStore, &d->metadata);
    if (status != KoFilter::OK) {
        return status;
    }

    // Parse manifest
    status = odfParser.parseManifest(*d->odfStore, &d->manifest);
    if (status != KoFilter::OK) {
        return status;
    }

    // Load the styles
    d->styleManager->loadStyles(d->odfStore);

    return KoFilter::OK;
}

KoStore *OdfReaderContext::odfStore() const
{
    return d->odfStore;
}

KoOdfStyleManager *OdfReaderContext::styleManager() const
{
    return d->styleManager;
}

QHash<QString, QString> OdfReaderContext::metadata() const
{
    return d->metadata;
}

QHash<QString, QString> OdfReaderContext::manifest() const
{
    return d->manifest;
}

bool OdfReaderContext::isInsideParagraph() const
{
    return d->isInsideParagraph;
}

void OdfReaderContext::setIsInsideParagraph(bool isInside)
{
    d->isInsideParagraph = isInside;
}

QHash<QString, QSizeF> OdfReaderContext::images() const
{
    return d->images;
}

#if 0 // NYI
QHash<QString, QString> OdfReaderContext::mediaFiles() const
{
    return d->mediaFiles;
}
#endif
