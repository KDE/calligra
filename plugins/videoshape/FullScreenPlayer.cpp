/* This file is part of the KDE project
 * Copyright (C) 2010 Casper Boemann <cbo@boemann.dk>
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
 
#include "FullScreenPlayer.h"

#ifndef CALLIGRA_NO_PHONON
#include <phonon/videowidget.h>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#endif

#include <QUrl>
#include <QVBoxLayout>
#include <QKeyEvent>

FullScreenPlayer::FullScreenPlayer(const QUrl &url)
    : QWidget(0)
    , m_mediaObject(0)
    , m_videoWidget(0)
    , m_audioOutput(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);

#ifndef CALLIGRA_NO_PHONON
    m_mediaObject = new Phonon::MediaObject();

    m_videoWidget = new Phonon::VideoWidget(this);
    Phonon::createPath(m_mediaObject, m_videoWidget);

    m_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory);
    Phonon::createPath(m_mediaObject, m_audioOutput);

    layout->addWidget(m_videoWidget);
#endif

    setLayout(layout);
    show();
    setWindowState(Qt::WindowFullScreen);

#ifndef CALLIGRA_NO_PHONON
    m_mediaObject->setCurrentSource(url);
    connect(m_mediaObject, SIGNAL(finished()), this, SLOT(stop()));
    m_mediaObject->play();
#endif
}

FullScreenPlayer::~FullScreenPlayer()
{
}

void FullScreenPlayer::stop()
{
    deleteLater();
}

void FullScreenPlayer::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
#ifndef CALLIGRA_NO_PHONON
    m_mediaObject->stop();
#endif
    deleteLater();
}

void FullScreenPlayer::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape) {
#ifndef CALLIGRA_NO_PHONON
       m_mediaObject->stop();
#endif
       deleteLater();
    }
}

#include <FullScreenPlayer.moc>
