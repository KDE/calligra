/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VIDEOTHUMBNAILER_H
#define VIDEOTHUMBNAILER_H

#include <phonon/Global>
#include <phonon/MediaObject>
#include <phonon/experimental/videodataoutput2.h>

#include <QEventLoop>
#include <QImage>
#include <QObject>
#include <QSize>

class VideoData;

namespace Phonon
{
namespace Experimental
{
class VideoDataOutput2;
}
class MediaObject;
}

class VideoThumbnailer : public QObject
{
    Q_OBJECT

public:
    VideoThumbnailer();
    ~VideoThumbnailer();

    void createThumbnail(VideoData *videoData, const QSize &size);
    QImage thumbnail();

Q_SIGNALS:
    void thumbnailReady();
    void signalCreateThumbnail(VideoData *videoData, const QSize &size);

private Q_SLOTS:
    void slotCreateThumbnail(VideoData *videoData, const QSize &size);
    void frameReady(const Phonon::Experimental::VideoFrame2 &frame);
    void stateChanged(Phonon::State newState, Phonon::State oldState);

private:
    static bool isFrameInteresting(const QImage &frame);
    Phonon::MediaObject m_media;
    Phonon::Experimental::VideoDataOutput2 m_vdata;
    QSize m_thumbnailSize;
    QEventLoop m_eventLoop;
    QImage m_thumbnailImage;
};

#endif // VIDEOTHUMBNAILER_H
