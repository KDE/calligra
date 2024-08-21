/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoData.h"

#include "VideoCollection.h"
#include "VideoDebug.h"

#include <KoStore.h>
#include <KoStoreDevice.h>

#include <QApplication>
#include <QAtomicInt>
#include <QBuffer>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QUrl>

class VideoDataPrivate
{
public:
    VideoDataPrivate();
    ~VideoDataPrivate();

    /// store the suffix based on the full filename.
    void setSuffix(const QString &fileName);

    QAtomicInt refCount;
    QTemporaryFile *temporaryFile;
    /**
     * a unique key of the video data
     */
    qint64 key;

    QString suffix; // the suffix of the video e.g. avi  TODO use a QByteArray ?

    QString saveName;

    QUrl videoLocation;

    VideoData::ErrorCode errorCode;

    VideoCollection *collection;

    // video data store.
    VideoData::DataStoreState dataStoreState;

    bool saveVideoInZip;
};

VideoDataPrivate::VideoDataPrivate()
    : refCount(0)
    , temporaryFile(nullptr)
    , key(0)
    , errorCode(VideoData::Success)
    , collection(nullptr)
    , dataStoreState(VideoData::StateEmpty)
    , saveVideoInZip(false)
{
}

VideoDataPrivate::~VideoDataPrivate()
{
    delete temporaryFile;
}

void VideoDataPrivate::setSuffix(const QString &name)
{
    QRegularExpression rx("\\.([^/]+$)"); // TODO does this work on windows or do we have to use \ instead of / for a path separator?
    QRegularExpressionMatch match;
    if (name.indexOf(rx, 0, &match) != -1) {
        suffix = match.captured(1);
    }
}

VideoData::VideoData()
    : KoShapeUserData()
    , d(nullptr)
{
}

VideoData::VideoData(const VideoData &videoData)
    : KoShapeUserData()
    , d(videoData.d)
{
    Q_UNUSED(videoData);

    if (d) {
        d->refCount.ref();
    }
}

VideoData::~VideoData()
{
    if (d && d->collection) {
        d->collection->removeOnKey(d->key);
    }

    if (d && !d->refCount.deref()) {
        delete d;
    }
}

void VideoData::setExternalVideo(const QUrl &location, bool saveInternal, VideoCollection *collection)
{
    if (collection) {
        // let the collection first check if it already has one. If it doesn't it'll call this method
        // again and we'll go to the other clause
        VideoData *other = collection->createExternalVideoData(location, saveInternal);
        this->operator=(*other);
        delete other;
    } else {
        delete d;
        d = new VideoDataPrivate();
        d->refCount.ref();

        d->videoLocation = location;
        d->saveVideoInZip = saveInternal;
        if (d->saveVideoInZip) {
            QFileInfo fileInfo(location.toLocalFile());
            d->setSuffix(fileInfo.fileName());
        } else {
            d->setSuffix(location.toEncoded());
        }

        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(location.toEncoded().append(saveInternal ? "true" : "false"));
        d->key = VideoData::generateKey(md5.result());
    }
}

void VideoData::setVideo(const QString &url, KoStore *store, VideoCollection *collection)
{
    if (collection) {
        // let the collection first check if it already has one. If it doesn't it'll call this method
        // again and we'll go to the other clause
        VideoData *other = collection->createVideoData(url, store);
        this->operator=(*other);
        delete other;
    } else {
        if (store->open(url)) {
            struct Finalizer {
                ~Finalizer()
                {
                    store->close();
                }
                KoStore *store;
            };
            Finalizer closer;
            closer.store = store;
            KoStoreDevice device(store);
            // QByteArray data = device.readAll();
            if (!device.open(QIODevice::ReadOnly)) {
                warnVideo << "open file from store " << url << "failed";
                d->errorCode = OpenFailed;
                store->close();
                return;
            }

            copyToTemporary(device);

            d->setSuffix(url);
        } else {
            warnVideo << "Find file in store " << url << "failed";
            d->errorCode = OpenFailed;
            return;
        }
    }
}

QUrl VideoData::playableUrl() const
{
    if (d->dataStoreState == StateSpooled) {
        Q_ASSERT(d);
        return QUrl(d->temporaryFile->fileName());
    } else {
        return d->videoLocation;
    }
}

QString VideoData::tagForSaving(int &counter)
{
    if (!d->saveName.isEmpty())
        return d->saveName;

    if (!d->videoLocation.isEmpty()) {
        if (d->saveVideoInZip) {
            d->saveName = QString("Videos/video%1.%2").arg(++counter).arg(d->suffix);
            return d->saveName;
        } else {
            return d->videoLocation.toString();
        }
    }

    if (d->suffix.isEmpty()) {
        return d->saveName = QString("Videos/video%1").arg(++counter);
    } else {
        return d->saveName = QString("Videos/video%1.%2").arg(++counter).arg(d->suffix);
    }
}

bool VideoData::isValid() const
{
    return d->dataStoreState != VideoData::StateEmpty && d->errorCode == Success;
}

bool VideoData::operator==(const VideoData &other) const
{
    Q_UNUSED(other);
    return false;
}

VideoData &VideoData::operator=(const VideoData &other)
{
    if (other.d) {
        other.d->refCount.ref();
    }

    if (d && !d->refCount.deref()) {
        delete d;
    }

    d = other.d;
    return *this;
}

bool VideoData::saveData(QIODevice &device)
{
    if (d->dataStoreState == StateSpooled) {
        Q_ASSERT(d->temporaryFile); // otherwise the collection should not have called this
        if (d->temporaryFile) {
            if (!d->temporaryFile->open()) {
                warnVideo << "Read file from temporary store failed";
                return false;
            }
            char buf[8192];
            while (true) {
                d->temporaryFile->waitForReadyRead(-1);
                qint64 bytes = d->temporaryFile->read(buf, sizeof(buf));
                if (bytes <= 0)
                    break; // done!
                do {
                    qint64 nWritten = device.write(buf, bytes);
                    if (nWritten == -1) {
                        d->temporaryFile->close();
                        return false;
                    }
                    bytes -= nWritten;
                } while (bytes > 0);
            }
            d->temporaryFile->close();
        }
        return true;
    } else if (!d->videoLocation.isEmpty()) {
        if (d->saveVideoInZip) {
            // An external video have been specified
            QFile file(d->videoLocation.toLocalFile());

            if (!file.open(QIODevice::ReadOnly)) {
                warnVideo << "Read file failed";
                return false;
            }
            char buf[8192];
            while (true) {
                file.waitForReadyRead(-1);
                qint64 bytes = file.read(buf, sizeof(buf));
                if (bytes <= 0)
                    break; // done!
                do {
                    qint64 nWritten = device.write(buf, bytes);
                    if (nWritten == -1) {
                        file.close();
                        return false;
                    }
                    bytes -= nWritten;
                } while (bytes > 0);
            }
            file.close();
        }
    }
    return false;
}

void VideoData::copyToTemporary(QIODevice &device)
{
    delete d;
    d = new VideoDataPrivate();
    d->temporaryFile = new QTemporaryFile(QLatin1String("KoVideoData/") + qAppName() + QLatin1String("_XXXXXX"));
    d->refCount.ref();
    if (!d->temporaryFile->open()) {
        warnVideo << "open temporary file for writing failed";
        d->errorCode = VideoData::StorageFailed;
        delete d;
        d = nullptr;
        return;
    }
    QCryptographicHash md5(QCryptographicHash::Md5);
    char buf[8192];
    while (true) {
        device.waitForReadyRead(-1);
        qint64 bytes = device.read(buf, sizeof(buf));
        if (bytes <= 0)
            break; // done!
        md5.addData(buf, bytes);
        do {
            bytes -= d->temporaryFile->write(buf, bytes);
        } while (bytes > 0);
    }
    d->key = VideoData::generateKey(md5.result());
    d->temporaryFile->close();

    d->dataStoreState = StateSpooled;
}

qint64 VideoData::generateKey(const QByteArray &bytes)
{
    quint64 answer = 1;
    const int max = qMin(8, bytes.count());
    for (int x = 0; x < max; ++x)
        answer += static_cast<quint64>(bytes[x]) << (8 * x);
    return answer;
}

QString VideoData::saveName() const
{
    return d->saveName;
}

void VideoData::setSaveName(const QString &saveName)
{
    d->saveName = saveName;
}

VideoCollection *VideoData::collection()
{
    return d->collection;
}

void VideoData::setCollection(VideoCollection *collection)
{
    d->collection = collection;
}

qint64 VideoData::key()
{
    return d->key;
}
