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
#include "kis_image_document.h"

#include <QString>
#include <QVariant>

#include <klocale.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorModelStandardIds.h>

#include <kis_image.h>
#include <kis_undo_stores.h>


class KisImageDocument::Private {

    Private()
        : modified(false)
    {
        // XXX: needs access to the document!
        undoStore = new KisSurrogateUndoStore();
    }

    bool modified;
    KisUndoStore *undoStore;
    KisImageSP image;
};

KisImageDocument::KisImageDocument(QObject *parent = 0)
{

    d->image = new KisImage(d->undoStore, 1024, 1024, KoColorSpaceRegistry::instance()->rgb8(0), "Unnamed");
}

KisImageDocument::KisImageDocument(const QMap<QString, QVariant> &parameters, QObject *parent)
    : Kasten2::AbstractDocument(parent)
    , d(new Private())
{
    qint32 width = parameters.value("width", QVariant(1024)).toInt();
    qint32 height = parameters.value("height", QVariant(1024)).toInt();
    QString name = parameters.value("name", QVariant("Unnamed")).toString();

    QString colorSpaceModel = parameters.value("colorSpaceModel", QVariant(RGBAColorModelID.id())).toString();
    QString colorDepth = parameters.value("colorDepth", QVariant(Integer8BitsColorDepthID.id())).toString();
    QString colorProfile = parameters.value("colorProfile", QVariant("")).toString();

    const KoColorSpace *cs = KoColorSpaceRegistry::instance()->colorSpace(colorSpaceModel,
                                                                          colorDepth,
                                                                          colorProfile);

    bool startProjection = parameters.value("startProjection", QVariant(true)).toBool();

    d->image = new KisImage(d->undoStore, width, height, cs, name, startProjection);

    connect(d->image, SIGNAL(sigImageModified()), this, SLOT(setModified()));
}

KisImageDocument::KisImageDocument(KisImageSP image, QObject *parent = 0)
    : Kasten2::AbstractDocument(parent)
    , d(new Private())
{
    d->image = image;
    connect(d->image, SIGNAL(sigImageModified()), this, SLOT(setModified()));
}

KisImageDocument::~KisImageDocument()
{
    delete d->undoStore;
    delete d;
}

QString KisImageDocument::typeName() const {
    return i18nc( "name of the data type", "Krita Image" );
}

QString KisImageDocument::mimeType() const {
    return QString::fromLatin1(APP_MIMETYPE);
}

ContentFlags KisImageDocument::contentFlags() const {
    return (isModified() ? Kasten2::ContentHasUnstoredChanges : Kasten2::ContentStateNormal);

}


bool KisImageDocument::isModified() const
{
    return d->modified;
}


void KisImageDocument::setModified()
{
    d->modified = true;
}

void KisImageDocument::image() const
{
    return d->image;
}
