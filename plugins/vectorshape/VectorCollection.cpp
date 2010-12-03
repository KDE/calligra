/* This file is part of the KDE project
 * Copyright (C) 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2009 Casper Boemann <cbo@boemann.dk>
 * Copyright (C) 2010 Inge Wallin <inge@lysator.liu.se>
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

#include "VectorCollection.h"
#include "VectorData.h"
#include "KoShapeSavingContext.h"

#include "Debug.h"

#include <KoStoreDevice.h>
#include <QCryptographicHash>
#include <KoXmlWriter.h>

#include <QMap>
#include <kdebug.h>
#include <kmimetype.h>

class VectorCollection::Private
{
public:
    ~Private()
    {
    }

    // A map with all the files in the collection
    QMap<qint64, VectorData*>     vectorFiles;

    // an extra map to find all dataObjects based on the key of a store.
    QMap<QByteArray, VectorData*> storeVectors;
};


VectorCollection::VectorCollection(QObject *parent)
    : QObject(parent)
    , saveCounter(0)
    , d(new Private())
{
    DEBUG_BLOCK
}

VectorCollection::~VectorCollection()
{
    DEBUG_BLOCK

    foreach(VectorData *id, d->vectorFiles) {
        id->m_collection = 0;
    }
    delete d;
}

bool VectorCollection::completeLoading(KoStore *store)
{
    Q_UNUSED( store );
    d->storeVectors.clear();
    return true;
}


bool VectorCollection::completeSaving(KoStore *store, KoXmlWriter *manifestWriter,
                                      KoShapeSavingContext *context)
{
    Q_UNUSED(context);

    // Loop through the files in the collection and save them.
    QMap<qint64, VectorData *>::iterator  dataIt(d->vectorFiles.begin());
    while (dataIt != d->vectorFiles.end()) {
        if (dataIt.value()->m_saveName.isEmpty())
            continue;

        VectorData *videoData = dataIt.value();
        if (store->open(videoData->m_saveName)) {
            KoStoreDevice device(store);
            bool ok = videoData->saveData(device);
            store->close();

            // If all went well, write the mime type to the manifest.
            if (ok) {
                const QString mimetype(KMimeType::findByPath(videoData->m_saveName, 0 , true)->name());
                manifestWriter->addManifestEntry(videoData->m_saveName, mimetype);
            } else {
                // TODO better error handling
                kWarning(30006) << "saving vector file failed";
            }
        } else {
            // TODO better error handling
            kWarning(30006) << "saving vector file failed: open store failed";
        }
        dataIt.value()->m_saveName.clear();

        ++dataIt;
    }
    saveCounter=0;
    return true;
}

VectorData *VectorCollection::createExternalVectorData(const QUrl &url)
{
    Q_ASSERT(!url.isEmpty() && url.isValid());

    // Check if the collection already contains this file.  If so, just return a copy.
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(url.toEncoded());
    qint64 key = VectorData::generateKey(md5.result());
    if (d->vectorFiles.contains(key))
        return new VectorData(*(d->vectorFiles.value(key)));

    // Otherwise create a new 
    VectorData *data = new VectorData();
    data->setExternalVector(url);
    data->m_collection = this;
    Q_ASSERT(data->m_key == key);
    d->vectorFiles.insert(key, data);
    return data;
}

VectorData *VectorCollection::createVectorData(const QString &href, KoStore *store)
{
    // the tricky thing with a 'store' is that we need to read the data now
    // as the store will no longer be readable after the loading completed.
    // The solution we use is to read the data, store it in a KTemporaryFile
    // and read and parse it on demand when the video data is actually needed.
    // This leads to having two keys, one for the store and one for the
    // actual video data. We need the latter so if someone else gets the same
    // video data he can find this data and share (warm fuzzy feeling here)
    QByteArray storeKey = (QString::number((qint64) store) + href).toLatin1();
    if (d->storeVectors.contains(storeKey))
        return new VectorData(*(d->storeVectors.value(storeKey)));

    VectorData *data = new VectorData();
    data->setVector(href, store);
    data->m_collection = this;

    d->storeVectors.insert(storeKey, data);
    return data;
}

int VectorCollection::size() const
{
    return d->vectorFiles.count();
}

int VectorCollection::count() const
{
    return d->vectorFiles.count();
}

void VectorCollection::removeOnKey(qint64 videoDataKey)
{
    d->vectorFiles.remove(videoDataKey);
}
