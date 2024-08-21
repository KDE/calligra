/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "OpcRelSetManager.h"

// Qt
#include <QHash>
#include <QString>

// odf lib
#include "KoStore.h"
#include <KoXmlNS.h>
#include <KoXmlStreamReader.h>

// Filter libraries
#include "KoOdfStyle.h"

// This filter
#include "DocxExportDebug.h"
#include "OpcRelSet.h"

// ================================================================
//                 class OpcRelSetManager

class OpcRelSetManager::Private
{
public:
    Private();
    ~Private();

    QHash<QString, OpcRelSet *> relSets; // path, relations
    OpcRelSet *documentRelSet; // The relations for the whole document
                               // Stored in _rels/.rels
};

OpcRelSetManager::Private::Private()
    : documentRelSet(nullptr)
{
}

OpcRelSetManager::Private::~Private()
{
    qDeleteAll(relSets);
    delete documentRelSet;
}

// ----------------------------------------------------------------

OpcRelSetManager::OpcRelSetManager()
    : d(new OpcRelSetManager::Private())
{
}

OpcRelSetManager::~OpcRelSetManager()
{
    delete d;
}

OpcRelSet *OpcRelSetManager::relSet(const QString &path) const
{
    return d->relSets.value(path, 0);
}

void OpcRelSetManager::setRelSet(const QString &path, OpcRelSet *relSet)
{
    d->relSets.insert(path, relSet);
}

OpcRelSet *OpcRelSetManager::documentRelSet() const
{
    return d->documentRelSet;
}

void OpcRelSetManager::setDocumentRelSet(OpcRelSet *relSet)
{
    d->documentRelSet = relSet;
}

void OpcRelSetManager::clear()
{
    qDeleteAll(d->relSets);
    d->relSets.clear();

    if (d->documentRelSet) {
        delete d->documentRelSet;
        d->documentRelSet = nullptr;
    }
}

// ----------------------------------------------------------------

bool OpcRelSetManager::loadRelSets(KoStore *odfStore)
{
    Q_UNUSED(odfStore);

    // QString errorMsg;
    // int errorLine;
    // int errorColumn;

    KoXmlStreamReader reader;
    // FIXME: Add expected namespaces for rels here.

    // FIXME: Look up all .rels file and load them here.

    // FIXME: Return actual return status.
    return true;
}

bool OpcRelSetManager::saveRelSets(KoStore *odfStore)
{
    Q_UNUSED(odfStore);

    // FIXME: save the document relset here

    //     foreach (const QString &path, d->relSets.keys()) {
    // FIXME: save the .rels file for the file with path 'path' here.
    //     }

    // FIXME: Return actual return status.
    return true;
}
