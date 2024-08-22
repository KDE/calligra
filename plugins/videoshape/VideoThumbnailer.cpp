/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * SPDX-FileCopyrightText: 2006-2009 Marco Gulino <marco.gulino@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoThumbnailer.h"

#include "VideoData.h"
#include "VideoDebug.h"

#include <QTime>
#include <QVBoxLayout>
#include <QVarLengthArray>
#include <phonon/experimental/videoframe2.h>

#define THRESHOLD_FRAME_VARIANCE 40.0

VideoThumbnailer::VideoThumbnailer()
    : QObject()
{
    m_vdata.setRunning(true);
    Phonon::createPath(&m_media, &m_vdata);
    connect(&m_media, &Phonon::MediaObject::stateChanged, this, &VideoThumbnailer::stateChanged);
    connect(this, &VideoThumbnailer::signalCreateThumbnail, this, &VideoThumbnailer::slotCreateThumbnail, Qt::QueuedConnection);
}

VideoThumbnailer::~VideoThumbnailer() = default;

void VideoThumbnailer::createThumbnail(VideoData *videoData, const QSize &size)
{
    Q_EMIT signalCreateThumbnail(videoData, size);
}

void VideoThumbnailer::slotCreateThumbnail(VideoData *videoData, const QSize &size)
{
    m_media.setCurrentSource(videoData->playableUrl());
    m_media.play();

    m_thumbnailSize = size;

    int retcode = 0;
    for (int i = 0; i < 50; i++) {
        retcode = m_eventLoop.exec();
        if (retcode == 0) {
            break;
        }
        debugVideo << "Seeking to " << (i * 3);
        m_media.seek(i * 3);
    }
    if (retcode) {
        warnVideo << "Unable to generate thumbnail for ";
        m_media.stop();
        return;
    }
    m_media.stop();

    Q_EMIT thumbnailReady();
}

void VideoThumbnailer::frameReady(const Phonon::Experimental::VideoFrame2 &frame)
{
    QImage thumb = frame.qImage().scaled(m_thumbnailSize.width(), m_thumbnailSize.height(), Qt::KeepAspectRatio);
    if (isFrameInteresting(thumb)) {
        m_thumbnailImage = thumb;
        disconnect(&m_vdata, &Phonon::Experimental::VideoDataOutput2::frameReadySignal, this, &VideoThumbnailer::frameReady);
        m_eventLoop.quit();
        return;
    }
    m_eventLoop.exit(1);
}

void VideoThumbnailer::stateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    if (newState == Phonon::PlayingState) {
        connect(&m_vdata, &Phonon::Experimental::VideoDataOutput2::frameReadySignal, this, &VideoThumbnailer::frameReady);
        m_eventLoop.exit(1);
    }
}

QImage VideoThumbnailer::thumbnail()
{
    return m_thumbnailImage;
}

bool VideoThumbnailer::isFrameInteresting(const QImage &frame)
{
    float variance = 0;
    // taken from mplayerthumbs
    uint delta = 0;
    uint avg = 0;
    uint bytes = frame.sizeInBytes();
    uint STEPS = bytes / 2;
    QVarLengthArray<uchar> pivot(STEPS);

    const uchar *bits = frame.bits();
    // First pass: get pivots and taking average
    for (uint i = 0; i < STEPS; i++) {
        pivot[i] = bits[i * (bytes / STEPS)];
        avg += pivot[i];
    }
    avg = avg / STEPS;
    // Second Step: calculate delta (average?)
    for (uint i = 0; i < STEPS; i++) {
        int curdelta = abs(int(avg - pivot[i]));
        delta += curdelta;
    }
    variance = delta / STEPS;

    return variance > THRESHOLD_FRAME_VARIANCE;
}
