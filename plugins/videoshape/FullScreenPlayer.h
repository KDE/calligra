/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef FULLSCREENPLAYER_H
#define FULLSCREENPLAYER_H

#include <QWidget>
#include <phonon/Global>

namespace Phonon
{
class MediaObject;
class VideoWidget;
class AudioOutput;
class SeekSlider;
class VolumeSlider;
}

class QUrl;
class QLabel;
class QToolButton;

/**
 * This class represents the click event action that starts the playing of the video.
 */
class FullScreenPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit FullScreenPlayer(const QUrl &);

    /// destructor
    virtual ~FullScreenPlayer();

protected Q_SLOTS:
    void play();
    void pause();
    void stop();
    void mute();
    void unmute();
    void playStateChanged(Phonon::State newState, Phonon::State oldState);
    void muteStateChanged(bool muted);
    void updatePlaybackTime(qint64 currentTime);

protected:
    void keyPressEvent(QKeyEvent *event) override; /// reimplemented
    void mousePressEvent(QMouseEvent *event) override; /// reimplemented

    Phonon::MediaObject *m_mediaObject;
    Phonon::VideoWidget *m_videoWidget;
    Phonon::AudioOutput *m_audioOutput;
    Phonon::SeekSlider *m_seekSlider;
    Phonon::VolumeSlider *m_volumeSlider;

    QToolButton *m_volumeIconMuted;
    QToolButton *m_volumeIconUnmuted;
    QLabel *m_playbackTime;
    QToolButton *m_play;
    QToolButton *m_pause;
    QToolButton *m_stop;
};

#endif
