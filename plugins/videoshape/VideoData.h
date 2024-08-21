/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef VIDEODATA_H
#define VIDEODATA_H

#include <KoShapeUserData.h>

class QIODevice;
class QUrl;
class VideoCollection;
class VideoDataPrivate;
class KoStore;

/**
 * This class is meant to represent the video data so it can be shared between video shapes.
 * This class inherits from KoShapeUserData which means you can set it on any KoShape using
 * KoShape::setUserData() and get it using KoShape::userData().  The videoshape plugin
 * uses this class to show its video data.
 */
class VideoData : public KoShapeUserData
{
    Q_OBJECT
public:
    /// Various error codes representing what has gone wrong
    enum ErrorCode {
        Success,
        OpenFailed,
        StorageFailed, ///< This is set if the video data has to be stored on disk in a temporary file, but we failed to do so
        LoadFailed
    };

    /// default constructor, creates an invalid imageData object
    VideoData();

    /**
     * copy constructor
     * @param videoData the other one.
     */
    VideoData(const VideoData &videoData);

    /// destructor
    virtual ~VideoData();

    void setExternalVideo(const QUrl &location, bool saveInternal, VideoCollection *collection = nullptr);
    void setVideo(const QString &location, KoStore *store, VideoCollection *collection = nullptr);
    // void setVideo(const QUrl &location);

    /**
     * Save the video data to the param device.
     * The full file is saved.
     * @param device the device that is used to get the data from.
     * @return returns true if load was successful.
     */
    bool saveData(QIODevice &device);

    QString tagForSaving(int &counter);

    VideoData &operator=(const VideoData &other);

    inline bool operator!=(const VideoData &other) const
    {
        return !operator==(other);
    }
    bool operator==(const VideoData &other) const;

    /// returns if this is a valid imageData with actual video data present on it.
    bool isValid() const;

    QUrl playableUrl() const;

    QString saveName() const;

    void setSaveName(const QString &saveName);

    VideoCollection *collection();
    void setCollection(VideoCollection *collection);

    qint64 key();

    enum DataStoreState {
        StateEmpty, ///< No video data, possible an external video
        StateSpooled, ///< Video data is spooled
    };

protected:
    friend class VideoCollection;

    /// take the data from \a device and store it in the temporaryFile
    void copyToTemporary(QIODevice &device);

    void clear();

    static qint64 generateKey(const QByteArray &bytes);

private:
    VideoDataPrivate *d;
};

#endif
