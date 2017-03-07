/* This file is part of the KDE project
 *
 * Copyright (C) 2013-2014 Inge Wallin <inge@lysator.liu.se>
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
#include "OpcRelSetManager.h"

// Qt
#include <QHash>
#include <QString>

// odf lib
#include "KoStore.h"
#include <KoXmlStreamReader.h>
#include <KoXmlNS.h>

// Filter libraries
#include "KoOdfStyle.h"

// This filter
#include "OpcRelSet.h"
#include "DocxExportDebug.h"


// ================================================================
//                 class OpcRelSetManager


class OpcRelSetManager::Private
{
public:
    Private();
    ~Private();

    QHash<QString, OpcRelSet*> relSets;         // path, relations
    OpcRelSet                 *documentRelSet;  // The relations for the whole document
                                                // Stored in _rels/.rels 
};

OpcRelSetManager::Private::Private()
    : documentRelSet(0)
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
        d->documentRelSet = 0;
    }
}


// ----------------------------------------------------------------


bool OpcRelSetManager::loadRelSets(KoStore *odfStore)
{
    Q_UNUSED(odfStore);

    QString errorMsg;
    //int errorLine;
    //int errorColumn;

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
