/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2008 C. Boemann <cbo@boemann.dk>
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

#include "KPrSoundData.h"

#include "KPrSoundCollection.h"
#include "StageDebug.h"

#include <QTemporaryFile>
#include <QIODevice>

// make it a QSharedData
class Q_DECL_HIDDEN KPrSoundData::Private
{
public:
    Private(KPrSoundCollection *c)
    : refCount(0)
    , collection(c)
    , tempFile(0)
    , taggedForSaving(false)
    { }

    ~Private() {
        delete tempFile;
    }
    QString tempFileName;
    QString title;

    int refCount;
    QString storeHref;
    KPrSoundCollection *collection;
    QTemporaryFile *tempFile;
    bool taggedForSaving;
};

KPrSoundData::KPrSoundData(KPrSoundCollection *collection, const QString &href)
    : d(new Private(collection))
{
    Q_ASSERT(collection);
    collection->addSound(this);
    d->storeHref = href;
    //TODO make sure the title is not duplicated
    d->title = href.section('/', -1); // TODO only works on linux like filenames
    Q_ASSERT(d->refCount == 1);
}

KPrSoundData::KPrSoundData(const KPrSoundData &soundData)
:    d(soundData.d)
{
    d->refCount++;
}

KPrSoundData::~KPrSoundData() {
    if(--d->refCount == 0) {
        d->collection->removeSound(this);
        delete d;
    }
}

bool KPrSoundData::operator==(const KPrSoundData &other) const {
    return other.d == d;
}

QString KPrSoundData::tagForSaving() {
    d->taggedForSaving=true;
    d->storeHref = QString("Sounds/%1").arg(d->title);

    return d->storeHref;
}

QString KPrSoundData::storeHref() const {
    return d->storeHref;
}

QString KPrSoundData::nameOfTempFile() const {
    return d->tempFileName;
}

QString KPrSoundData::title() const {
    return d->title;
}

bool KPrSoundData::saveToFile(QIODevice *device)
{
    if(! d->tempFile->open())
        return false;
    char * data = new char[32 * 1024];
    while(true) {
        bool failed = false;
        qint64 bytes = d->tempFile->read(data, 32*1024);
        if(bytes == 0)
            break;
        else if(bytes == -1) {
            warnStage << "Failed to read data from the tmpfile";
            failed = true;
        }
        while(! failed && bytes > 0) {
            qint64 written = device->write(data, bytes);
            if(written < 0) {// error!
                warnStage << "Failed to copy the sound from the temp file";
                failed = true;
            }
            bytes -= written;
        }
        if(failed) { // read or write failed; so lets cleanly abort.
                delete[] data;
            return false;
        }
    }
    delete[] data;
    return true;
}

bool KPrSoundData::isTaggedForSaving()
{
    return d->taggedForSaving;
}


bool KPrSoundData::loadFromFile(QIODevice *device) {
    struct Finally {
        Finally(QIODevice *d) : device (d), bytes(0) {}
        ~Finally() {
            delete device;
            delete[] bytes;
        }
        QIODevice *device;
        char *bytes;
    };
    Finally finally(device);

    // remove prev data
    delete d->tempFile;
    d->tempFile = 0;

    d->tempFile = new QTemporaryFile();
    if(! d->tempFile->open())
        return false;
    char * data = new char[32 * 1024];
    finally.bytes = data;
    while(true) {
        bool failed = false;
        qint64 bytes = device->read(data, 32*1024);
        if(bytes == 0)
            break;
        else if(bytes == -1) {
            warnStage << "Failed to read sound data";
            failed = true;
        }
        while(! failed && bytes > 0) {
            qint64 written = d->tempFile->write(data, bytes);
            if(written < 0) {// error!
                warnStage << "Failed to copy the sound to temp";
                failed = true;
            }
            bytes -= written;
        }
        if(failed) { // read or write failed; so lets cleanly abort.
            delete d->tempFile;
            d->tempFile = 0;
            return false;
        }
    }
    d->tempFileName = d->tempFile->fileName();
    d->tempFile->close();
    return true;
}

KPrSoundCollection * KPrSoundData::soundCollection()
{
    return d->collection;
}
