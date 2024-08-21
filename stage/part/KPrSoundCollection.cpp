/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrSoundCollection.h"

#include "KPrSoundData.h"
#include "StageDebug.h"

#include <KoStoreDevice.h>
#include <KoXmlWriter.h>

#include <QList>
#include <QMimeDatabase>

class Q_DECL_HIDDEN KPrSoundCollection::Private
{
public:
    QList<KPrSoundData *> sounds;
};

KPrSoundCollection::KPrSoundCollection(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

KPrSoundCollection::~KPrSoundCollection()
{
    delete d;
}

void KPrSoundCollection::addSound(KPrSoundData *image)
{
    d->sounds.append(new KPrSoundData(*image));
}

void KPrSoundCollection::removeSound(KPrSoundData *image)
{
    foreach (KPrSoundData *data, d->sounds) {
        if (data->operator==(*image)) {
            d->sounds.removeAll(data);
            delete data;
        }
    }
}

KPrSoundData *KPrSoundCollection::findSound(const QString &title)
{
    foreach (KPrSoundData *sound, d->sounds) {
        if (sound->title() == title) {
            return sound;
        }
    }
    return nullptr;
}

QStringList KPrSoundCollection::titles()
{
    QStringList list;
    list.reserve(d->sounds.size());

    foreach (KPrSoundData *sound, d->sounds) {
        list << sound->title();
    }
    return list;
}

// TODO move to loading of the actual element using the sound
bool KPrSoundCollection::completeLoading(KoStore *store)
{
    foreach (KPrSoundData *sound, d->sounds) {
        if (!store->open(sound->storeHref()))
            return false;
        bool ok = sound->loadFromFile(new KoStoreDevice(store));
        store->close();
        if (!ok) {
            return false;
        }
    }
    return true;
}

// use a KoSharedSavingData in the context to save which sounds need to be saved
bool KPrSoundCollection::completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context)
{
    Q_UNUSED(context);
    foreach (KPrSoundData *sound, d->sounds) {
        if (sound->isTaggedForSaving()) {
            if (!store->open(sound->storeHref()))
                return false;
            bool ok = sound->saveToFile(new KoStoreDevice(store));
            store->close();
            if (!ok)
                return false;
            // TODO: can't we get the mimetype from elsewhere? e.g. already when loading? or from data?
            const QString mimetype(QMimeDatabase().mimeTypesForFileName(sound->storeHref()).first().name());
            manifestWriter->addManifestEntry(sound->storeHref(), mimetype);
        }
    }
    return true;
}
