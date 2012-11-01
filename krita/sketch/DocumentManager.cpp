/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "DocumentManager.h"

#include <KoColorSpaceRegistry.h>

#include <kis_doc2.h>
#include <kis_image.h>

class DocumentManager::Private
{
public:
    Private() : document(0) { }
    KisDoc2 *document;
    KisPart2 *part;
};

DocumentManager *DocumentManager::sm_instance = 0;

KisDoc2* DocumentManager::document()
{
    return d->document;
}

KisPart2* DocumentManager::part()
{
    return d->part;
}

void DocumentManager::newDocument(int width, int height, float resolution)
{
    closeDocument();

    d->document = new KisDoc2(d->part);
    d->document->setSaveInBatchMode(true);
    d->part->setDocument(d->document);
    d->document->newImage("New Image", width, height, KoColorSpaceRegistry::instance()->rgb8());
    d->document->image()->setResolution(resolution, resolution);

    emit documentChanged();
}

void DocumentManager::openDocument(const QString& document)
{
    closeDocument();

    d->document = new KisDoc2(d->part);
    d->document->setSaveInBatchMode(true);
    d->part->setDocument(d->document);

    d->document->openUrl(QUrl::fromLocalFile(document));

    emit documentChanged();
}

void DocumentManager::closeDocument()
{
    if(d->document) {
        emit aboutToDeleteDocument();
        d->part->closeUrl(false);
        delete d->document;
    }
}

DocumentManager* DocumentManager::instance()
{
    if(!sm_instance) {
        sm_instance = new DocumentManager(QCoreApplication::instance());
    }

    return sm_instance;
}

DocumentManager::DocumentManager(QObject* parent)
    : QObject(parent), d(new Private)
{
    d->part = new KisPart2(this);
}

DocumentManager::~DocumentManager()
{
    delete d;
}

