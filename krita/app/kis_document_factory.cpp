/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
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
#include "kis_document_factory.h"

#include <QMimeData>

#include "kis_image_document.h"

KisDocumentFactory::KisDocumentFactory(QObject *parent)
    : AbstractDocumentFactory(parent)
{
}

virtual KisDocumentFactory::~KisDocumentFactory()
{
}


bool KisDocumentFactory::canCreateFromData( const QMimeData* mimeData )
{
    return true;
}

Kasten2::AbstractDocument* KisDocumentFactory::create()
{
    return new KisImageDocument();
}

virtual Kasten2::AbstractDocument* create(const QMap<QString, QVariant> &parameters)
{
    return new KisImageDocument(parameters);
}


Kasten2::AbstractDocument* KisDocumentFactory::createFromData( const QMimeData* mimeData, bool /*setModified*/ )
{
//    if (mimeData->hasImage() || mimeData->hasUrls() || mimeData->hasFormat("application/x-krita-node")) {

//    }
    return new KisImageDocument();
}
