/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef VIDEOCOLLECTION_H
#define VIDEOCOLLECTION_H

#include <KoDataCenterBase.h>
#include <QObject>

class QUrl;
class KoStore;
class VideoData;

/**
 * An collection of VideoData objects to allow loading and saving them all together to the KoStore.
 * It also makes sure that if the same image is added to the collection that they share the internal data structure.
 */
class VideoCollection : public QObject, public KoDataCenterBase
{
    Q_OBJECT
public:
    enum ResouceManager {
        ResourceId = 75208282
    };
    /// constructor
    explicit VideoCollection(QObject *parent = nullptr);
    virtual ~VideoCollection();

    /// reimplemented
    bool completeLoading(KoStore *store) override;

    /**
     * Save all videos to the store which are in the context
     * @return returns true if save was successful (no videos failed).
     */
    bool completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context) override;

    /**
     * Create a data object for the video data.
     * The collection will create an video data in a way that if there is an
     * existing data object with the same video the returned VideoData will
     * share its data.
     * @param url a valid, local url to point to an video on the filesystem.
     * @param saveInternal, if true then the video is saved inside the resulting document
     * @see VideoData::isValid()
     */
    VideoData *createExternalVideoData(const QUrl &url, bool saveInternal);

    /**
     * Create a data object for the video data.
     * The collection will create an image data in a way that if there is an
     * existing data object with the same video the returned VideoData will
     * share its data.
     * @param href the name of the video inside the store.
     * @param store the KoStore object.
     * @see VideoData::isValid()
     */
    VideoData *createVideoData(const QString &href, KoStore *store);

    void add(const VideoData &data);
    void remove(const VideoData &data);
    void removeOnKey(qint64 videoDataKey);

    /**
     * Get the number of videos inside the collection
     */
    int size() const;
    /**
     * Get the number of videos inside the collection
     */
    int count() const;

    int saveCounter;

private:
    VideoData *cacheVideo(VideoData *data);

    class Private;
    Private *const d;
};

#endif // VIDEOCOLLECTION_H
