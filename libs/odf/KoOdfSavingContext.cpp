/* This file is part of the KDE project
   Copyright (C) 2004-2006 David Faure <faure@kde.org>
   Copyright (C) 2007-2009, 2011 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2013 inge Wallin <inge@lysator.liu.se>

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
 * Boston, MA 02110-1301, USA.
*/

// Own
#include "KoOdfSavingContext.h"

// Qt
#include <QMap>
#include <QUuid>

// KDE libs
#include <kmimetype.h>
#include <kdebug.h>

//#include "KoImageData.h"
//#include "KoMarker.h"

// odf library
#include <KoXmlWriter.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoElementReference.h>


class KoOdfSavingContext::Private {
public:
    Private(KoXmlWriter&, KoGenStyles&, KoEmbeddedDocumentSaver&);
    ~Private();

    KoXmlWriter *xmlWriter;
    KoGenStyles& mainStyles;
    KoEmbeddedDocumentSaver& embeddedSaver;

    //QSet<KoDataCenterBase *> dataCenters;

    //QMap<qint64, QString> imageNames;
    //int imageId;
    //QMap<QString, QImage> images;

    //QMap<const KoMarker *, QString> markerRefs;

    QMap<const void*, KoElementReference> references;
    QMap<QString, int> referenceCounters;
    QMap<QString, QList<const void*> > prefixedReferences;
};


KoOdfSavingContext::Private::Private(KoXmlWriter &w, KoGenStyles &s, KoEmbeddedDocumentSaver &e)
    : xmlWriter(&w)
      //, imageId(0)
    , mainStyles(s)
    , embeddedSaver(e)
{
}

KoOdfSavingContext::Private::~Private()
{
}


KoOdfSavingContext::KoOdfSavingContext(KoXmlWriter &xmlWriter, KoGenStyles &mainStyles,
                                       KoEmbeddedDocumentSaver &embeddedSaver)
    : d(new Private(xmlWriter, mainStyles, embeddedSaver))
{
}

KoOdfSavingContext::~KoOdfSavingContext()
{
    delete d;
}

KoXmlWriter &KoOdfSavingContext::xmlWriter()
{
    return *d->xmlWriter;
}

void KoOdfSavingContext::setXmlWriter(KoXmlWriter &xmlWriter)
{
    d->xmlWriter = &xmlWriter;
}

KoGenStyles & KoOdfSavingContext::mainStyles()
{
    return d->mainStyles;
}

KoEmbeddedDocumentSaver &KoOdfSavingContext::embeddedSaver()
{
    return d->embeddedSaver;
}



KoElementReference KoOdfSavingContext::xmlid(const void *referent, const QString& prefix,
                                             KoElementReference::GenerationOption counter)
{
    Q_ASSERT(counter == KoElementReference::UUID
             || (counter == KoElementReference::Counter && !prefix.isEmpty()));

    if (d->references.contains(referent)) {
        return d->references[referent];
    }

    KoElementReference ref;

    if (counter == KoElementReference::Counter) {
        int referenceCounter = d->referenceCounters[prefix];
        referenceCounter++;
        ref = KoElementReference(prefix, referenceCounter);
        d->references.insert(referent, ref);
        d->referenceCounters[prefix] = referenceCounter;
    }
    else {
        if (!prefix.isEmpty()) {
            ref = KoElementReference(prefix);
            d->references.insert(referent, ref);
        }
        else {
            d->references.insert(referent, ref);
        }
    }

    if (!prefix.isNull()) {
        d->prefixedReferences[prefix].append(referent);
    }
    return ref;
}

KoElementReference KoOdfSavingContext::existingXmlid(const void *referent)
{
    if (d->references.contains(referent)) {
        return d->references[referent];
    }
    else {
        KoElementReference ref;
        ref.invalidate();
        return ref;
    }
}

void KoOdfSavingContext::clearXmlIds(const QString &prefix)
{

    if (d->prefixedReferences.contains(prefix)) {
        foreach(const void* ptr, d->prefixedReferences[prefix]) {
            d->references.remove(ptr);
        }
        d->prefixedReferences.remove(prefix);
    }

    if (d->referenceCounters.contains(prefix)) {
        d->referenceCounters[prefix] = 0;
    }
}


#if 0
QString KoOdfSavingContext::imageHref(const KoImageData *image)
{
    QMap<qint64, QString>::iterator it(d->imageNames.find(image->key()));
    if (it == d->imageNames.end()) {
        QString suffix = image->suffix();
        if (suffix.isEmpty()) {
            it = d->imageNames.insert(image->key(), QString("Pictures/image%1").arg(++d->imageId));
        }
        else {
            it = d->imageNames.insert(image->key(), QString("Pictures/image%1.%2").arg(++d->imageId).arg(suffix));
        }
    }
    return it.value();
}

QString KoOdfSavingContext::imageHref(const QImage &image)
{
    // TODO this can be optimized to recognize images which have the same content
    // Also this can use quite a lot of memeory as the qimage are all kept until
    // they are saved to the store in memory
    QString href = QString("Pictures/image%1.png").arg(++d->imageId);
    d->images.insert(href, image);
    return href;
}

QMap<qint64, QString> KoOdfSavingContext::imagesToSave()
{
    return d->imageNames;
}
#endif
#if 0
QString KoOdfSavingContext::markerRef(const KoMarker *marker)
{
    QMap<const KoMarker *, QString>::iterator it = d->markerRefs.find(marker);
    if (it == d->markerRefs.end()) {
        it = d->markerRefs.insert(marker, marker->saveOdf(*this));
    }

    return it.value();
}
#endif
#if 0
void KoOdfSavingContext::addDataCenter(KoDataCenterBase * dataCenter)
{
    if (dataCenter) {
        d->dataCenters.insert(dataCenter);
    }
}

bool KoOdfSavingContext::saveDataCenter(KoStore *store, KoXmlWriter* manifestWriter)
{
    bool ok = true;
    foreach(KoDataCenterBase *dataCenter, d->dataCenters) {
        ok = ok && dataCenter->completeSaving(store, manifestWriter, this);
        //kDebug() << "ok" << ok;
    }

    // Save images
    for (QMap<QString, QImage>::iterator it(d->images.begin()); it != d->images.end(); ++it) {
        if (store->open(it.key())) {
            KoStoreDevice device(store);
            ok = ok && it.value().save(&device, "PNG");
            store->close();
            // TODO error handling
            if (ok) {
                const QString mimetype(KMimeType::findByPath(it.key(), 0 , true)->name());
                manifestWriter->addManifestEntry(it.key(), mimetype);
            }
            else {
                kWarning(30006) << "saving image failed";
            }
        }
        else {
            ok = false;
            kWarning(30006) << "saving image failed: open store failed";
        }
    }
    return ok;
}
#endif
