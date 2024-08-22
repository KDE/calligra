/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FullScreenPlayer.h"

#include <KoIcon.h>

#include <KLocalizedString>

#include <phonon/SeekSlider>
#include <phonon/VolumeSlider>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/videowidget.h>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

FullScreenPlayer::FullScreenPlayer(const QUrl &url)
    : QWidget(nullptr)
    , m_seekSlider(new Phonon::SeekSlider(this))
    , m_volumeSlider(new Phonon::VolumeSlider(this))
{
    m_mediaObject = new Phonon::MediaObject();
    m_mediaObject->setTickInterval(1000);

    m_videoWidget = new Phonon::VideoWidget(this);
    Phonon::createPath(m_mediaObject, m_videoWidget);

    m_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory);
    connect(m_audioOutput, &Phonon::AudioOutput::mutedChanged, this, &FullScreenPlayer::muteStateChanged);

    Phonon::createPath(m_mediaObject, m_audioOutput);

    m_seekSlider->setMediaObject(m_mediaObject);
    m_seekSlider->setIconVisible(false);

    m_volumeSlider->setAudioOutput(m_audioOutput);
    m_volumeSlider->setMuteVisible(false);
    m_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    m_playbackTime = new QLabel(QString("00:00:00"), this);

    m_play = new QToolButton(this);
    m_play->setIcon(koIcon("media-playback-start"));
    m_play->setToolTip(i18n("Play"));
    connect(m_play, &QAbstractButton::clicked, this, &FullScreenPlayer::play);

    m_pause = new QToolButton(this);
    m_pause->setIcon(koIcon("media-playback-pause"));
    m_pause->setToolTip(i18n("Pause"));
    connect(m_pause, &QAbstractButton::clicked, this, &FullScreenPlayer::pause);

    m_stop = new QToolButton(this);
    m_stop->setIcon(koIcon("media-playback-stop"));
    m_stop->setToolTip(i18n("Stop"));
    connect(m_stop, &QAbstractButton::clicked, this, &FullScreenPlayer::stop);

    m_volumeIconMuted = new QToolButton(this);
    m_volumeIconMuted->setIcon(koIcon("audio-volume-muted"));
    m_volumeIconMuted->setToolTip(i18n("Unmute"));
    connect(m_volumeIconMuted, &QAbstractButton::clicked, this, &FullScreenPlayer::unmute);

    m_volumeIconUnmuted = new QToolButton(this);
    m_volumeIconUnmuted->setIcon(koIcon("audio-volume-medium"));
    m_volumeIconUnmuted->setToolTip(i18n("Mute"));
    connect(m_volumeIconUnmuted, &QAbstractButton::clicked, this, &FullScreenPlayer::mute);

    QHBoxLayout *playbackControls = new QHBoxLayout();
    playbackControls->addWidget(m_play);
    playbackControls->addWidget(m_pause);
    playbackControls->addWidget(m_stop);
    playbackControls->addWidget(m_seekSlider);
    playbackControls->addWidget(m_playbackTime);
    playbackControls->addWidget(m_volumeIconMuted);
    playbackControls->addWidget(m_volumeIconUnmuted);
    playbackControls->addWidget(m_volumeSlider);
    playbackControls->setSizeConstraint(QLayout::SetFixedSize);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_videoWidget);
    layout->addLayout(playbackControls);
    layout->setContentsMargins({});
    setLayout(layout);
    show();
    setWindowState(Qt::WindowFullScreen);

    m_mediaObject->setCurrentSource(url);
    connect(m_mediaObject, &Phonon::MediaObject::finished, this, &FullScreenPlayer::stop);
    connect(m_mediaObject, &Phonon::MediaObject::stateChanged, this, &FullScreenPlayer::playStateChanged);
    connect(m_mediaObject, &Phonon::MediaObject::tick, this, &FullScreenPlayer::updatePlaybackTime);

    play();

    mute();
    unmute();
}

FullScreenPlayer::~FullScreenPlayer() = default;

void FullScreenPlayer::play()
{
    m_mediaObject->play();
}

void FullScreenPlayer::pause()
{
    m_mediaObject->pause();
}

void FullScreenPlayer::stop()
{
    m_mediaObject->stop();
    deleteLater();
}

void FullScreenPlayer::mute()
{
    qreal volume = m_audioOutput->volume();
    m_audioOutput->setMuted(true);
    m_audioOutput->setVolume(volume); //
}

void FullScreenPlayer::unmute()
{
    m_audioOutput->setMuted(false);
}

void FullScreenPlayer::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_mediaObject->stop();
    deleteLater();
}

void FullScreenPlayer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        m_mediaObject->stop();
        deleteLater();
    }
}

void FullScreenPlayer::playStateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);

    switch (newState) {
    case Phonon::PlayingState:
        m_play->setVisible(false);
        m_pause->setVisible(true);
        break;
    case Phonon::PausedState:
        m_play->setVisible(true);
        m_pause->setVisible(false);
        break;
    default:;
    }
}

void FullScreenPlayer::updatePlaybackTime(qint64 currentTime)
{
    QString currentPlayTime = QString("%1:%2:%3")
                                  .arg((currentTime / 3600000) % 60, 2, 10, QChar('0'))
                                  .arg((currentTime / 60000) % 60, 2, 10, QChar('0'))
                                  .arg((currentTime / 1000) % 60, 2, 10, QChar('0'));

    qint64 time = m_mediaObject->totalTime();
    QString totalTime = QString("%1:%2:%3")
                            .arg((time / 3600000) % 60, 2, 10, QChar('0'))
                            .arg((time / 60000) % 60, 2, 10, QChar('0'))
                            .arg((time / 1000) % 60, 2, 10, QChar('0'));

    m_playbackTime->setText(QString("%1/%2").arg(currentPlayTime, totalTime));
}

void FullScreenPlayer::muteStateChanged(bool muted)
{
    if (muted) {
        m_volumeIconMuted->setVisible(true);
        m_volumeIconUnmuted->setVisible(false);
    } else {
        m_volumeIconMuted->setVisible(false);
        m_volumeIconUnmuted->setVisible(true);
    }
}
